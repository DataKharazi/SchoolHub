#include "Exam.h"
/*
	Name: Exam.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Exam class.
*/


Exam::Exam()
{
	examName = "";
}

Exam::Exam(string examName, Date examDate)
{
	this->examName = examName;
	this->examDate = examDate;
}

void Exam::setExamName(string examName)
{
	this->examName = examName;
}

void Exam::setExamDate(Date examDate)
{
	this->examDate = examDate;
}

string Exam::getExamName() const
{
	return examName;
}

Date Exam::getExamDate() const
{
	return examDate;
}