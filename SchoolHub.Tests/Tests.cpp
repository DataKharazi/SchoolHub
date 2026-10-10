#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../SchoolHub/Student.h"
#include "../SchoolHub/Calendar.h"
#include "../SchoolHub/Database.h"

// Unlike assert(), CHECK remains active in Release builds.
#define CHECK(condition) check((condition), #condition, __LINE__)

void check(bool condition, const char* expression, int line)
{
    if (!condition)
        throw std::runtime_error("line " + std::to_string(line) + ": " + expression);
}

bool sameTime(Time time, int hour, int minute)
{
    return time.getHour() == hour && time.getMinute() == minute;
}

StudyClass smallCourse()
{
    StudyClass course("Data Structures", "College", 3,
        Date(2026, 9, 7), Date(2026, 9, 16));
    course.addClassMeeting(ClassMeeting(WeekDay::Monday, Time(11, 30), Time(12, 45)));
    course.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(11, 30), Time(12, 45)));
    return course;
}

void checkSorted(const std::vector<CalendarEvent>& events)
{
    for (size_t i = 1; i < events.size(); ++i)
        CHECK(!(events[i].getDate() < events[i - 1].getDate()));
}

struct Test
{
    const char* name;
    void (*run)();
};

int main()
{
    const std::vector<Test> tests = {
        {"Date constructors and getters", [] {
            Date empty;
            CHECK(empty.getYear() == 0);
            CHECK(empty.getMonth() == 0);
            CHECK(empty.getDay() == 0);
            Date date(2026, 9, 26);
            CHECK(date.getYear() == 2026);
            CHECK(date.getMonth() == 9);
            CHECK(date.getDay() == 26);
        }},
        {"Date comparison operators", [] {
            Date date(2026, 9, 26);
            CHECK(date == Date(2026, 9, 26));
            CHECK(!(date == Date(2026, 9, 27)));
            CHECK(!(date == Date(2026, 10, 26)));
            CHECK(!(date == Date(2027, 9, 26)));
            CHECK(date < Date(2026, 9, 27));
            CHECK(date < Date(2026, 10, 1));
            CHECK(date < Date(2027, 1, 1));
            CHECK(date > Date(2026, 9, 25));
            CHECK(date > Date(2026, 8, 31));
            CHECK(date > Date(2025, 12, 31));
            CHECK(!(date < date));
            CHECK(!(date > date));
            CHECK(!(date < Date(2026, 9, 25)));
            CHECK(!(date > Date(2026, 9, 27)));
        }},
        {"Date next ordinary day", [] {
            CHECK(Date(2026, 9, 10).getNextDay() == Date(2026, 9, 11));
        }},
        {"Date month boundaries", [] {
            CHECK(Date(2026, 4, 30).getNextDay() == Date(2026, 5, 1));
            CHECK(Date(2026, 1, 31).getNextDay() == Date(2026, 2, 1));
        }},
        {"Date year boundary", [] {
            CHECK(Date(2026, 12, 31).getNextDay() == Date(2027, 1, 1));
        }},
        {"Date February in common years", [] {
            CHECK(Date(2026, 2, 28).getNextDay() == Date(2026, 3, 1));
            CHECK(Date(1900, 2, 28).getNextDay() == Date(1900, 3, 1));
        }},
        {"Date leap years including century rule", [] {
            CHECK(Date(2024, 2, 28).getNextDay() == Date(2024, 2, 29));
            CHECK(Date(2024, 2, 29).getNextDay() == Date(2024, 3, 1));
            CHECK(Date(2000, 2, 28).getNextDay() == Date(2000, 2, 29));
            CHECK(Date(2000, 2, 29).getNextDay() == Date(2000, 3, 1));
        }},
        {"Date known weekdays", [] {
            CHECK(Date(2000, 1, 1).getWeekDay() == WeekDay::Saturday);
            CHECK(Date(2024, 2, 29).getWeekDay() == WeekDay::Thursday);
            CHECK(Date(2026, 9, 1).getWeekDay() == WeekDay::Tuesday);
            CHECK(Date(2026, 9, 6).getWeekDay() == WeekDay::Sunday);
            CHECK(Date(2026, 9, 7).getWeekDay() == WeekDay::Monday);
            CHECK(Date(2026, 9, 9).getWeekDay() == WeekDay::Wednesday);
            CHECK(Date(2026, 9, 11).getWeekDay() == WeekDay::Friday);
        }},
        {"Time constructors getters and setters", [] {
            CHECK(sameTime(Time(), 0, 0));
            Time time(11, 30);
            CHECK(sameTime(time, 11, 30));
            time.setHour(12);
            time.setMinute(45);
            CHECK(sameTime(time, 12, 45));
        }},
        {"Assignment dynamic status and due-date boundary", [] {
            Assignment assignment("Homework", Date(2026, 9, 1), Date(2026, 9, 10));
            CHECK(!assignment.isFinished());
            CHECK(assignment.getStatus(Date(2026, 9, 9)) == AssignmentStatus::Active);
            CHECK(assignment.getStatus(Date(2026, 9, 10)) == AssignmentStatus::Active);
            CHECK(assignment.getStatus(Date(2026, 9, 11)) == AssignmentStatus::Overdue);
            assignment.setDueDate(Date(2026, 9, 12));
            CHECK(assignment.getStatus(Date(2026, 9, 11)) == AssignmentStatus::Active);
        }},
        {"Assignment Finished takes precedence", [] {
            Assignment assignment("Homework", Date(2026, 9, 1), Date(2026, 9, 10));
            assignment.setFinished(true);
            CHECK(assignment.isFinished());
            CHECK(assignment.getStatus(Date(2026, 9, 9)) == AssignmentStatus::Finished);
            CHECK(assignment.getStatus(Date(2026, 9, 11)) == AssignmentStatus::Finished);
            assignment.setFinished(false);
            CHECK(assignment.getStatus(Date(2026, 9, 11)) == AssignmentStatus::Overdue);
        }},
        {"ClassMeeting weekday and times", [] {
            ClassMeeting meeting(WeekDay::Monday, Time(11, 30), Time(12, 45));
            CHECK(meeting.getWeekDay() == WeekDay::Monday);
            CHECK(sameTime(meeting.getStartTime(), 11, 30));
            CHECK(sameTime(meeting.getEndTime(), 12, 45));
            meeting.setWeekDay(WeekDay::Wednesday);
            meeting.setStartTime(Time(9, 0));
            meeting.setEndTime(Time(10, 15));
            CHECK(meeting.getWeekDay() == WeekDay::Wednesday);
            CHECK(sameTime(meeting.getStartTime(), 9, 0));
            CHECK(sameTime(meeting.getEndTime(), 10, 15));
        }},
        {"StudyClass stores course data and collections", [] {
            StudyClass course = smallCourse();
            CHECK(course.getClassName() == "Data Structures");
            CHECK(course.getOrganizationName() == "College");
            CHECK(course.getCredits() == 3);
            CHECK(course.getStartDate() == Date(2026, 9, 7));
            CHECK(course.getEndDate() == Date(2026, 9, 16));
            CHECK(course.getBooks().empty());
            CHECK(course.getAssignments().empty());
            CHECK(course.getExams().empty());
            CHECK(course.getGrades().empty());
            Book book("Textbook", "books/textbook.pdf");
            book.setBookMark(125);
            course.addBook(book);
            course.addBook(Book("Reference", "books/reference.pdf"));
            course.addAssignment(Assignment("Homework", Date(2026, 9, 7), Date(2026, 9, 10)));
            course.addAssignment(Assignment("Project", Date(2026, 9, 8), Date(2026, 9, 16)));
            course.addExam(Exam("Quiz", Date(2026, 9, 11)));
            course.addExam(Exam("Final", Date(2026, 9, 16)));
            course.addGrade(95);
            course.addGrade(88);
            CHECK(course.getBooks().size() == 2);
            CHECK(course.getBooks()[0].getBookName() == "Textbook");
            CHECK(course.getBooks()[0].getFilePath() == "books/textbook.pdf");
            CHECK(course.getBooks()[0].getBookMark() == 125);
            CHECK(course.getBooks()[1].getBookName() == "Reference");
            CHECK(course.getAssignments().size() == 2);
            CHECK(course.getAssignments()[0].getDescription() == "Homework");
            CHECK(course.getAssignments()[0].getStartDate() == Date(2026, 9, 7));
            CHECK(course.getAssignments()[0].getDueDate() == Date(2026, 9, 10));
            CHECK(course.getAssignments()[1].getDescription() == "Project");
            CHECK(course.getExams().size() == 2);
            CHECK(course.getExams()[0].getExamName() == "Quiz");
            CHECK(course.getExams()[0].getExamDate() == Date(2026, 9, 11));
            CHECK(course.getExams()[1].getExamName() == "Final");
            CHECK(course.getClassMeetings().size() == 2);
            CHECK(course.getClassMeetings()[0].getWeekDay() == WeekDay::Monday);
            CHECK(course.getClassMeetings()[1].getWeekDay() == WeekDay::Wednesday);
            CHECK(sameTime(course.getClassMeetings()[0].getStartTime(), 11, 30));
            CHECK(sameTime(course.getClassMeetings()[0].getEndTime(), 12, 45));
            CHECK(course.getGrades() == std::vector<double>({95, 88}));
        }},
        {"Student data and course collection", [] {
            Student student("David", 3.5);
            CHECK(student.getStudentName() == "David");
            CHECK(student.getGPA() == 3.5);
            CHECK(student.getStudyClasses().empty());
            student.setStudentName("David Kharazi");
            student.setGPA(3.75);
            CHECK(student.getStudentName() == "David Kharazi");
            CHECK(student.getGPA() == 3.75);
            student.addStudyClass(smallCourse());
            student.addStudyClass(StudyClass("Math", "College", 4, Date(2026, 9, 1), Date(2026, 12, 20)));
            CHECK(student.getStudyClasses().size() == 2);
            CHECK(student.getStudyClasses()[0].getClassName() == "Data Structures");
            CHECK(student.getStudyClasses()[0].getClassMeetings().size() == 2);
            CHECK(student.getStudyClasses()[1].getClassName() == "Math");
        }},
        {"CalendarEvent without times", [] {
            CalendarEvent event("Homework", Date(2026, 9, 10), EventType::Assignment);
            CHECK(event.getTitle() == "Homework");
            CHECK(event.getDate() == Date(2026, 9, 10));
            CHECK(event.getType() == EventType::Assignment);
            CHECK(!event.hasTime());
            CHECK(!event.getStartTime().has_value());
            CHECK(!event.getEndTime().has_value());
        }},
        {"CalendarEvent with times", [] {
            CalendarEvent event("Lecture", Date(2026, 9, 7), EventType::ClassMeeting, Time(11, 30), Time(12, 45));
            CHECK(event.getTitle() == "Lecture");
            CHECK(event.getDate() == Date(2026, 9, 7));
            CHECK(event.getType() == EventType::ClassMeeting);
            CHECK(event.hasTime());
            CHECK(sameTime(event.getStartTime().value(), 11, 30));
            CHECK(sameTime(event.getEndTime().value(), 12, 45));
        }},
        {"Calendar exact recurring dates and inclusive semester boundaries", [] {
            StudyClass course = smallCourse();
            auto events = Calendar().generateClassEvents(course);
            const std::vector<Date> expected = {Date(2026, 9, 7), Date(2026, 9, 9), Date(2026, 9, 14), Date(2026, 9, 16)};
            CHECK(events.size() == expected.size());
            for (size_t i = 0; i < events.size(); ++i) {
                CHECK(events[i].getDate() == expected[i]);
                CHECK(!(events[i].getDate() < course.getStartDate()));
                CHECK(!(events[i].getDate() > course.getEndDate()));
                auto day = events[i].getDate().getWeekDay();
                CHECK(day == WeekDay::Monday || day == WeekDay::Wednesday);
                CHECK(events[i].getType() == EventType::ClassMeeting);
                CHECK(events[i].getTitle() == course.getClassName());
                CHECK(events[i].hasTime());
                CHECK(sameTime(events[i].getStartTime().value(), 11, 30));
                CHECK(sameTime(events[i].getEndTime().value(), 12, 45));
            }
        }},
        {"Calendar semester with no matching meetings", [] {
            auto course = smallCourse();
            course.setStartDate(Date(2026, 9, 8));
            course.setEndDate(Date(2026, 9, 8));
            CHECK(Calendar().generateClassEvents(course).empty());
        }},
        {"Calendar converts and sorts deadlines", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment("Later homework", Date(2026, 9, 7), Date(2026, 9, 15)));
            course.addAssignment(Assignment("Earlier homework", Date(2026, 9, 7), Date(2026, 9, 10)));
            course.addExam(Exam("Quiz", Date(2026, 9, 11)));
            auto events = Calendar().generateDeadlineEvents(course);
            CHECK(events.size() == 3);
            CHECK(events[0].getTitle() == "Earlier homework");
            CHECK(events[0].getType() == EventType::Assignment);
            CHECK(events[0].getDate() == Date(2026, 9, 10));
            CHECK(events[1].getTitle() == "Quiz");
            CHECK(events[1].getType() == EventType::Exam);
            CHECK(events[1].getDate() == Date(2026, 9, 11));
            CHECK(events[2].getTitle() == "Later homework");
            CHECK(events[2].getDate() == Date(2026, 9, 15));
            for (const auto& event : events) CHECK(!event.hasTime());
            checkSorted(events);
        }},
        {"Calendar combines all types in date order", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment("Homework", Date(2026, 9, 7), Date(2026, 9, 14)));
            course.addExam(Exam("Quiz", Date(2026, 9, 8)));
            auto events = Calendar().generateAllEvents(course);
            CHECK(events.size() == 6);
            int meetings = 0, assignments = 0, exams = 0;
            for (const auto& event : events) {
                if (event.getType() == EventType::ClassMeeting) ++meetings;
                if (event.getType() == EventType::Assignment) ++assignments;
                if (event.getType() == EventType::Exam) ++exams;
            }
            CHECK(meetings == 4);
            CHECK(assignments == 1);
            CHECK(exams == 1);
            checkSorted(events);
        }},
        {"Calendar known sample has 31 meetings and 35 events", [] {
            auto course = smallCourse();
            course.setStartDate(Date(2026, 9, 1));
            course.setEndDate(Date(2026, 12, 20));
            course.addAssignment(Assignment("Set ADT Assignment", Date(2026, 9, 20), Date(2026, 10, 5)));
            course.addAssignment(Assignment("Old Homework", Date(2026, 9, 10), Date(2026, 9, 20)));
            Assignment finished("Assignment 1", Date(2026, 9, 1), Date(2026, 9, 15));
            finished.setFinished(true);
            course.addAssignment(finished);
            course.addExam(Exam("Test 1", Date(2026, 10, 14)));
            CHECK(Calendar().generateClassEvents(course).size() == 31);
            auto events = Calendar().generateAllEvents(course);
            CHECK(events.size() == 35);
            checkSorted(events);
        }},
        {"Database open create current table and close", [] {
            Database database;
            CHECK(!database.isOpen());
            CHECK(database.open(":memory:"));
            CHECK(database.isOpen());
            CHECK(database.createTables());
            CHECK(database.createTables());
            CHECK(database.execute("INSERT INTO students (name) VALUES ('Test student');"));
            CHECK(database.execute("SELECT id, name, previous_gpa, previous_credits FROM students;"));
            CHECK(database.getLastError().empty());
            database.close();
            CHECK(!database.isOpen());
            database.close();
            CHECK(!database.isOpen());
        }},
        {"Database closed-operation error", [] {
            Database database;
            CHECK(!database.execute("SELECT 1;"));
            CHECK(database.getLastError() == "Database is not open.");
            CHECK(!database.createTables());
        }},
        {"Database SQL failure and error recovery", [] {
            Database database;
            CHECK(database.open(":memory:"));
            CHECK(!database.execute("THIS IS NOT SQL;"));
            CHECK(!database.getLastError().empty());
            CHECK(database.isOpen());
            CHECK(database.execute("SELECT 1;"));
            CHECK(database.getLastError().empty());
        }},
        {"Database reopening creates an isolated in-memory database", [] {
            Database database;
            CHECK(database.open(":memory:"));
            CHECK(database.createTables());
            CHECK(database.open(":memory:"));
            CHECK(database.isOpen());
            CHECK(!database.execute("SELECT * FROM students;"));
            CHECK(database.createTables());
            CHECK(database.getLastError().empty());
        }}
    };

    size_t passed = 0;
    for (const auto& test : tests) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS: " << test.name << '\n';
        }
        catch (const std::exception& error) {
            std::cerr << "FAIL: " << test.name << " -- " << error.what() << '\n';
        }
        catch (...) {
            std::cerr << "FAIL: " << test.name << " -- unknown exception\n";
        }
    }
    std::cout << passed << " passed, " << tests.size() - passed << " failed\n";
    return passed == tests.size() ? 0 : 1;
}
