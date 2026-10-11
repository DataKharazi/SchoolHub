/*
	Name: ClassMeeting.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the ClassMeeting class.
*/

#pragma once
#include <cstdint>

#include "Time.h"
#include "WeekDay.h"

class ClassMeeting
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	WeekDay weekDay;
	Time startTime;
	Time endTime;

public:
    std::int64_t getId() const { return id; }
	ClassMeeting();
	ClassMeeting(WeekDay weekDay, Time startTime, Time endTime);

	void setWeekDay(WeekDay weekDay);
	void setStartTime(Time startTime);
	void setEndTime(Time endTime);

	WeekDay getWeekDay() const;
	Time getStartTime() const;
	Time getEndTime() const;
};
