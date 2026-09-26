/*
	Name: Book.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Book class.
*/

#pragma once

#include <string>
using namespace std;

class Book
{
private:
	string bookName;
	string filePath;
	int bookMark;

public:
	Book();
	Book(string bookName, string filePath);

	void setBookName(string bookName);
	void setFilePath(string filePath);
	void setBookMark(int bookMark);

	string getBookName() const;
	string getFilePath() const;
	int getBookMark() const;
};