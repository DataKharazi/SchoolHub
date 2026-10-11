#include "Database.h"
#include "External/SQLite/sqlite3.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>

namespace
{
    // Finalize prepared statements on every path, including exceptions.
    class Statement
    {
        sqlite3* db;
        sqlite3_stmt* statement = nullptr;
    public:
        Statement(sqlite3* db, const std::string& sql) : db(db)
        {
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(db));
        }
        ~Statement() { sqlite3_finalize(statement); }
        Statement(const Statement&) = delete;
        Statement& operator=(const Statement&) = delete;

        void check(int result)
        {
            if (result != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db));
        }
        void integer(int parameter, std::int64_t value)
        {
            check(sqlite3_bind_int64(statement, parameter, value));
        }
        void real(int parameter, double value)
        {
            if (!std::isfinite(value)) throw std::invalid_argument("Cannot persist non-finite numbers.");
            check(sqlite3_bind_double(statement, parameter, value));
        }
        void text(int parameter, const std::string& value)
        {
            if (value.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
                throw std::invalid_argument("Text exceeds SQLite binding size.");
            check(sqlite3_bind_text(statement, parameter, value.data(),
                static_cast<int>(value.size()), SQLITE_TRANSIENT));
        }
        void null(int parameter) { check(sqlite3_bind_null(statement, parameter)); }
        bool row()
        {
            int result = sqlite3_step(statement);
            if (result == SQLITE_ROW) return true;
            if (result == SQLITE_DONE) return false;
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        void run()
        {
            if (row()) throw std::runtime_error("Unexpected row from a write operation.");
        }
        std::int64_t integer(int column) const { return sqlite3_column_int64(statement, column); }
        int smallInteger(int column) const
        {
            auto value = integer(column);
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
                throw std::runtime_error("Stored integer is outside the domain range.");
            return static_cast<int>(value);
        }
        double real(int column) const
        {
            double value = sqlite3_column_double(statement, column);
            if (!std::isfinite(value)) throw std::runtime_error("Stored number is not finite.");
            return value;
        }
        std::string text(int column) const
        {
            const auto* value = sqlite3_column_text(statement, column);
            return value ? std::string(reinterpret_cast<const char*>(value),
                sqlite3_column_bytes(statement, column)) : std::string();
        }
        bool isNull(int column) const { return sqlite3_column_type(statement, column) == SQLITE_NULL; }
    };

    void run(sqlite3* db, const std::string& sql)
    {
        Statement statement(db, sql);
        statement.run();
    }

    // These operations own their transaction; never commit/rollback a caller's transaction.
    void requireReady(sqlite3* db)
    {
        if (!db) throw std::runtime_error("Database is not open.");
        Statement version(db, "PRAGMA user_version;");
        if (!version.row() || version.integer(0) != 1)
            throw std::runtime_error("Initialize the version 1 schema with createTables() first.");
        Statement keys(db, "PRAGMA foreign_keys;");
        if (!keys.row() || keys.integer(0) != 1)
            throw std::runtime_error("Foreign key enforcement must be enabled.");
        if (!sqlite3_get_autocommit(db))
            throw std::runtime_error("Persistence operations cannot run inside a caller transaction.");
    }

    void rollback(sqlite3* db)
    {
        // Preserve the original error if rollback itself encounters a problem.
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }

    void bindDate(Statement& statement, int first, const Date& date)
    {
        if (!date.isValid()) throw std::invalid_argument("Cannot persist an invalid date.");
        statement.integer(first, date.getYear());
        statement.integer(first + 1, date.getMonth());
        statement.integer(first + 2, date.getDay());
    }

    Date readDate(const Statement& statement, int first)
    {
        Date date(statement.smallInteger(first), statement.smallInteger(first + 1),
            statement.smallInteger(first + 2));
        if (!date.isValid()) throw std::runtime_error("Database contains an invalid date.");
        return date;
    }

    void bindGrade(Statement& statement, int first, const std::optional<Grade>& grade)
    {
        if (grade)
        {
            statement.real(first, grade->getEarnedPoints());
            statement.real(first + 1, grade->getMaxPoints());
        }
        else
        {
            statement.null(first);
            statement.null(first + 1);
        }
    }

    std::optional<Grade> readGrade(const Statement& statement, int first)
    {
        if (statement.isNull(first) && statement.isNull(first + 1)) return std::nullopt;
        if (statement.isNull(first) || statement.isNull(first + 1))
            throw std::runtime_error("Database contains an incomplete grade.");
        return Grade(statement.real(first), statement.real(first + 1));
    }

    void validateTime(const Time& time)
    {
        if (time.getHour() < 0 || time.getHour() > 23 || time.getMinute() < 0 || time.getMinute() > 59)
            throw std::invalid_argument("Meeting time must be a valid hour/minute.");
    }

    // Table/column names below come only from fixed source-code strings, never user input.
    // Existing IDs must still exist under the same parent; names never establish ownership.
    void checkIdentity(sqlite3* db, const std::string& table, std::int64_t id,
        const std::string& parentColumn, std::int64_t parentId)
    {
        if (id == 0) return;
        if (id < 0) throw std::invalid_argument("Invalid entity ID.");
        Statement owner(db, "SELECT " + (parentColumn.empty() ? "id" : parentColumn)
            + " FROM " + table + " WHERE id = ?;");
        owner.integer(1, id);
        if (!owner.row()) throw std::runtime_error("Stale or unknown " + table + " ID.");
        if (!parentColumn.empty() && owner.integer(0) != parentId)
            throw std::runtime_error("Ownership mismatch for " + table + "; refusing to move another parent's data.");
    }

    void recordId(std::set<std::int64_t>& ids, std::int64_t id)
    {
        if (!ids.insert(id).second)
            throw std::runtime_error("The same saved entity occurs more than once in a collection.");
    }

    void pruneChildren(sqlite3* db, const std::string& table, const std::string& parentColumn,
        std::int64_t parentId, const std::set<std::int64_t>& retained)
    {
        std::vector<std::int64_t> removed;
        {
            Statement rows(db, "SELECT id FROM " + table + " WHERE " + parentColumn + " = ?;");
            rows.integer(1, parentId);
            while (rows.row())
                if (!retained.count(rows.integer(0))) removed.push_back(rows.integer(0));
        }
        for (auto id : removed)
        {
            Statement remove(db, "DELETE FROM " + table + " WHERE id = ? AND " + parentColumn + " = ?;");
            remove.integer(1, id);
            remove.integer(2, parentId);
            remove.run();
        }
    }
}


bool Database::saveStudent(Student& student)
{
    bool transaction = false;
    try
    {
        requireReady(database);
        Student saved = student; // Do not expose new IDs if any later row fails.
        run(database, "BEGIN IMMEDIATE;");
        transaction = true;
        checkIdentity(database, "students", saved.id, "", 0);
        {
            Statement save(database, "INSERT INTO students (id, name, gpa) VALUES (?, ?, ?) "
                "ON CONFLICT(id) DO UPDATE SET name=excluded.name, gpa=excluded.gpa;");
            if (saved.id == 0) save.null(1); else save.integer(1, saved.id);
            save.text(2, saved.getStudentName());
            save.real(3, saved.getGPA());
            save.run();
            if (saved.id == 0) saved.id = sqlite3_last_insert_rowid(database);
        }
        std::set<std::int64_t> schoolIds;
        for (size_t position = 0; position < saved.schools.size(); ++position)
        {
            auto& school = saved.schools[position];
            checkIdentity(database, "schools", school.id, "student_id", saved.id);
            {
                Statement save(database, "INSERT INTO schools (id, student_id, position, name) VALUES (?, ?, ?, ?) "
                    "ON CONFLICT(id) DO UPDATE SET position=excluded.position, name=excluded.name;");
                if (school.id == 0) save.null(1); else save.integer(1, school.id);
                save.integer(2, saved.id);
                save.integer(3, static_cast<std::int64_t>(position));
                save.text(4, school.getSchoolName());
                save.run();
                if (school.id == 0) school.id = sqlite3_last_insert_rowid(database);
            }
            recordId(schoolIds, school.id);
            std::set<std::int64_t> classIds;
            for (size_t position = 0; position < school.studyClasses.size(); ++position)
            {
                auto& course = school.studyClasses[position];
                checkIdentity(database, "study_classes", course.id, "school_id", school.id);
                {
                    Statement save(database, "INSERT INTO study_classes (id, school_id, position, name, credits, start_year, start_month, start_day, end_year, end_month, end_day) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                        "ON CONFLICT(id) DO UPDATE SET position=excluded.position, name=excluded.name, credits=excluded.credits, start_year=excluded.start_year, start_month=excluded.start_month, start_day=excluded.start_day, end_year=excluded.end_year, end_month=excluded.end_month, end_day=excluded.end_day;");
                    if (course.id == 0) save.null(1); else save.integer(1, course.id);
                    save.integer(2, school.id);
                    save.integer(3, static_cast<std::int64_t>(position));
                    save.text(4, course.getClassName());
                    save.real(5, course.getCredits());
                    bindDate(save, 6, course.getStartDate());
                    bindDate(save, 9, course.getEndDate());
                    save.run();
                    if (course.id == 0) course.id = sqlite3_last_insert_rowid(database);
                }
                recordId(classIds, course.id);
                std::set<std::int64_t> assignmentIds;
                for (size_t position = 0; position < course.assignments.size(); ++position)
                {
                    auto& assignment = course.assignments[position];
                    checkIdentity(database, "assignments", assignment.id, "study_class_id", course.id);
                    {
                        Statement save(database, "INSERT INTO assignments (id, study_class_id, position, description, start_year, start_month, start_day, due_year, due_month, due_day, finished, earned_points, max_points) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                            "ON CONFLICT(id) DO UPDATE SET position=excluded.position, description=excluded.description, start_year=excluded.start_year, start_month=excluded.start_month, start_day=excluded.start_day, due_year=excluded.due_year, due_month=excluded.due_month, due_day=excluded.due_day, finished=excluded.finished, earned_points=excluded.earned_points, max_points=excluded.max_points;");
                        if (assignment.id == 0) save.null(1); else save.integer(1, assignment.id);
                        save.integer(2, course.id);
                        save.integer(3, static_cast<std::int64_t>(position));
                        save.text(4, assignment.getDescription());
                        bindDate(save, 5, assignment.getStartDate());
                        bindDate(save, 8, assignment.getDueDate());
                        save.integer(11, assignment.isFinished() ? 1 : 0);
                        bindGrade(save, 12, assignment.getGrade());
                        save.run();
                        if (assignment.id == 0) assignment.id = sqlite3_last_insert_rowid(database);
                    }
                    recordId(assignmentIds, assignment.id);
                }
                pruneChildren(database, "assignments", "study_class_id", course.id, assignmentIds);
                std::set<std::int64_t> examIds;
                for (size_t position = 0; position < course.exams.size(); ++position)
                {
                    auto& exam = course.exams[position];
                    checkIdentity(database, "exams", exam.id, "study_class_id", course.id);
                    {
                        Statement save(database, "INSERT INTO exams (id, study_class_id, position, name, exam_year, exam_month, exam_day, earned_points, max_points) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) "
                            "ON CONFLICT(id) DO UPDATE SET position=excluded.position, name=excluded.name, exam_year=excluded.exam_year, exam_month=excluded.exam_month, exam_day=excluded.exam_day, earned_points=excluded.earned_points, max_points=excluded.max_points;");
                        if (exam.id == 0) save.null(1); else save.integer(1, exam.id);
                        save.integer(2, course.id);
                        save.integer(3, static_cast<std::int64_t>(position));
                        save.text(4, exam.getExamName());
                        bindDate(save, 5, exam.getExamDate());
                        bindGrade(save, 8, exam.getGrade());
                        save.run();
                        if (exam.id == 0) exam.id = sqlite3_last_insert_rowid(database);
                    }
                    recordId(examIds, exam.id);
                }
                pruneChildren(database, "exams", "study_class_id", course.id, examIds);
                std::set<std::int64_t> bookIds;
                for (size_t position = 0; position < course.books.size(); ++position)
                {
                    auto& book = course.books[position];
                    checkIdentity(database, "books", book.id, "study_class_id", course.id);
                    {
                        Statement save(database, "INSERT INTO books (id, study_class_id, position, name, file_path, bookmark) VALUES (?, ?, ?, ?, ?, ?) "
                            "ON CONFLICT(id) DO UPDATE SET position=excluded.position, name=excluded.name, file_path=excluded.file_path, bookmark=excluded.bookmark;");
                        if (book.id == 0) save.null(1); else save.integer(1, book.id);
                        save.integer(2, course.id);
                        save.integer(3, static_cast<std::int64_t>(position));
                        save.text(4, book.getBookName());
                        save.text(5, book.getFilePath());
                        save.integer(6, book.getBookMark());
                        save.run();
                        if (book.id == 0) book.id = sqlite3_last_insert_rowid(database);
                    }
                    recordId(bookIds, book.id);
                }
                pruneChildren(database, "books", "study_class_id", course.id, bookIds);
                std::set<std::int64_t> meetingIds;
                for (size_t position = 0; position < course.classMeetings.size(); ++position)
                {
                    auto& meeting = course.classMeetings[position];
                    checkIdentity(database, "class_meetings", meeting.id, "study_class_id", course.id);
                    {
                        Statement save(database, "INSERT INTO class_meetings (id, study_class_id, position, weekday, start_hour, start_minute, end_hour, end_minute) VALUES (?, ?, ?, ?, ?, ?, ?, ?) "
                            "ON CONFLICT(id) DO UPDATE SET position=excluded.position, weekday=excluded.weekday, start_hour=excluded.start_hour, start_minute=excluded.start_minute, end_hour=excluded.end_hour, end_minute=excluded.end_minute;");
                        if (meeting.id == 0) save.null(1); else save.integer(1, meeting.id);
                        save.integer(2, course.id);
                        save.integer(3, static_cast<std::int64_t>(position));
                        validateTime(meeting.getStartTime());
                        validateTime(meeting.getEndTime());
                        save.integer(4, static_cast<int>(meeting.getWeekDay()));
                        save.integer(5, meeting.getStartTime().getHour());
                        save.integer(6, meeting.getStartTime().getMinute());
                        save.integer(7, meeting.getEndTime().getHour());
                        save.integer(8, meeting.getEndTime().getMinute());
                        save.run();
                        if (meeting.id == 0) meeting.id = sqlite3_last_insert_rowid(database);
                    }
                    recordId(meetingIds, meeting.id);
                }
                pruneChildren(database, "class_meetings", "study_class_id", course.id, meetingIds);
            }
            pruneChildren(database, "study_classes", "school_id", school.id, classIds);
        }
        pruneChildren(database, "schools", "student_id", saved.id, schoolIds);
        run(database, "COMMIT;");
        student = std::move(saved);
        lastError.clear();
        return true;
    }
    catch (const std::exception& error)
    {
        if (transaction) rollback(database);
        lastError = error.what();
        return false;
    }
}


Student Database::loadStudentHierarchy(std::int64_t id)
{
    Statement studentRow(database, "SELECT id, name, gpa FROM students WHERE id = ?;");
    studentRow.integer(1, id);
    if (!studentRow.row()) throw std::runtime_error("Student not found.");
    Student student(studentRow.text(1), studentRow.real(2));
    student.id = studentRow.integer(0);

    Statement schools(database, "SELECT id, name FROM schools WHERE student_id = ? ORDER BY position, id;");
    schools.integer(1, student.id);
    while (schools.row())
    {
        School school(schools.text(1));
        school.id = schools.integer(0);
        Statement classes(database, "SELECT id, name, credits, start_year, start_month, start_day, "
            "end_year, end_month, end_day FROM study_classes WHERE school_id = ? ORDER BY position, id;");
        classes.integer(1, school.id);
        while (classes.row())
        {
            StudyClass course(classes.text(1), classes.real(2), readDate(classes, 3), readDate(classes, 6));
            course.id = classes.integer(0);

            Statement assignments(database, "SELECT id, description, start_year, start_month, start_day, "
                "due_year, due_month, due_day, finished, earned_points, max_points "
                "FROM assignments WHERE study_class_id = ? ORDER BY position, id;");
            assignments.integer(1, course.id);
            while (assignments.row())
            {
                Assignment assignment(assignments.text(1), readDate(assignments, 2), readDate(assignments, 5));
                assignment.id = assignments.integer(0);
                assignment.setFinished(assignments.integer(8) != 0);
                assignment.grade = readGrade(assignments, 9);
                course.addAssignment(assignment);
            }

            Statement exams(database, "SELECT id, name, exam_year, exam_month, exam_day, earned_points, max_points "
                "FROM exams WHERE study_class_id = ? ORDER BY position, id;");
            exams.integer(1, course.id);
            while (exams.row())
            {
                Exam exam(exams.text(1), readDate(exams, 2));
                exam.id = exams.integer(0);
                exam.grade = readGrade(exams, 5);
                course.addExam(exam);
            }

            Statement books(database, "SELECT id, name, file_path, bookmark FROM books "
                "WHERE study_class_id = ? ORDER BY position, id;");
            books.integer(1, course.id);
            while (books.row())
            {
                Book book(books.text(1), books.text(2));
                book.id = books.integer(0);
                book.setBookMark(books.smallInteger(3));
                course.addBook(book);
            }

            Statement meetings(database, "SELECT id, weekday, start_hour, start_minute, end_hour, end_minute "
                "FROM class_meetings WHERE study_class_id = ? ORDER BY position, id;");
            meetings.integer(1, course.id);
            while (meetings.row())
            {
                int weekday = meetings.smallInteger(1);
                if (weekday < 0 || weekday > 6) throw std::runtime_error("Invalid stored weekday.");
                Time start(meetings.smallInteger(2), meetings.smallInteger(3));
                Time end(meetings.smallInteger(4), meetings.smallInteger(5));
                validateTime(start);
                validateTime(end);
                ClassMeeting meeting(static_cast<WeekDay>(weekday), start, end);
                meeting.id = meetings.integer(0);
                course.addClassMeeting(meeting);
            }
            school.addStudyClass(course);
        }
        student.addSchool(school);
    }
    return student;
}

std::optional<Student> Database::loadStudent(std::int64_t id)
{
    bool transaction = false;
    try
    {
        requireReady(database);
        run(database, "BEGIN;");
        transaction = true;
        Student student = loadStudentHierarchy(id);
        run(database, "COMMIT;");
        lastError.clear();
        return student;
    }
    catch (const std::exception& error)
    {
        if (transaction) rollback(database);
        lastError = error.what();
        return std::nullopt;
    }
}

std::vector<Student> Database::loadStudents()
{
    bool transaction = false;
    try
    {
        requireReady(database);
        run(database, "BEGIN;");
        transaction = true;
        std::vector<Student> students;
        {
            Statement rows(database, "SELECT id FROM students ORDER BY id;");
            while (rows.row()) students.push_back(loadStudentHierarchy(rows.integer(0)));
        }
        run(database, "COMMIT;");
        lastError.clear();
        return students;
    }
    catch (const std::exception& error)
    {
        if (transaction) rollback(database);
        lastError = error.what();
        return {};
    }
}

bool Database::deleteStudent(std::int64_t id)
{
    try
    {
        requireReady(database);
        Statement remove(database, "DELETE FROM students WHERE id = ?;");
        remove.integer(1, id);
        remove.run();
        if (sqlite3_changes(database) != 1) throw std::runtime_error("Student not found.");
        lastError.clear();
        return true;
    }
    catch (const std::exception& error)
    {
        lastError = error.what();
        return false;
    }
}

bool Database::deleteSchool(std::int64_t studentId, std::int64_t schoolId)
{
    try
    {
        requireReady(database);
        Statement remove(database, "DELETE FROM schools WHERE id = ? AND student_id = ?;");
        remove.integer(1, schoolId);
        remove.integer(2, studentId);
        remove.run();
        if (sqlite3_changes(database) != 1)
            throw std::runtime_error("School not found for this student.");
        lastError.clear();
        return true;
    }
    catch (const std::exception& error)
    {
        lastError = error.what();
        return false;
    }
}

bool Database::createTables()
{
    bool transaction = false;
    try
    {
        if (!database) throw std::runtime_error("Database is not open.");
        if (!sqlite3_get_autocommit(database))
            throw std::runtime_error("Schema initialization cannot run inside a caller transaction.");
        int version;
        {
            Statement query(database, "PRAGMA user_version;");
            query.row();
            version = query.smallInteger(0);
        }
        if (version != 0 && version != 1)
            throw std::runtime_error("Unsupported database schema version.");
        if (version == 0)
        {
            Statement tables(database, "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';");
            if (tables.row())
                throw std::runtime_error("Old development schema detected. Back up, then delete and recreate the development database.");
        }
        run(database, "BEGIN IMMEDIATE;");
        transaction = true;
        if (!execute(R"(CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            gpa REAL NOT NULL DEFAULT 0.0
        );)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS schools (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL REFERENCES students(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            name TEXT NOT NULL
        );
        CREATE INDEX IF NOT EXISTS schools_parent ON schools(student_id);)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS study_classes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            school_id INTEGER NOT NULL REFERENCES schools(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            name TEXT NOT NULL, credits REAL NOT NULL,
            start_year INTEGER NOT NULL CHECK(start_year BETWEEN 1 AND 2147483647),
            start_month INTEGER NOT NULL CHECK(start_month BETWEEN 1 AND 12),
            start_day INTEGER NOT NULL CHECK(start_day BETWEEN 1 AND 31), end_year INTEGER NOT NULL CHECK(end_year BETWEEN 1 AND 2147483647),
            end_month INTEGER NOT NULL CHECK(end_month BETWEEN 1 AND 12),
            end_day INTEGER NOT NULL CHECK(end_day BETWEEN 1 AND 31)
        );
        CREATE INDEX IF NOT EXISTS study_classes_parent ON study_classes(school_id);)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS assignments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            study_class_id INTEGER NOT NULL REFERENCES study_classes(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            description TEXT NOT NULL,
            start_year INTEGER NOT NULL CHECK(start_year BETWEEN 1 AND 2147483647),
            start_month INTEGER NOT NULL CHECK(start_month BETWEEN 1 AND 12),
            start_day INTEGER NOT NULL CHECK(start_day BETWEEN 1 AND 31), due_year INTEGER NOT NULL CHECK(due_year BETWEEN 1 AND 2147483647),
            due_month INTEGER NOT NULL CHECK(due_month BETWEEN 1 AND 12),
            due_day INTEGER NOT NULL CHECK(due_day BETWEEN 1 AND 31),
            finished INTEGER NOT NULL CHECK(finished IN (0, 1)),
            earned_points REAL, max_points REAL,
            CHECK((earned_points IS NULL AND max_points IS NULL) OR
                (earned_points IS NOT NULL AND max_points IS NOT NULL AND earned_points >= 0 AND max_points > 0))
        );
        CREATE INDEX IF NOT EXISTS assignments_parent ON assignments(study_class_id);)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS exams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            study_class_id INTEGER NOT NULL REFERENCES study_classes(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            name TEXT NOT NULL,
            exam_year INTEGER NOT NULL CHECK(exam_year BETWEEN 1 AND 2147483647),
            exam_month INTEGER NOT NULL CHECK(exam_month BETWEEN 1 AND 12),
            exam_day INTEGER NOT NULL CHECK(exam_day BETWEEN 1 AND 31), earned_points REAL, max_points REAL,
            CHECK((earned_points IS NULL AND max_points IS NULL) OR
                (earned_points IS NOT NULL AND max_points IS NOT NULL AND earned_points >= 0 AND max_points > 0))
        );
        CREATE INDEX IF NOT EXISTS exams_parent ON exams(study_class_id);)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            study_class_id INTEGER NOT NULL REFERENCES study_classes(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            name TEXT NOT NULL, file_path TEXT NOT NULL, bookmark INTEGER NOT NULL
        );
        CREATE INDEX IF NOT EXISTS books_parent ON books(study_class_id);)")) throw std::runtime_error(lastError);
        if (!execute(R"(CREATE TABLE IF NOT EXISTS class_meetings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            study_class_id INTEGER NOT NULL REFERENCES study_classes(id) ON DELETE CASCADE,
            position INTEGER NOT NULL CHECK(position >= 0),
            weekday INTEGER NOT NULL CHECK(weekday BETWEEN 0 AND 6),
            start_hour INTEGER NOT NULL CHECK(start_hour BETWEEN 0 AND 23),
            start_minute INTEGER NOT NULL CHECK(start_minute BETWEEN 0 AND 59),
            end_hour INTEGER NOT NULL CHECK(end_hour BETWEEN 0 AND 23),
            end_minute INTEGER NOT NULL CHECK(end_minute BETWEEN 0 AND 59)
        );
        CREATE INDEX IF NOT EXISTS class_meetings_parent ON class_meetings(study_class_id);)")) throw std::runtime_error(lastError);
        run(database, "PRAGMA user_version = 1;");
        run(database, "COMMIT;");
        lastError.clear();
        return true;
    }
    catch (const std::exception& error)
    {
        if (transaction) rollback(database);
        lastError = error.what();
        return false;
    }
}

