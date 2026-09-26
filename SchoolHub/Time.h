/*
	Name: Time.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Time class.
*/

#pragma once

class Time
{
private:
	int hour;
	int minute;

public:
	Time();
	Time(int hour, int minute);

	void setHour(int hour);
	void setMinute(int minute);

	int getHour() const;
	int getMinute() const;
};