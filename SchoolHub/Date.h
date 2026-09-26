/*
	Name: Date.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Date class.
*/

class Date
{
private:
    int year;
    int month;
    int day;

public:
    Date();
    Date(int year, int month, int day);

    int getYear() const;
    int getMonth() const;
    int getDay() const;
};
