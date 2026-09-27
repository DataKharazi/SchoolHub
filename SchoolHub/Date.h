/*/*
	Name: Date.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Date class.
*/

#pragma once

#include "WeekDay.h"

class Date
{
private:
	int year;
	int month;
	int day;

public:
	Date();
	Date(int year, int month, int day);

	void setYear(int year);
	void setMonth(int month);
	void setDay(int day);

	int getYear() const;
	int getMonth() const;
	int getDay() const;

	WeekDay getWeekDay() const;
	Date getNextDay() const;

	bool operator==(const Date& other) const;
	bool operator<(const Date& other) const;
	bool operator>(const Date& other) const;
};