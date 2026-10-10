/*
	Name: Calendar.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Calendar class.
*/

#include "Calendar.h"

#include <algorithm>
#include <stdexcept>

namespace
{
	void requireValidDate(const Date& date)
	{
		if (!date.isValid())
			throw std::invalid_argument("Calendar queries require valid dates.");
	}

	bool eventComesBefore(const CalendarEvent& first, const CalendarEvent& second)
	{
		if (!(first.getDate() == second.getDate()))
			return first.getDate() < second.getDate();
		if (first.hasTime() != second.hasTime())
			return first.hasTime();
		if (!first.hasTime())
			return false;
		Time firstTime = first.getStartTime().value();
		Time secondTime = second.getStartTime().value();
		if (firstTime.getHour() != secondTime.getHour())
			return firstTime.getHour() < secondTime.getHour();
		return firstTime.getMinute() < secondTime.getMinute();
	}
}

vector<CalendarEvent> Calendar::generateClassEvents(
	const StudyClass& studyClass) const
{
	requireValidDate(studyClass.getStartDate());
	requireValidDate(studyClass.getEndDate());
	if (studyClass.getStartDate() > studyClass.getEndDate())
		return {};
	return generateClassEvents(studyClass, studyClass.getStartDate(), studyClass.getEndDate());
}

vector<CalendarEvent> Calendar::generateClassEvents(const StudyClass& studyClass,
	const Date& rangeStart, const Date& rangeEnd) const
{
	vector<CalendarEvent> events;
	requireValidDate(rangeStart);
	requireValidDate(rangeEnd);
	requireValidDate(studyClass.getStartDate());
	requireValidDate(studyClass.getEndDate());
	if (rangeStart > rangeEnd)
		throw std::invalid_argument("Calendar range start must not follow its end.");

	Date currentDate = std::max(studyClass.getStartDate(), rangeStart);
	Date endDate = std::min(studyClass.getEndDate(), rangeEnd);

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

		if (currentDate == endDate)
			break;
		currentDate = currentDate.getNextDay();
	}

	stable_sort(events.begin(), events.end(), eventComesBefore);
	return events;
}

vector<CalendarEvent> Calendar::generateDeadlineEvents(
	const StudyClass& studyClass) const
{
	vector<CalendarEvent> events;

	for (Assignment assignment :
	studyClass.getAssignments())
	{
		requireValidDate(assignment.getDueDate());
		CalendarEvent event(
			assignment.getDescription(),
			assignment.getDueDate(),
			EventType::Assignment
		);

		events.push_back(event);
	}

	for (Exam exam : studyClass.getExams())
	{
		requireValidDate(exam.getExamDate());
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
		eventComesBefore
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
		eventComesBefore
	);

	return events;
}

vector<CalendarEvent> Calendar::getUpcomingEvents(const School& school,
	const Date& referenceDate) const
{
	requireValidDate(referenceDate);
	vector<CalendarEvent> events;
	for (const StudyClass& studyClass : school.getStudyClasses())
	{
		for (const CalendarEvent& event : generateDeadlineEvents(studyClass))
		{
			if (!(event.getDate() < referenceDate))
				events.push_back(event);
		}
	}
	stable_sort(events.begin(), events.end(), eventComesBefore);
	return events;
}

vector<CalendarEvent> Calendar::getWeeklySchedule(const School& school,
	const Date& referenceDate, ScheduleMode mode) const
{
	Date monday = referenceDate.getWeekStartDate();
	Date sunday = referenceDate.getWeekEndDate();
	if (mode != ScheduleMode::ClassesOnly && mode != ScheduleMode::EventsOnly
		&& mode != ScheduleMode::Combined)
		throw std::invalid_argument("Unknown schedule mode.");
	vector<CalendarEvent> events;
	for (const StudyClass& studyClass : school.getStudyClasses())
	{
		if (mode != ScheduleMode::EventsOnly)
		{
			auto meetings = generateClassEvents(studyClass, monday, sunday);
			events.insert(events.end(), meetings.begin(), meetings.end());
		}
		if (mode != ScheduleMode::ClassesOnly)
		{
			for (const CalendarEvent& event : generateDeadlineEvents(studyClass))
			{
				if (!(event.getDate() < monday) && !(event.getDate() > sunday))
					events.push_back(event);
			}
		}
	}
	stable_sort(events.begin(), events.end(), eventComesBefore);
	return events;
}
