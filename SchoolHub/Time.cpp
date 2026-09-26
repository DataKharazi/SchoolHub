/*
	Name: Time.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Time class.
*/

#include "Time.h"

Time::Time()
{
	hour = 0;
	minute = 0;
}

Time::Time(int hour, int minute)
{
	this->hour = hour;
	this->minute = minute;
}

void Time::setHour(int hour)
{
	this->hour = hour;
}

void Time::setMinute(int minute)
{
	this->minute = minute;
}

int Time::getHour() const
{
	return hour;
}

int Time::getMinute() const
{
	return minute;
}