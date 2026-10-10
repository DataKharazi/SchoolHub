/*
	Name: Calendar.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Calendar class.
*/

#pragma once

#include <vector>

#include "StudyClass.h"
#include "CalendarEvent.h"
#include "School.h"

using namespace std;

enum class ScheduleMode
{
	ClassesOnly,
	EventsOnly,
	Combined
};

class Calendar
{
public:
	vector<CalendarEvent> generateClassEvents(
		const StudyClass& studyClass
	) const;

	// Inclusive range, clipped to the course semester.
	vector<CalendarEvent> generateClassEvents(const StudyClass& studyClass,
		const Date& rangeStart, const Date& rangeEnd) const;

	// Includes assignments/exams on the reference date; never includes meetings.
	vector<CalendarEvent> getUpcomingEvents(const School& school,
		const Date& referenceDate) const;

	vector<CalendarEvent> getWeeklySchedule(const School& school,
		const Date& referenceDate, ScheduleMode mode = ScheduleMode::Combined) const;

	vector<CalendarEvent> generateDeadlineEvents(
		const StudyClass& studyClass
	) const;

	vector<CalendarEvent> generateAllEvents(
		const StudyClass& studyClass
	) const;
};
