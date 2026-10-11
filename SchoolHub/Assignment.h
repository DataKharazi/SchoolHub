/*
	Name: Assignment.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Assignment class.
*/

#pragma once
#include <cstdint>

#include <string>
#include <optional>
#include "Grade.h"
#include "Date.h"

using namespace std;

enum class AssignmentStatus
{
	Active,
	Overdue,
	Finished
};

class Assignment
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	string description;
	Date startDate;
	Date dueDate;
	bool finished;
	optional<Grade> grade;

public:
    std::int64_t getId() const { return id; }
	Assignment();
	Assignment(string description, Date startDate, Date dueDate);

	void setDescription(string description);
	void setStartDate(Date startDate);
	void setDueDate(Date dueDate);
	void setFinished(bool finished);

	string getDescription() const;
	Date getStartDate() const;
	Date getDueDate() const;
	bool isFinished() const;

	AssignmentStatus getStatus(const Date& currentDate) const;

	void setGrade(double earnedPoints, double maxPoints);
	void clearGrade();
	optional<Grade> getGrade() const;
};
