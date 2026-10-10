#include <iostream>
#include <cmath>
#include <limits>
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

bool near(double actual, double expected)
{
    return std::abs(actual - expected) < 0.000001;
}

template <typename Exception, typename Action>
void expectThrows(Action action)
{
    try {
        action();
    }
    catch (const Exception&) {
        return;
    }
    throw std::runtime_error("Expected exception was not thrown");
}

StudyClass smallCourse()
{
    StudyClass course("Data Structures", 3,
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

School querySchool()
{
    School school("Query school");
    StudyClass later("Later class", 3, Date(2026, 9, 8), Date(2026, 9, 9));
    later.addClassMeeting(ClassMeeting(WeekDay::Monday, Time(12, 0), Time(13, 0)));
    later.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(10, 30), Time(11, 30)));
    later.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(9, 30), Time(10, 0)));
    later.addAssignment(Assignment("Future assignment", Date(2026, 9, 1), Date(2026, 9, 11)));
    Assignment today("Today assignment", Date(2026, 9, 1), Date(2026, 9, 9));
    today.setFinished(true); // Queries add no completion-status filtering rule.
    later.addAssignment(today);
    later.addAssignment(Assignment("Past assignment", Date(2026, 9, 1), Date(2026, 9, 8)));
    later.addExam(Exam("Future exam", Date(2026, 9, 12)));
    later.addExam(Exam("Past exam", Date(2026, 9, 6)));
    later.addExam(Exam("Next week exam", Date(2026, 9, 14)));
    school.addStudyClass(later);

    StudyClass earlier("Earlier class", 3, Date(2026, 9, 1), Date(2026, 9, 13));
    earlier.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(9, 0), Time(10, 0)));
    earlier.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(10, 30), Time(12, 0)));
    earlier.addClassMeeting(ClassMeeting(WeekDay::Monday, Time(8, 0), Time(9, 0)));
    earlier.addClassMeeting(ClassMeeting(WeekDay::Sunday, Time(8, 0), Time(9, 0)));
    earlier.addAssignment(Assignment("Monday assignment", Date(2026, 9, 1), Date(2026, 9, 7)));
    earlier.addExam(Exam("Sunday exam", Date(2026, 9, 13)));
    earlier.addExam(Exam("Today exam", Date(2026, 9, 9)));
    school.addStudyClass(earlier);
    return school;
}

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
            CHECK(course.getCredits() == 3);
            CHECK(course.getStartDate() == Date(2026, 9, 7));
            CHECK(course.getEndDate() == Date(2026, 9, 16));
            CHECK(course.getBooks().empty());
            CHECK(course.getAssignments().empty());
            CHECK(course.getExams().empty());
            CHECK(!course.getGradeSummary());
            Book book("Textbook", "books/textbook.pdf");
            book.setBookMark(125);
            course.addBook(book);
            course.addBook(Book("Reference", "books/reference.pdf"));
            course.addAssignment(Assignment("Homework", Date(2026, 9, 7), Date(2026, 9, 10)));
            course.addAssignment(Assignment("Project", Date(2026, 9, 8), Date(2026, 9, 16)));
            course.addExam(Exam("Quiz", Date(2026, 9, 11)));
            course.addExam(Exam("Final", Date(2026, 9, 16)));
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
            CHECK(!course.getGradeSummary());
        }},
        {"School constructors and name", [] {
            School empty;
            CHECK(empty.getSchoolName().empty());
            CHECK(empty.getStudyClasses().empty());
            School school("College");
            CHECK(school.getSchoolName() == "College");
            school.setSchoolName("University");
            CHECK(school.getSchoolName() == "University");
        }},
        {"School adds and retrieves classes by value", [] {
            School school("College");
            auto course = smallCourse();
            school.addStudyClass(course);
            school.addStudyClass(StudyClass("Math", 4, Date(2026, 9, 1), Date(2026, 12, 20)));
            course.setClassName("Changed outside school");
            CHECK(school.getStudyClasses().size() == 2);
            CHECK(school.getStudyClasses()[0].getClassName() == "Data Structures");
            CHECK(school.getStudyClasses()[0].getClassMeetings().size() == 2);
            CHECK(school.getStudyClasses()[1].getClassName() == "Math");
            auto copy = school.getStudyClasses();
            copy.clear();
            CHECK(school.getStudyClasses().size() == 2);
        }},
        {"School safely removes classes by index", [] {
            School school("College");
            CHECK(!school.removeStudyClass(0));
            auto first = smallCourse();
            auto second = smallCourse();
            second.setClassName("Math");
            school.addStudyClass(first);
            school.addStudyClass(second);
            school.addStudyClass(first);
            CHECK(!school.removeStudyClass(3));
            CHECK(!school.removeStudyClass(static_cast<size_t>(-1)));
            CHECK(school.getStudyClasses().size() == 3);
            CHECK(school.removeStudyClass(1));
            CHECK(school.getStudyClasses().size() == 2);
            CHECK(school.getStudyClasses()[0].getClassName() == "Data Structures");
            CHECK(school.getStudyClasses()[1].getClassName() == "Data Structures");
            CHECK(school.removeStudyClass(1));
            CHECK(school.removeStudyClass(0));
            CHECK(school.getStudyClasses().empty());
            CHECK(!school.removeStudyClass(0));
        }},
        {"Student data and school collection", [] {
            Student student("David", 3.5);
            CHECK(student.getStudentName() == "David");
            CHECK(student.getGPA() == 3.5);
            CHECK(student.getSchools().empty());
            student.setStudentName("David Kharazi");
            student.setGPA(3.75);
            CHECK(student.getStudentName() == "David Kharazi");
            CHECK(student.getGPA() == 3.75);
            School school("College");
            school.addStudyClass(smallCourse());
            student.addSchool(school);
            student.addSchool(School("University"));
            CHECK(student.getSchools().size() == 2);
            CHECK(student.getSchools()[0].getSchoolName() == "College");
            CHECK(student.getSchools()[0].getStudyClasses().size() == 1);
            CHECK(student.getSchools()[0].getStudyClasses()[0].getClassName() == "Data Structures");
            CHECK(student.getSchools()[1].getSchoolName() == "University");
            CHECK(student.getSchools()[1].getStudyClasses().empty());
        }},
        {"Student owns independent school values", [] {
            School school("College");
            school.addStudyClass(smallCourse());
            Student first("First", 0);
            Student second("Second", 0);
            first.addSchool(school);
            second.addSchool(school);
            school.removeStudyClass(0);
            school.setSchoolName("Changed");
            auto schools = first.getSchools();
            schools[0].removeStudyClass(0);
            schools[0].setSchoolName("Changed copy");
            CHECK(first.getSchools()[0].getSchoolName() == "College");
            CHECK(first.getSchools()[0].getStudyClasses().size() == 1);
            CHECK(second.getSchools()[0].getSchoolName() == "College");
            CHECK(second.getSchools()[0].getStudyClasses().size() == 1);
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

        {"Assignment starts ungraded", [] {
            Assignment empty;
            Assignment named("Homework", Date(2026, 9, 1), Date(2026, 9, 10));
            CHECK(!empty.getGrade());
            CHECK(!named.getGrade());
        }},
        {"Assignment stores points and calculates percentage and letter", [] {
            Assignment work;
            work.setGrade(18, 20);
            CHECK(work.getGrade().has_value());
            CHECK(work.getGrade()->getEarnedPoints() == 18);
            CHECK(work.getGrade()->getMaxPoints() == 20);
            CHECK(near(work.getGrade()->getPercentage(), 90));
            CHECK(work.getGrade()->getLetter() == GradeLetter::A);
            work.setGrade(1, 3);
            CHECK(near(work.getGrade()->getPercentage(), 100.0 / 3));
        }},
        {"Assignment rejects invalid maximum and preserves prior grade", [] {
            Assignment work;
            expectThrows<std::invalid_argument>([&] { work.setGrade(5, 0); });
            CHECK(!work.getGrade());
            work.setGrade(18, 20);
            expectThrows<std::invalid_argument>([&] { work.setGrade(5, -10); });
            CHECK(work.getGrade()->getEarnedPoints() == 18);
            CHECK(work.getGrade()->getMaxPoints() == 20);
        }},
        {"Assignment rejects negative earned points", [] {
            Assignment work;
            expectThrows<std::invalid_argument>([&] { work.setGrade(-1, 20); });
            CHECK(!work.getGrade());
        }},
        {"Assignment allows extra credit and zero earned points", [] {
            Assignment work;
            work.setGrade(22, 20);
            CHECK(near(work.getGrade()->getPercentage(), 110));
            CHECK(work.getGrade()->getLetter() == GradeLetter::A);
            work.setGrade(0, 20);
            CHECK(work.getGrade().has_value());
            CHECK(work.getGrade()->getPercentage() == 0);
            CHECK(work.getGrade()->getLetter() == GradeLetter::F);
        }},
        {"Assignment replaces and clears a grade", [] {
            Assignment work;
            work.setGrade(18, 20);
            work.setGrade(83, 100);
            CHECK(work.getGrade()->getEarnedPoints() == 83);
            CHECK(work.getGrade()->getMaxPoints() == 100);
            CHECK(near(work.getGrade()->getPercentage(), 83));
            CHECK(work.getGrade()->getLetter() == GradeLetter::B);
            work.clearGrade();
            CHECK(!work.getGrade());
            work.clearGrade();
            CHECK(!work.getGrade());
        }},
        {"Assignment rejects non-finite grades and percentage overflow", [] {
            Assignment work;
            const double nan = std::numeric_limits<double>::quiet_NaN();
            const double infinity = std::numeric_limits<double>::infinity();
            for (double invalid : {nan, infinity, -infinity}) {
                expectThrows<std::invalid_argument>([&] { work.setGrade(invalid, 20); });
                expectThrows<std::invalid_argument>([&] { work.setGrade(10, invalid); });
            }
            expectThrows<std::invalid_argument>([&] {
                work.setGrade(std::numeric_limits<double>::max(), 0.5);
            });
            CHECK(!work.getGrade());
        }},

        {"Exam starts ungraded", [] {
            Exam empty;
            Exam named("Quiz", Date(2026, 9, 10));
            CHECK(!empty.getGrade());
            CHECK(!named.getGrade());
        }},
        {"Exam stores points and calculates percentage and letter", [] {
            Exam work;
            work.setGrade(18, 20);
            CHECK(work.getGrade().has_value());
            CHECK(work.getGrade()->getEarnedPoints() == 18);
            CHECK(work.getGrade()->getMaxPoints() == 20);
            CHECK(near(work.getGrade()->getPercentage(), 90));
            CHECK(work.getGrade()->getLetter() == GradeLetter::A);
            work.setGrade(1, 3);
            CHECK(near(work.getGrade()->getPercentage(), 100.0 / 3));
        }},
        {"Exam rejects invalid maximum and preserves prior grade", [] {
            Exam work;
            expectThrows<std::invalid_argument>([&] { work.setGrade(5, 0); });
            CHECK(!work.getGrade());
            work.setGrade(18, 20);
            expectThrows<std::invalid_argument>([&] { work.setGrade(5, -10); });
            CHECK(work.getGrade()->getEarnedPoints() == 18);
            CHECK(work.getGrade()->getMaxPoints() == 20);
        }},
        {"Exam rejects negative earned points", [] {
            Exam work;
            expectThrows<std::invalid_argument>([&] { work.setGrade(-1, 20); });
            CHECK(!work.getGrade());
        }},
        {"Exam allows extra credit and zero earned points", [] {
            Exam work;
            work.setGrade(22, 20);
            CHECK(near(work.getGrade()->getPercentage(), 110));
            CHECK(work.getGrade()->getLetter() == GradeLetter::A);
            work.setGrade(0, 20);
            CHECK(work.getGrade().has_value());
            CHECK(work.getGrade()->getPercentage() == 0);
            CHECK(work.getGrade()->getLetter() == GradeLetter::F);
        }},
        {"Exam replaces and clears a grade", [] {
            Exam work;
            work.setGrade(18, 20);
            work.setGrade(83, 100);
            CHECK(work.getGrade()->getEarnedPoints() == 83);
            CHECK(work.getGrade()->getMaxPoints() == 100);
            CHECK(near(work.getGrade()->getPercentage(), 83));
            CHECK(work.getGrade()->getLetter() == GradeLetter::B);
            work.clearGrade();
            CHECK(!work.getGrade());
            work.clearGrade();
            CHECK(!work.getGrade());
        }},
        {"Exam rejects non-finite grades and percentage overflow", [] {
            Exam work;
            const double nan = std::numeric_limits<double>::quiet_NaN();
            const double infinity = std::numeric_limits<double>::infinity();
            for (double invalid : {nan, infinity, -infinity}) {
                expectThrows<std::invalid_argument>([&] { work.setGrade(invalid, 20); });
                expectThrows<std::invalid_argument>([&] { work.setGrade(10, invalid); });
            }
            expectThrows<std::invalid_argument>([&] {
                work.setGrade(std::numeric_limits<double>::max(), 0.5);
            });
            CHECK(!work.getGrade());
        }},

        {"Course without graded work has no summary", [] {
            auto course = smallCourse();
            CHECK(!course.getGradeSummary());
            course.addAssignment(Assignment());
            course.addExam(Exam());
            CHECK(!course.getGradeSummary());
        }},
        {"Course with one graded assignment", [] {
            auto course = smallCourse();
            Assignment assignment;
            assignment.setGrade(18, 20);
            course.addAssignment(assignment);
            auto summary = course.getGradeSummary();
            CHECK(summary.has_value());
            CHECK(summary->getEarnedPoints() == 18);
            CHECK(summary->getMaxPoints() == 20);
            CHECK(near(summary->getPercentage(), 90));
            CHECK(summary->getLetter() == GradeLetter::A);
        }},
        {"Course totals points instead of averaging percentages", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.addAssignment(Assignment());
            course.setAssignmentGrade(0, 10, 10);
            course.setAssignmentGrade(1, 0, 90);
            auto summary = course.getGradeSummary();
            CHECK(summary.has_value());
            CHECK(summary->getEarnedPoints() == 10);
            CHECK(summary->getMaxPoints() == 100);
            CHECK(near(summary->getPercentage(), 10));
            CHECK(summary->getLetter() == GradeLetter::F);
        }},
        {"Course combines assignment and exam and excludes ungraded work", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.addAssignment(Assignment());
            course.addExam(Exam());
            course.addExam(Exam());
            course.setAssignmentGrade(0, 18, 20);
            course.setExamGrade(0, 90, 100);
            auto summary = course.getGradeSummary();
            CHECK(summary.has_value());
            CHECK(summary->getEarnedPoints() == 108);
            CHECK(summary->getMaxPoints() == 120);
            CHECK(near(summary->getPercentage(), 90));
            CHECK(summary->getLetter() == GradeLetter::A);
            CHECK(!course.getAssignments()[1].getGrade());
            CHECK(!course.getExams()[1].getGrade());
        }},
        {"Course with only a graded exam", [] {
            auto course = smallCourse();
            course.addExam(Exam());
            course.setExamGrade(0, 83, 100);
            auto summary = course.getGradeSummary();
            CHECK(summary.has_value());
            CHECK(summary->getEarnedPoints() == 83);
            CHECK(summary->getMaxPoints() == 100);
            CHECK(near(summary->getPercentage(), 83));
            CHECK(summary->getLetter() == GradeLetter::B);
        }},
        {"Course zero points is F and extra credit is A", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.setAssignmentGrade(0, 0, 20);
            CHECK(course.getGradeSummary().has_value());
            CHECK(course.getGradeSummary()->getLetter() == GradeLetter::F);
            course.setAssignmentGrade(0, 22, 20);
            CHECK(near(course.getGradeSummary()->getPercentage(), 110));
            CHECK(course.getGradeSummary()->getLetter() == GradeLetter::A);
        }},
        {"Course recalculates from owned work after grading and clearing", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.addExam(Exam());
            course.setAssignmentGrade(0, 18, 20);
            course.setExamGrade(0, 90, 100);
            CHECK(course.getAssignments()[0].getGrade()->getEarnedPoints() == 18);
            CHECK(course.getExams()[0].getGrade()->getEarnedPoints() == 90);
            course.setAssignmentGrade(0, 10, 20);
            CHECK(course.getGradeSummary()->getEarnedPoints() == 100);
            CHECK(near(course.getGradeSummary()->getPercentage(), 10000.0 / 120));
            expectThrows<std::invalid_argument>([&] { course.setAssignmentGrade(0, -1, 20); });
            expectThrows<std::invalid_argument>([&] { course.setExamGrade(0, 10, 0); });
            CHECK(course.getGradeSummary()->getEarnedPoints() == 100);
            course.clearAssignmentGrade(0);
            CHECK(course.getGradeSummary()->getMaxPoints() == 100);
            CHECK(near(course.getGradeSummary()->getPercentage(), 90));
            course.clearExamGrade(0);
            CHECK(!course.getGradeSummary());
        }},
        {"Course grading rejects invalid indices", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.addExam(Exam());
            expectThrows<std::out_of_range>([&] { course.setAssignmentGrade(1, 18, 20); });
            expectThrows<std::out_of_range>([&] { course.setExamGrade(1, 18, 20); });
            expectThrows<std::out_of_range>([&] { course.clearAssignmentGrade(1); });
            expectThrows<std::out_of_range>([&] { course.clearExamGrade(1); });
            CHECK(!course.getGradeSummary());
        }},
        {"Course reports numeric overflow in totals", [] {
            auto course = smallCourse();
            course.addAssignment(Assignment());
            course.addExam(Exam());
            const double largest = std::numeric_limits<double>::max();
            course.setAssignmentGrade(0, largest, largest);
            course.setExamGrade(0, largest, largest);
            expectThrows<std::overflow_error>([&] { course.getGradeSummary(); });
        }},
        {"Letter grade exact boundaries without rounding", [] {
            struct Boundary { double percentage; GradeLetter letter; };
            const Boundary boundaries[] = {
                {100, GradeLetter::A}, {90, GradeLetter::A},
                {89.999, GradeLetter::B}, {80, GradeLetter::B},
                {79.999, GradeLetter::C}, {70, GradeLetter::C},
                {69.999, GradeLetter::D}, {60, GradeLetter::D},
                {59.999, GradeLetter::F}
            };
            for (const auto& boundary : boundaries) {
                Grade grade(boundary.percentage, 100);
                CHECK(near(grade.getPercentage(), boundary.percentage));
                CHECK(grade.getLetter() == boundary.letter);
            }
        }},
        {"Assignment grading and completion remain independent", [] {
            Assignment assignment("Homework", Date(2026, 9, 1), Date(2026, 9, 10));
            assignment.setGrade(18, 20);
            CHECK(!assignment.isFinished());
            CHECK(assignment.getStatus(Date(2026, 9, 11)) == AssignmentStatus::Overdue);
            assignment.setFinished(true);
            CHECK(assignment.getGrade()->getEarnedPoints() == 18);
            assignment.clearGrade();
            CHECK(assignment.isFinished());
        }},
        {"Week containing Monday", [] {
            CHECK(Date(2026, 9, 7).getWeekStartDate() == Date(2026, 9, 7));
            CHECK(Date(2026, 9, 7).getWeekEndDate() == Date(2026, 9, 13));
        }},
        {"Week containing Wednesday", [] {
            CHECK(Date(2026, 9, 9).getWeekStartDate() == Date(2026, 9, 7));
            CHECK(Date(2026, 9, 9).getWeekEndDate() == Date(2026, 9, 13));
        }},
        {"Week containing Sunday", [] {
            CHECK(Date(2026, 9, 13).getWeekStartDate() == Date(2026, 9, 7));
            CHECK(Date(2026, 9, 13).getWeekEndDate() == Date(2026, 9, 13));
        }},
        {"Week crossing month boundary", [] {
            CHECK(Date(2026, 10, 1).getWeekStartDate() == Date(2026, 9, 28));
            CHECK(Date(2026, 10, 1).getWeekEndDate() == Date(2026, 10, 4));
        }},
        {"Week crossing year boundary", [] {
            CHECK(Date(2027, 1, 1).getWeekStartDate() == Date(2026, 12, 28));
            CHECK(Date(2027, 1, 1).getWeekEndDate() == Date(2027, 1, 3));
        }},
        {"Previous day handles month year and leap boundaries", [] {
            CHECK(Date(2026, 9, 10).getPreviousDay() == Date(2026, 9, 9));
            CHECK(Date(2027, 1, 1).getPreviousDay() == Date(2026, 12, 31));
            CHECK(Date(2026, 5, 1).getPreviousDay() == Date(2026, 4, 30));
            CHECK(Date(2024, 3, 1).getPreviousDay() == Date(2024, 2, 29));
            CHECK(Date(1900, 3, 1).getPreviousDay() == Date(1900, 2, 28));
            CHECK(Date(2000, 3, 1).getPreviousDay() == Date(2000, 2, 29));
        }},
        {"Previous and next week navigation", [] {
            Date reference(2027, 1, 1);
            auto previous = reference.getWeekStartDate().getPreviousDay();
            auto next = reference.getWeekEndDate().getNextDay();
            CHECK(previous.getWeekStartDate() == Date(2026, 12, 21));
            CHECK(next.getWeekStartDate() == Date(2027, 1, 4));
        }},
        {"Date validity and invalid arithmetic", [] {
            CHECK(Date(2024, 2, 29).isValid());
            CHECK(Date(2000, 2, 29).isValid());
            const Date invalid[] = {Date(), Date(0, 1, 1), Date(-1, 1, 1),
                Date(2026, 13, 1), Date(2026, 0, 1), Date(2026, 4, 31),
                Date(2026, 2, 29), Date(1900, 2, 29), Date(2026, 1, 0)};
            for (const Date& date : invalid) {
                CHECK(!date.isValid());
                expectThrows<std::invalid_argument>([&] { date.getWeekDay(); });
                expectThrows<std::invalid_argument>([&] { date.getNextDay(); });
                expectThrows<std::invalid_argument>([&] { date.getPreviousDay(); });
                expectThrows<std::invalid_argument>([&] { date.getWeekStartDate(); });
                expectThrows<std::invalid_argument>([&] { date.getWeekEndDate(); });
            }
        }},
        {"Date arithmetic rejects year overflow and underflow", [] {
            CHECK(Date(1, 1, 1).getWeekDay() == WeekDay::Monday);
            expectThrows<std::out_of_range>([] { Date(1, 1, 1).getPreviousDay(); });
            Date last(std::numeric_limits<int>::max(), 12, 31);
            CHECK(last.isValid());
            expectThrows<std::out_of_range>([&] { last.getNextDay(); });
        }},
        {"School upcoming events include today across courses", [] {
            auto events = Calendar().getUpcomingEvents(querySchool(), Date(2026, 9, 9));
            const std::vector<std::string> titles = {"Today assignment", "Today exam",
                "Future assignment", "Future exam", "Sunday exam", "Next week exam"};
            CHECK(events.size() == titles.size());
            for (size_t i = 0; i < events.size(); ++i) {
                CHECK(events[i].getTitle() == titles[i]);
                CHECK(!(events[i].getDate() < Date(2026, 9, 9)));
                CHECK(events[i].getType() != EventType::ClassMeeting);
                CHECK(!events[i].hasTime());
            }
            CHECK(events[0].getDate() == Date(2026, 9, 9));
            CHECK(events[0].getType() == EventType::Assignment);
            CHECK(events[1].getType() == EventType::Exam);
            checkSorted(events);
        }},
        {"Weekly classes combine courses within inclusive boundaries", [] {
            auto events = Calendar().getWeeklySchedule(querySchool(), Date(2026, 9, 9), ScheduleMode::ClassesOnly);
            CHECK(events.size() == 6);
            CHECK(events.front().getDate() == Date(2026, 9, 7));
            CHECK(events.back().getDate() == Date(2026, 9, 13));
            int laterCount = 0;
            for (const auto& event : events) {
                CHECK(event.getType() == EventType::ClassMeeting);
                CHECK(event.hasTime());
                CHECK(!(event.getDate() < Date(2026, 9, 7)));
                CHECK(!(event.getDate() > Date(2026, 9, 13)));
                auto day = event.getDate().getWeekDay();
                CHECK(day == WeekDay::Monday || day == WeekDay::Wednesday || day == WeekDay::Sunday);
                if (event.getTitle() == "Later class") {
                    ++laterCount;
                    CHECK(event.getDate() == Date(2026, 9, 9));
                }
            }
            CHECK(laterCount == 2);
            checkSorted(events);
        }},
        {"Weekly events exclude meetings and dates outside the week", [] {
            auto events = Calendar().getWeeklySchedule(querySchool(), Date(2026, 9, 9), ScheduleMode::EventsOnly);
            const std::vector<std::string> titles = {"Monday assignment", "Past assignment",
                "Today assignment", "Today exam", "Future assignment", "Future exam", "Sunday exam"};
            CHECK(events.size() == titles.size());
            for (size_t i = 0; i < events.size(); ++i) {
                CHECK(events[i].getTitle() == titles[i]);
                CHECK(events[i].getType() != EventType::ClassMeeting);
                CHECK(!events[i].hasTime());
            }
            CHECK(events.front().getDate() == Date(2026, 9, 7));
            CHECK(events.back().getDate() == Date(2026, 9, 13));
            checkSorted(events);
        }},
        {"Weekly combined schedule sorts same-day times then untimed events", [] {
            auto events = Calendar().getWeeklySchedule(querySchool(), Date(2026, 9, 9));
            CHECK(events.size() == 13);
            checkSorted(events);
            std::vector<CalendarEvent> wednesday;
            for (const auto& event : events)
                if (event.getDate() == Date(2026, 9, 9)) wednesday.push_back(event);
            CHECK(wednesday.size() == 6);
            CHECK(wednesday[0].getTitle() == "Earlier class");
            CHECK(sameTime(wednesday[0].getStartTime().value(), 9, 0));
            CHECK(wednesday[1].getTitle() == "Later class");
            CHECK(sameTime(wednesday[1].getStartTime().value(), 9, 30));
            CHECK(wednesday[2].getTitle() == "Later class");
            CHECK(wednesday[3].getTitle() == "Earlier class");
            CHECK(sameTime(wednesday[2].getStartTime().value(), 10, 30));
            CHECK(sameTime(wednesday[3].getStartTime().value(), 10, 30));
            CHECK(wednesday[4].getTitle() == "Today assignment");
            CHECK(wednesday[5].getTitle() == "Today exam");
            CHECK(!wednesday[4].hasTime());
            CHECK(!wednesday[5].hasTime());
        }},
        {"All reference days in a week give the same schedule", [] {
            auto school = querySchool();
            auto monday = Calendar().getWeeklySchedule(school, Date(2026, 9, 7));
            for (int day = 8; day <= 13; ++day) {
                auto events = Calendar().getWeeklySchedule(school, Date(2026, 9, day));
                CHECK(events.size() == monday.size());
                for (size_t i = 0; i < events.size(); ++i) {
                    CHECK(events[i].getTitle() == monday[i].getTitle());
                    CHECK(events[i].getDate() == monday[i].getDate());
                }
            }
        }},
        {"Past and future semesters exclude meetings but retain actual deadlines", [] {
            School school("College");
            StudyClass past("Past", 3, Date(2026, 8, 1), Date(2026, 9, 6));
            StudyClass future("Future", 3, Date(2026, 9, 14), Date(2026, 10, 1));
            for (StudyClass* course : {&past, &future})
                course->addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(8, 0), Time(9, 0)));
            past.addAssignment(Assignment("Late deadline", Date(2026, 8, 1), Date(2026, 9, 9)));
            future.addExam(Exam("Early exam", Date(2026, 9, 10)));
            school.addStudyClass(past);
            school.addStudyClass(future);
            CHECK(Calendar().getWeeklySchedule(school, Date(2026, 9, 9), ScheduleMode::ClassesOnly).empty());
            CHECK(Calendar().getWeeklySchedule(school, Date(2026, 9, 9)).size() == 2);
            CHECK(Calendar().getUpcomingEvents(school, Date(2026, 9, 9)).size() == 2);
        }},
        {"Range generation reuses recurrence and clips a very long semester", [] {
            StudyClass course("Long course", 3, Date(1, 1, 1), Date(std::numeric_limits<int>::max(), 12, 31));
            course.addClassMeeting(ClassMeeting(WeekDay::Monday, Time(8, 0), Time(9, 0)));
            auto events = Calendar().generateClassEvents(course, Date(2026, 9, 7), Date(2026, 9, 13));
            CHECK(events.size() == 1);
            CHECK(events[0].getDate() == Date(2026, 9, 7));
            expectThrows<std::invalid_argument>([&] {
                Calendar().generateClassEvents(course, Date(2026, 9, 13), Date(2026, 9, 7));
            });
        }},
        {"Queries of empty schools return empty collections", [] {
            School school("Empty");
            CHECK(Calendar().getUpcomingEvents(school, Date(2026, 9, 9)).empty());
            for (auto mode : {ScheduleMode::ClassesOnly, ScheduleMode::EventsOnly, ScheduleMode::Combined})
                CHECK(Calendar().getWeeklySchedule(school, Date(2026, 9, 9), mode).empty());
        }},
        {"Queries reject invalid reference dates and invalid mode", [] {
            School school("Empty");
            expectThrows<std::invalid_argument>([&] { Calendar().getUpcomingEvents(school, Date()); });
            expectThrows<std::invalid_argument>([&] { Calendar().getWeeklySchedule(school, Date()); });
            expectThrows<std::invalid_argument>([&] {
                Calendar().getWeeklySchedule(school, Date(2026, 9, 9), static_cast<ScheduleMode>(99));
            });
        }},
        {"Calendar rejects invalid source dates", [] {
            StudyClass course;
            expectThrows<std::invalid_argument>([&] { Calendar().generateClassEvents(course); });
            course = smallCourse();
            course.addAssignment(Assignment());
            expectThrows<std::invalid_argument>([&] { Calendar().generateDeadlineEvents(course); });
            course = smallCourse();
            course.addExam(Exam());
            expectThrows<std::invalid_argument>([&] { Calendar().generateDeadlineEvents(course); });
        }},
        {"Weekly query crosses the year and preserves source objects", [] {
            School school("College");
            StudyClass course("Winter", 3, Date(2026, 12, 30), Date(2027, 1, 2));
            course.addClassMeeting(ClassMeeting(WeekDay::Wednesday, Time(8, 0), Time(9, 0)));
            course.addClassMeeting(ClassMeeting(WeekDay::Saturday, Time(8, 0), Time(9, 0)));
            course.addExam(Exam("New year exam", Date(2027, 1, 1)));
            school.addStudyClass(course);
            auto events = Calendar().getWeeklySchedule(school, Date(2027, 1, 1));
            CHECK(events.size() == 3);
            CHECK(events[0].getDate() == Date(2026, 12, 30));
            CHECK(events[1].getDate() == Date(2027, 1, 1));
            CHECK(events[2].getDate() == Date(2027, 1, 2));
            events[0].setTitle("Changed view");
            CHECK(Calendar().getWeeklySchedule(school, Date(2027, 1, 1))[0].getTitle() == "Winter");
            CHECK(school.getStudyClasses()[0].getClassMeetings().size() == 2);
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
