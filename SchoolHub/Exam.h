/*
	Name: Exam.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Exam class.
*/

#pragma once

#include <string>
#include "Date.h"

using namespace std;

class Exam
{
private:
	string examName;
	Date examDate;

public:
	Exam();
	Exam(string examName, Date examDate);

	void setExamName(string examName);
	void setExamDate(Date examDate);

	string getExamName() const;
	Date getExamDate() const;
};