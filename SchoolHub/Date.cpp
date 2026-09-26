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