/*
	Name: Exam.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Exam class.
*/

#pragma once
#include <cstdint>

#include <string>
#include <optional>
#include "Grade.h"
#include "Date.h"

using namespace std;

class Exam
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	string examName;
	Date examDate;
	optional<Grade> grade;

public:
    std::int64_t getId() const { return id; }
	Exam();
	Exam(string examName, Date examDate);

	void setExamName(string examName);
	void setExamDate(Date examDate);

	string getExamName() const;
	Date getExamDate() const;

	void setGrade(double earnedPoints, double maxPoints);
	void clearGrade();
	optional<Grade> getGrade() const;
};
