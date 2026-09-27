/*
	Name: CalendarEvent.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the CalendarEvent class.
*/

#include "CalendarEvent.h"

CalendarEvent::CalendarEvent()
{
	title = "";
	type = EventType::Assignment;
}

CalendarEvent::CalendarEvent(
	string title,
	Date date,
	EventType type)
{
	this->title = title;
	this->date = date;
	this->type = type;
}

CalendarEvent::CalendarEvent(
	string title,
	Date date,
	EventType type,
	Time startTime,
	Time endTime)
{
	this->title = title;
	this->date = date;
	this->type = type;

	this->startTime = startTime;
	this->endTime = endTime;
}

void CalendarEvent::setTitle(string title)
{
	this->title = title;
}

void CalendarEvent::setDate(Date date)
{
	this->date = date;
}

void CalendarEvent::setType(EventType type)
{
	this->type = type;
}

string CalendarEvent::getTitle() const
{
	return title;
}

Date CalendarEvent::getDate() const
{
	return date;
}

EventType CalendarEvent::getType() const
{
	return type;
}

bool CalendarEvent::hasTime() const
{
	return startTime.has_value() &&
		endTime.has_value();
}

optional<Time> CalendarEvent::getStartTime() const
{
	return startTime;
}

optional<Time> CalendarEvent::getEndTime() const
{
	return endTime;
}