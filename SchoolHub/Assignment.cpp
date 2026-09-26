/*
	Name: Assignment.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Assignment class.
*/

#include "Assignment.h"

Assignment::Assignment()
{
	description = "";
	finished = false;
}

Assignment::Assignment(string description, Date startDate, Date dueDate)
{
	this->description = description;
	this->startDate = startDate;
	this->dueDate = dueDate;
	finished = false;
}

void Assignment::setDescription(string description)
{
	this->description = description;
}

void Assignment::setStartDate(Date startDate)
{
	this->startDate = startDate;
}

void Assignment::setDueDate(Date dueDate)
{
	this->dueDate = dueDate;
}

void Assignment::setFinished(bool finished)
{
	this->finished = finished;
}

string Assignment::getDescription() const
{
	return description;
}

Date Assignment::getStartDate() const
{
	return startDate;
}

Date Assignment::getDueDate() const
{
	return dueDate;
}

bool Assignment::isFinished() const
{
	return finished;
}

AssignmentStatus Assignment::getStatus(const Date& currentDate) const
{
	if (finished)
		return AssignmentStatus::Finished;

	if (currentDate > dueDate)
		return AssignmentStatus::Overdue;

	return AssignmentStatus::Active;
}