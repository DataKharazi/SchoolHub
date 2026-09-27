/*
	Name: Calendar.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Calendar class.
*/

#include "Calendar.h"

#include <algorithm>

vector<CalendarEvent> Calendar::generateClassEvents(
	const StudyClass& studyClass) const
{
	vector<CalendarEvent> events;

	Date currentDate = studyClass.getStartDate();
	Date endDate = studyClass.getEndDate();

	vector<ClassMeeting> meetings =
		studyClass.getClassMeetings();

	while (!(currentDate > endDate))
	{
		for (ClassMeeting meeting : meetings)
		{
			if (currentDate.getWeekDay() ==
				meeting.getWeekDay())
			{
				CalendarEvent event(
					studyClass.getClassName(),
					currentDate,
					EventType::ClassMeeting,
					meeting.getStartTime(),
					meeting.getEndTime()
				);

				events.push_back(event);
			}
		}

		currentDate = currentDate.getNextDay();
	}

	return events;
}

vector<CalendarEvent> Calendar::generateDeadlineEvents(
	const StudyClass& studyClass) const
{
	vector<CalendarEvent> events;

	for (Assignment assignment :
	studyClass.getAssignments())
	{
		CalendarEvent event(
			assignment.getDescription(),
			assignment.getDueDate(),
			EventType::Assignment
		);

		events.push_back(event);
	}

	for (Exam exam : studyClass.getExams())
	{
		CalendarEvent event(
			exam.getExamName(),
			exam.getExamDate(),
			EventType::Exam
		);

		events.push_back(event);
	}

	stable_sort(
		events.begin(),
		events.end(),
		[](const CalendarEvent& first,
			const CalendarEvent& second)
		{
			return first.getDate() < second.getDate();
		}
	);

	return events;
}

vector<CalendarEvent> Calendar::generateAllEvents(
	const StudyClass& studyClass) const
{
	vector<CalendarEvent> events =
		generateClassEvents(studyClass);

	vector<CalendarEvent> deadlineEvents =
		generateDeadlineEvents(studyClass);

	for (CalendarEvent event : deadlineEvents)
	{
		events.push_back(event);
	}

	stable_sort(
		events.begin(),
		events.end(),
		[](const CalendarEvent& first,
			const CalendarEvent& second)
		{
			return first.getDate() < second.getDate();
		}
	);

	return events;
}