/*
	Name: StudyClass.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the StudyClass class.
*/

#pragma once
#include <cstdint>

#include <string>
#include <vector>
#include <cstddef>
#include <optional>
#include "Grade.h"

#include "Date.h"
#include "Book.h"
#include "Assignment.h"
#include "Exam.h"
#include "ClassMeeting.h"

using namespace std;

class StudyClass
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	string className;
	double credits;

	Date startDate;
	Date endDate;

	vector<Assignment> assignments;
	vector<Book> books;
	vector<Exam> exams;
	vector<ClassMeeting> classMeetings;

public:
    std::int64_t getId() const { return id; }
	StudyClass();
	StudyClass(string className,
		double credits, Date startDate, Date endDate);

	void setClassName(string className);
	void setCredits(double credits);
	void setStartDate(Date startDate);
	void setEndDate(Date endDate);

	string getClassName() const;
	double getCredits() const;
	Date getStartDate() const;
	Date getEndDate() const;

	void addAssignment(Assignment assignment);
	void addBook(Book book);
	void addExam(Exam exam);
	void addClassMeeting(ClassMeeting classMeeting);

	// Derived from graded assignments and exams; nullopt means no graded work.
	optional<Grade> getGradeSummary() const;

	// Zero-based indices; invalid indices throw out_of_range.
	void setAssignmentGrade(size_t index, double earnedPoints, double maxPoints);
	void clearAssignmentGrade(size_t index);
	void setExamGrade(size_t index, double earnedPoints, double maxPoints);
	void clearExamGrade(size_t index);

	vector<Assignment> getAssignments() const;
	vector<Book> getBooks() const;
	vector<Exam> getExams() const;
	vector<ClassMeeting> getClassMeetings() const;
};
