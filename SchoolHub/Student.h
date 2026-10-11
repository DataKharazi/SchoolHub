/*
	Name: Student.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Student class.
*/

#pragma once
#include <cstdint>

#include <string>
#include <vector>
#include "School.h"

using namespace std;

class Student
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	string studentName;
	double gpa;
	vector<School> schools;

public:
    std::int64_t getId() const { return id; }
	Student();
	Student(string studentName, double gpa);

	void setStudentName(string studentName);
	void setGPA(double gpa);

	string getStudentName() const;
	double getGPA() const;

	void addSchool(School school);

	vector<School> getSchools() const;
	// Checked access to owned values for editing and then saving the hierarchy.
	School& getSchool(size_t index) { return schools.at(index); }
	const School& getSchool(size_t index) const { return schools.at(index); }
};
