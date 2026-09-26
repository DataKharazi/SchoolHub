/*
	Name: SchoolHub.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
*/

#include <iostream>

#include "Date.h"
#include "Book.h"
#include "Assignment.h"
#include "Exam.h"
#include "StudyClass.h"
#include "Student.h"
#include "Time.h"
#include "ClassMeeting.h"

using namespace std;

using namespace std;

string statusToString(AssignmentStatus status)
{
	if (status == AssignmentStatus::Finished)
		return "Finished";

	if (status == AssignmentStatus::Overdue)
		return "Overdue";

	return "Active";
}

string weekDayToString(WeekDay weekDay)
{
	if (weekDay == WeekDay::Monday)
		return "Monday";

	if (weekDay == WeekDay::Tuesday)
		return "Tuesday";

	if (weekDay == WeekDay::Wednesday)
		return "Wednesday";

	if (weekDay == WeekDay::Thursday)
		return "Thursday";

	if (weekDay == WeekDay::Friday)
		return "Friday";

	if (weekDay == WeekDay::Saturday)
		return "Saturday";

	return "Sunday";
}

int main()
{
	// Dates
	Date semesterStart(2026, 9, 1);
	Date semesterEnd(2026, 12, 20);
	Date currentDate(2026, 9, 26);

	// Book
	Book book(
		"Data Structures",
		"C:\\Books\\DataStructures.pdf"
	);

	book.setBookMark(125);

	// Assignments
	Assignment assignment1(
		"Set ADT Assignment",
		Date(2026, 9, 20),
		Date(2026, 10, 5)
	);

	Assignment assignment2(
		"Old Homework",
		Date(2026, 9, 10),
		Date(2026, 9, 20)
	);

	Assignment assignment3(
		"Assignment 1",
		Date(2026, 9, 1),
		Date(2026, 9, 15)
	);

	assignment3.setFinished(true);

	// Exam
	Exam exam(
		"Test 1",
		Date(2026, 10, 14)
	);

	// Study class
	StudyClass cs3700(
		"CS 3700 - Data Structures",
		"Kingsborough Community College",
		3,
		semesterStart,
		semesterEnd
	);

	// Class meetings
	ClassMeeting mondayMeeting(
		WeekDay::Monday,
		Time(11, 30),
		Time(12, 45)
	);

	ClassMeeting wednesdayMeeting(
		WeekDay::Wednesday,
		Time(11, 30),
		Time(12, 45)
	);

	cs3700.addClassMeeting(mondayMeeting);
	cs3700.addClassMeeting(wednesdayMeeting);

	cs3700.addBook(book);

	cs3700.addAssignment(assignment1);
	cs3700.addAssignment(assignment2);
	cs3700.addAssignment(assignment3);

	cs3700.addExam(exam);

	cs3700.addGrade(95);
	cs3700.addGrade(88);

	// Student
	Student student("David Kharazi", 3.5);

	student.addStudyClass(cs3700);

	// Test output
	cout << "Student: " << student.getStudentName() << endl;
	cout << "GPA: " << student.getGPA() << endl;

	cout << "\nClasses:" << endl;

	for (StudyClass studyClass : student.getStudyClasses())
	{
		cout << "\n" << studyClass.getClassName() << endl;

		cout << "Organization: "
			<< studyClass.getOrganizationName()
			<< endl;

		cout << "Credits: "
			<< studyClass.getCredits()
			<< endl;

		cout << "\nClass Schedule:" << endl;

		for (ClassMeeting meeting : studyClass.getClassMeetings())
		{
			cout << "- "
				<< weekDayToString(meeting.getWeekDay())
				<< " | "
				<< meeting.getStartTime().getHour()
				<< ":"
				<< meeting.getStartTime().getMinute()
				<< " - "
				<< meeting.getEndTime().getHour()
				<< ":"
				<< meeting.getEndTime().getMinute()
				<< endl;
		}

		cout << "\nBooks:" << endl;

		for (Book currentBook : studyClass.getBooks())
		{
			cout << "- "
				<< currentBook.getBookName()
				<< " | Bookmark: "
				<< currentBook.getBookMark()
				<< endl;
		}

		cout << "\nAssignments:" << endl;

		for (Assignment assignment : studyClass.getAssignments())
		{
			cout << "- "
				<< assignment.getDescription()
				<< " | "
				<< statusToString(
					assignment.getStatus(currentDate))
				<< endl;
		}

		cout << "\nExams:" << endl;

		for (Exam currentExam : studyClass.getExams())
		{
			cout << "- "
				<< currentExam.getExamName()
				<< endl;
		}
	}

	return 0;
}