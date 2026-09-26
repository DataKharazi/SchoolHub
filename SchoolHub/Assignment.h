/*
	Name: Assignment.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Assignment class.
*/

#pragma once

#include <string>
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
private:
	string description;
	Date startDate;
	Date dueDate;
	bool finished;

public:
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
};