/*
	Name: StudyClass.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the StudyClass class.
*/

#include "StudyClass.h"
#include <cmath>
#include <stdexcept>

StudyClass::StudyClass()
{
	className = "";
	credits = 0;
}

StudyClass::StudyClass(string className,
	double credits, Date startDate, Date endDate)
{
	this->className = className;
	this->credits = credits;
	this->startDate = startDate;
	this->endDate = endDate;
}

void StudyClass::setClassName(string className)
{
	this->className = className;
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

optional<Grade> StudyClass::getGradeSummary() const
{
	double earned = 0;
	double possible = 0;
	for (const Assignment& assignment : assignments)
	{
		auto grade = assignment.getGrade();
		if (grade)
		{
			earned += grade->getEarnedPoints();
			possible += grade->getMaxPoints();
		}
	}
	for (const Exam& exam : exams)
	{
		auto grade = exam.getGrade();
		if (grade)
		{
			earned += grade->getEarnedPoints();
			possible += grade->getMaxPoints();
		}
	}
	if (!std::isfinite(earned) || !std::isfinite(possible))
		throw std::overflow_error("Course grade totals exceed the supported numeric range.");
	if (possible == 0)
		return nullopt;
	return Grade(earned, possible);
}

void StudyClass::setAssignmentGrade(size_t index, double earnedPoints, double maxPoints)
{
	assignments.at(index).setGrade(earnedPoints, maxPoints);
}

void StudyClass::clearAssignmentGrade(size_t index)
{
	assignments.at(index).clearGrade();
}

void StudyClass::setExamGrade(size_t index, double earnedPoints, double maxPoints)
{
	exams.at(index).setGrade(earnedPoints, maxPoints);
}

void StudyClass::clearExamGrade(size_t index)
{
	exams.at(index).clearGrade();
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
