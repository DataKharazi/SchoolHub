/*
	Name: ClassMeeting.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the ClassMeeting class.
*/

#pragma once

#include "Time.h"
#include "WeekDay.h"

class ClassMeeting
{
private:
	WeekDay weekDay;
	Time startTime;
	Time endTime;

public:
	ClassMeeting();
	ClassMeeting(WeekDay weekDay, Time startTime, Time endTime);

	void setWeekDay(WeekDay weekDay);
	void setStartTime(Time startTime);
	void setEndTime(Time endTime);

	WeekDay getWeekDay() const;
	Time getStartTime() const;
	Time getEndTime() const;
};