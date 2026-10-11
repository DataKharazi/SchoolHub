/*
	Name: Database.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Database class.
*/

#pragma once

#include <string>
#include <optional>
#include <vector>
#include "Student.h"

using namespace std;

struct sqlite3;

class Database
{
private:
	sqlite3* database;
	string lastError;
	Student loadStudentHierarchy(std::int64_t id);

public:
	Database();
	~Database();
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	bool open(const string& filePath);
	void close();

	bool isOpen() const;

	bool execute(const string& sql);
	bool createTables();

	// Saves the complete snapshot. IDs are published to student only after commit.
	bool saveStudent(Student& student);
	std::optional<Student> loadStudent(std::int64_t id);
	std::vector<Student> loadStudents();
	bool deleteStudent(std::int64_t id);
	bool deleteSchool(std::int64_t studentId, std::int64_t schoolId);

	string getLastError() const;
};
