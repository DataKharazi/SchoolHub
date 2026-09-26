/*
	Name: Student.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Student class.
*/

#include "Student.h"

Student::Student()
{
	studentName = "";
	gpa = 0;
}

Student::Student(string studentName, double gpa)
{
	this->studentName = studentName;
	this->gpa = gpa;
}

void Student::setStudentName(string studentName)
{
	this->studentName = studentName;
}

void Student::setGPA(double gpa)
{
	this->gpa = gpa;
}

string Student::getStudentName() const
{
	return studentName;
}

double Student::getGPA() const
{
	return gpa;
}

void Student::addStudyClass(StudyClass studyClass)
{
	studyClasses.push_back(studyClass);
}

vector<StudyClass> Student::getStudyClasses() const
{
	return studyClasses;
}