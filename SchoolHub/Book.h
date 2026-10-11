/*
	Name: Book.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Book class.
*/

#pragma once
#include <cstdint>

#include <string>
using namespace std;

class Book
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
	string bookName;
	string filePath;
	int bookMark;

public:
    std::int64_t getId() const { return id; }
	Book();
	Book(string bookName, string filePath);

	void setBookName(string bookName);
	void setFilePath(string filePath);
	void setBookMark(int bookMark);

	string getBookName() const;
	string getFilePath() const;
	int getBookMark() const;
};
