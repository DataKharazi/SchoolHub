/*
	Name: StudyClass.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the StudyClass class.
*/

#include "StudyClass.h"

StudyClass::StudyClass()
{
	className = "";
	organizationName = "";
	credits = 0;
}

StudyClass::StudyClass(string className, string organizationName,
	double credits, Date startDate, Date endDate)
{
	this->className = className;
	this->organizationName = organizationName;
	this->credits = credits;
	this->startDate = startDate;
	this->endDate = endDate;
}

void StudyClass::setClassName(string className)
{
	this->className = className;
}

void StudyClass::setOrganizationName(string organizationName)
{
	this->organizationName = organizationName;
}

void StudyClass::setCredits(double credits)
{
	this->credits = credits;
}

void StudyClass::setStartDate(Date startDate)
{
	this->startDate = startDate;
}

void StudyClass::setEndDate(Date endDate)
{
	this->endDate = endDate;
}

string StudyClass::getClassName() const
{
	return className;
}

string StudyClass::getOrganizationName() const
{
	return organizationName;
}

double StudyClass::getCredits() const
{
	return credits;
}

Date StudyClass::getStartDate() const
{
	return startDate;
}

Date StudyClass::getEndDate() const
{
	return endDate;
}

void StudyClass::addGrade(double grade)
{
	grades.push_back(grade);
}

void StudyClass::addAssignment(Assignment assignment)
{
	assignments.push_back(assignment);
}

void StudyClass::addBook(Book book)
{
	books.push_back(book);
}

void StudyClass::addExam(Exam exam)
{
	exams.push_back(exam);
}

void StudyClass::addClassMeeting(ClassMeeting classMeeting)
{
	classMeetings.push_back(classMeeting);
}

vector<double> StudyClass::getGrades() const
{
	return grades;
}

vector<Assignment> StudyClass::getAssignments() const
{
	return assignments;
}

vector<Book> StudyClass::getBooks() const
{
	return books;
}

vector<Exam> StudyClass::getExams() const
{
	return exams;
}

vector<ClassMeeting> StudyClass::getClassMeetings() const
{
	return classMeetings;
}