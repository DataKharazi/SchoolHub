/*
	Name: ClassMeeting.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the ClassMeeting class.
*/

#include "ClassMeeting.h"

ClassMeeting::ClassMeeting()
{
	weekDay = WeekDay::Monday;
}

ClassMeeting::ClassMeeting(
	WeekDay weekDay,
	Time startTime,
	Time endTime)
{
	this->weekDay = weekDay;
	this->startTime = startTime;
	this->endTime = endTime;
}

void ClassMeeting::setWeekDay(WeekDay weekDay)
{
	this->weekDay = weekDay;
}

void ClassMeeting::setStartTime(Time startTime)
{
	this->startTime = startTime;
}

void ClassMeeting::setEndTime(Time endTime)
{
	this->endTime = endTime;
}

WeekDay ClassMeeting::getWeekDay() const
{
	return weekDay;
}

Time ClassMeeting::getStartTime() const
{
	return startTime;
}

Time ClassMeeting::getEndTime() const
{
	return endTime;
}