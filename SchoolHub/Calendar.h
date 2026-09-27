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

using namespace std;

class Calendar
{
public:
	vector<CalendarEvent> generateClassEvents(
		const StudyClass& studyClass
	) const;

	vector<CalendarEvent> generateDeadlineEvents(
		const StudyClass& studyClass
	) const;

	vector<CalendarEvent> generateAllEvents(
		const StudyClass& studyClass
	) const;
};