/*
	Name: Date.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Date class.
*/

#include "Date.h"
#include <limits>
#include <stdexcept>

namespace
{
	int daysInMonth(int year, int month)
	{
		if (month == 2)
		{
			bool leapYear = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
			return leapYear ? 29 : 28;
		}
		if (month == 4 || month == 6 || month == 9 || month == 11)
			return 30;
		return 31;
	}
}

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
	if (!isValid())
		throw std::invalid_argument("Cannot calculate the weekday of an invalid date.");
	long long adjustedYear = year;

	int monthCodes[] =
	{
		0, 3, 2, 5, 0, 3,
		5, 1, 4, 6, 2, 4
	};

	if (month < 3)
		adjustedYear--;

	long long dayOfWeek =
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
	if (!isValid())
		throw std::invalid_argument("Cannot advance an invalid date.");
	int newYear = year;
	int newMonth = month;
	int newDay = day + 1;

	if (newDay > daysInMonth(year, month))
	{
		newDay = 1;
		newMonth++;

		if (newMonth > 12)
		{
			if (year == std::numeric_limits<int>::max())
				throw std::out_of_range("Date exceeds the maximum supported year.");
			newMonth = 1;
			newYear++;
		}
	}

	return Date(newYear, newMonth, newDay);
}

bool Date::isValid() const
{
	return year >= 1 && month >= 1 && month <= 12
		&& day >= 1 && day <= daysInMonth(year, month);
}

Date Date::getPreviousDay() const
{
	if (!isValid())
		throw std::invalid_argument("Cannot move back from an invalid date.");
	if (day > 1)
		return Date(year, month, day - 1);
	if (month > 1)
		return Date(year, month - 1, daysInMonth(year, month - 1));
	if (year == 1)
		throw std::out_of_range("Date precedes the minimum supported year.");
	return Date(year - 1, 12, 31);
}

Date Date::getWeekStartDate() const
{
	Date monday = *this;
	// WeekDay is ordered Monday (0) through Sunday (6).
	int daysSinceMonday = static_cast<int>(getWeekDay());
	for (int i = 0; i < daysSinceMonday; ++i)
		monday = monday.getPreviousDay();
	return monday;
}

Date Date::getWeekEndDate() const
{
	Date sunday = *this;
	int daysUntilSunday = 6 - static_cast<int>(getWeekDay());
	for (int i = 0; i < daysUntilSunday; ++i)
		sunday = sunday.getNextDay();
	return sunday;
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
