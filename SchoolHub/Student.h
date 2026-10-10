/*
	Name: Student.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Student class.
*/

#pragma once

#include <string>
#include <vector>
#include "School.h"

using namespace std;

class Student
{
private:
	string studentName;
	double gpa;
	vector<School> schools;

public:
	Student();
	Student(string studentName, double gpa);

	void setStudentName(string studentName);
	void setGPA(double gpa);

	string getStudentName() const;
	double getGPA() const;

	void addSchool(School school);

	vector<School> getSchools() const;
};
