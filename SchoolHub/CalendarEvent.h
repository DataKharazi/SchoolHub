/*
	Name: CalendarEvent.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the CalendarEvent class.
*/

#pragma once

#include <string>
#include <optional>

#include "Date.h"
#include "Time.h"

using namespace std;

enum class EventType
{
	ClassMeeting,
	Assignment,
	Exam
};

class CalendarEvent
{
private:
	string title;
	Date date;
	EventType type;

	optional<Time> startTime;
	optional<Time> endTime;

public:
	CalendarEvent();

	CalendarEvent(
		string title,
		Date date,
		EventType type
	);

	CalendarEvent(
		string title,
		Date date,
		EventType type,
		Time startTime,
		Time endTime
	);

	void setTitle(string title);
	void setDate(Date date);
	void setType(EventType type);

	string getTitle() const;
	Date getDate() const;
	EventType getType() const;

	bool hasTime() const;

	optional<Time> getStartTime() const;
	optional<Time> getEndTime() const;
};