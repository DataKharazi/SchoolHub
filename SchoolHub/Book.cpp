/*
	Name: Book.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Book class.
*/

#include "Book.h"

Book::Book()
{
	bookName = "";
	filePath = "";
	bookMark = 0;
}

Book::Book(string bookName, string filePath)
{
	this->bookName = bookName;
	this->filePath = filePath;
	bookMark = 0;
}

void Book::setBookName(string bookName)
{
	this->bookName = bookName;
}

void Book::setFilePath(string filePath)
{
	this->filePath = filePath;
}

void Book::setBookMark(int bookMark)
{
	this->bookMark = bookMark;
}

string Book::getBookName() const
{
	return bookName;
}

string Book::getFilePath() const
{
	return filePath;
}

int Book::getBookMark() const
{
	return bookMark;
}
