/*
	Name: ConsoleDemo.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Runs the SchoolHub console demonstration.
*/

#include <iostream>
#include <vector>
#include <string>

#include "ConsoleDemo.h"
#include "Database.h"
#include "Date.h"
#include "Time.h"
#include "Book.h"
#include "Assignment.h"
#include "Exam.h"
#include "ClassMeeting.h"
#include "StudyClass.h"
#include "Student.h"
#include "CalendarEvent.h"
#include "Calendar.h"

using namespace std;


// --------------------------------------------------
// Helper Functions
// --------------------------------------------------

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


string eventTypeToString(EventType type)
{
	if (type == EventType::ClassMeeting)
		return "Class";

	if (type == EventType::Assignment)
		return "Assignment";

	return "Exam";
}


// --------------------------------------------------
// Console Demo
// --------------------------------------------------

void runConsoleDemo()
{
	Database database;

	cout << "Database Test:" << endl;

	if (database.open("schoolhub.db"))
	{
		cout << "schoolhub.db opened successfully."
			<< endl;

		if (database.createTables())
		{
			cout << "Database tables created successfully."
				<< endl;
		}
		else
		{
			cout << "Table creation error: "
				<< database.getLastError()
				<< endl;
		}
	}
	else
	{
		cout << "Database error: "
			<< database.getLastError()
			<< endl;
	}

	cout << endl;
	// --------------------------------------------------
	// Dates
	// --------------------------------------------------

	Date semesterStart(2026, 9, 1);
	Date semesterEnd(2026, 12, 20);
	Date currentDate(2026, 9, 26);


	// --------------------------------------------------
	// Book
	// --------------------------------------------------

	Book book(
		"Data Structures",
		"C:\\Books\\DataStructures.pdf"
	);

	book.setBookMark(125);


	// --------------------------------------------------
	// Assignments
	// --------------------------------------------------

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


	// --------------------------------------------------
	// Exam
	// --------------------------------------------------

	Exam exam(
		"Test 1",
		Date(2026, 10, 14)
	);


	// --------------------------------------------------
	// Study Class
	// --------------------------------------------------

	StudyClass cs3700(
		"CS 3700 - Data Structures",
		"Kingsborough Community College",
		3,
		semesterStart,
		semesterEnd
	);


	// --------------------------------------------------
	// Recurring Class Meetings
	// --------------------------------------------------

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


	// --------------------------------------------------
	// Add Course Data
	// --------------------------------------------------

	cs3700.addBook(book);

	cs3700.addAssignment(assignment1);
	cs3700.addAssignment(assignment2);
	cs3700.addAssignment(assignment3);

	cs3700.addExam(exam);

	cs3700.addGrade(95);
	cs3700.addGrade(88);


	// --------------------------------------------------
	// Student
	// --------------------------------------------------

	Student student(
		"David Kharazi",
		3.5
	);

	student.addStudyClass(cs3700);


	// ==================================================
	// Student / Course Output
	// ==================================================

	cout << "Student: "
		<< student.getStudentName()
		<< endl;

	cout << "GPA: "
		<< student.getGPA()
		<< endl;

	cout << "\nClasses:" << endl;


	for (const StudyClass& studyClass :
		student.getStudyClasses())
	{
		cout << "\n"
			<< studyClass.getClassName()
			<< endl;

		cout << "Organization: "
			<< studyClass.getOrganizationName()
			<< endl;

		cout << "Credits: "
			<< studyClass.getCredits()
			<< endl;


		// Class schedule

		cout << "\nClass Schedule:" << endl;

		for (const ClassMeeting& meeting :
			studyClass.getClassMeetings())
		{
			cout
				<< "- "
				<< weekDayToString(
					meeting.getWeekDay())
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


		// Books

		cout << "\nBooks:" << endl;

		for (const Book& currentBook :
			studyClass.getBooks())
		{
			cout
				<< "- "
				<< currentBook.getBookName()
				<< " | Bookmark: "
				<< currentBook.getBookMark()
				<< endl;
		}


		// Assignments

		cout << "\nAssignments:" << endl;

		for (const Assignment& assignment :
			studyClass.getAssignments())
		{
			cout
				<< "- "
				<< assignment.getDescription()
				<< " | "
				<< statusToString(
					assignment.getStatus(currentDate))
				<< endl;
		}


		// Exams

		cout << "\nExams:" << endl;

		for (const Exam& currentExam :
			studyClass.getExams())
		{
			cout
				<< "- "
				<< currentExam.getExamName()
				<< endl;
		}
	}


	// ==================================================
	// CalendarEvent Test
	// ==================================================

	CalendarEvent assignmentEvent(
		"Set ADT Assignment",
		Date(2026, 10, 5),
		EventType::Assignment
	);

	CalendarEvent classEvent(
		"CS 3700 - Data Structures",
		Date(2026, 9, 28),
		EventType::ClassMeeting,
		Time(11, 30),
		Time(12, 45)
	);


	cout << "\nCalendar Event Test:" << endl;

	cout
		<< assignmentEvent.getTitle()
		<< " | Has time: "
		<< (assignmentEvent.hasTime()
			? "Yes"
			: "No")
		<< endl;

	cout
		<< classEvent.getTitle()
		<< " | Has time: "
		<< (classEvent.hasTime()
			? "Yes"
			: "No")
		<< endl;


	if (classEvent.hasTime())
	{
		Time start =
			classEvent.getStartTime().value();

		Time end =
			classEvent.getEndTime().value();

		cout
			<< "Class time: "
			<< start.getHour()
			<< ":"
			<< start.getMinute()
			<< " - "
			<< end.getHour()
			<< ":"
			<< end.getMinute()
			<< endl;
	}


	// ==================================================
	// Date Algorithm Test
	// ==================================================

	cout << "\nDate Algorithm Test:" << endl;

	Date testDate(2026, 9, 1);

	for (int i = 0; i < 9; i++)
	{
		cout
			<< testDate.getMonth()
			<< "/"
			<< testDate.getDay()
			<< "/"
			<< testDate.getYear()
			<< " | "
			<< weekDayToString(
				testDate.getWeekDay())
			<< endl;

		testDate =
			testDate.getNextDay();
	}


	// ==================================================
	// Calendar
	// ==================================================

	Calendar calendar;


	// ==================================================
	// Generated Class Calendar
	// ==================================================

	vector<CalendarEvent> classEvents =
		calendar.generateClassEvents(cs3700);


	cout << "\nGenerated Class Calendar:"
		<< endl;

	cout
		<< "Total class events: "
		<< classEvents.size()
		<< endl;


	for (const CalendarEvent& event :
		classEvents)
	{
		Date eventDate =
			event.getDate();

		cout
			<< eventDate.getMonth()
			<< "/"
			<< eventDate.getDay()
			<< "/"
			<< eventDate.getYear()
			<< " | "
			<< weekDayToString(
				eventDate.getWeekDay())
			<< " | "
			<< event.getTitle();


		if (event.hasTime())
		{
			Time start =
				event.getStartTime().value();

			Time end =
				event.getEndTime().value();

			cout
				<< " | "
				<< start.getHour()
				<< ":"
				<< start.getMinute()
				<< " - "
				<< end.getHour()
				<< ":"
				<< end.getMinute();
		}

		cout << endl;
	}


	// ==================================================
	// Complete Course Calendar
	// ==================================================

	vector<CalendarEvent> allEvents =
		calendar.generateAllEvents(cs3700);


	cout << "\nComplete Course Calendar:"
		<< endl;

	cout
		<< "Total events: "
		<< allEvents.size()
		<< endl;


	for (const CalendarEvent& event :
		allEvents)
	{
		Date eventDate =
			event.getDate();

		cout
			<< eventDate.getMonth()
			<< "/"
			<< eventDate.getDay()
			<< "/"
			<< eventDate.getYear()
			<< " | "
			<< eventTypeToString(
				event.getType())
			<< " | "
			<< event.getTitle();


		if (event.hasTime())
		{
			Time start =
				event.getStartTime().value();

			Time end =
				event.getEndTime().value();

			cout
				<< " | "
				<< start.getHour()
				<< ":"
				<< start.getMinute()
				<< " - "
				<< end.getHour()
				<< ":"
				<< end.getMinute();
		}

		cout << endl;
	}
}