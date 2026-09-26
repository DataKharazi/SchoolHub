/*
	Name: StudyClass.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the StudyClass class.
*/

#pragma once

#include <string>
#include <vector>

#include "Date.h"
#include "Book.h"
#include "Assignment.h"
#include "Exam.h"
#include "ClassMeeting.h"

using namespace std;

class StudyClass
{
private:
	string className;
	string organizationName;
	double credits;

	vector<double> grades;

	Date startDate;
	Date endDate;

	vector<Assignment> assignments;
	vector<Book> books;
	vector<Exam> exams;
	vector<ClassMeeting> classMeetings;

public:
	StudyClass();
	StudyClass(string className, string organizationName,
		double credits, Date startDate, Date endDate);

	void setClassName(string className);
	void setOrganizationName(string organizationName);
	void setCredits(double credits);
	void setStartDate(Date startDate);
	void setEndDate(Date endDate);

	string getClassName() const;
	string getOrganizationName() const;
	double getCredits() const;
	Date getStartDate() const;
	Date getEndDate() const;

	void addGrade(double grade);
	void addAssignment(Assignment assignment);
	void addBook(Book book);
	void addExam(Exam exam);
	void addClassMeeting(ClassMeeting classMeeting);

	vector<double> getGrades() const;
	vector<Assignment> getAssignments() const;
	vector<Book> getBooks() const;
	vector<Exam> getExams() const;
	vector<ClassMeeting> getClassMeetings() const;
};