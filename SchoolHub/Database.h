/*
	Name: Database.h
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Defines the Database class.
*/

#pragma once

#include <string>

using namespace std;

struct sqlite3;

class Database
{
private:
	sqlite3* database;
	string lastError;

public:
	Database();
	~Database();

	bool open(const string& filePath);
	void close();

	bool isOpen() const;

	bool execute(const string& sql);
	bool createTables();

	string getLastError() const;
};