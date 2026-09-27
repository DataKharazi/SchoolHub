/*
	Name: Date.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Date class.
*/

#include "Date.h"

Date::Date()
{
	year = 0;
	month = 0;
	day = 0;
}

Date::Date(int year, int month, int day)
{
	this->year = year;
	this->month = month;
	this->day = day;
}

void Date::setYear(int year)
{
	this->year = year;
}

void Date::setMonth(int month)
{
	this->month = month;
}

void Date::setDay(int day)
{
	this->day = day;
}

int Date::getYear() const
{
	return year;
}

int Date::getMonth() const
{
	return month;
}

int Date::getDay() const
{
	return day;
}

WeekDay Date::getWeekDay() const
{
	int adjustedYear = year;

	int monthCodes[] =
	{
		0, 3, 2, 5, 0, 3,
		5, 1, 4, 6, 2, 4
	};

	if (month < 3)
		adjustedYear--;

	int dayOfWeek =
		(
			adjustedYear
			+ adjustedYear / 4
			- adjustedYear / 100
			+ adjustedYear / 400
			+ monthCodes[month - 1]
			+ day
			) % 7;

	switch (dayOfWeek)
	{
	case 0:
		return WeekDay::Sunday;

	case 1:
		return WeekDay::Monday;

	case 2:
		return WeekDay::Tuesday;

	case 3:
		return WeekDay::Wednesday;

	case 4:
		return WeekDay::Thursday;

	case 5:
		return WeekDay::Friday;

	default:
		return WeekDay::Saturday;
	}
}

Date Date::getNextDay() const
{
	int newYear = year;
	int newMonth = month;
	int newDay = day + 1;

	int daysInMonth;

	if (month == 2)
	{
		bool leapYear =
			(year % 400 == 0) ||
			(year % 4 == 0 && year % 100 != 0);

		daysInMonth = leapYear ? 29 : 28;
	}
	else if (
		month == 4 ||
		month == 6 ||
		month == 9 ||
		month == 11)
	{
		daysInMonth = 30;
	}
	else
	{
		daysInMonth = 31;
	}

	if (newDay > daysInMonth)
	{
		newDay = 1;
		newMonth++;

		if (newMonth > 12)
		{
			newMonth = 1;
			newYear++;
		}
	}

	return Date(newYear, newMonth, newDay);
}

bool Date::operator==(const Date& other) const
{
	return year == other.year &&
		month == other.month &&
		day == other.day;
}

bool Date::operator<(const Date& other) const
{
	if (year != other.year)
		return year < other.year;

	if (month != other.month)
		return month < other.month;

	return day < other.day;
}

bool Date::operator>(const Date& other) const
{
	return other < *this;
}