/*
	Name: Database.cpp
	Copyright: David Kharazi 2026
	Author: David Kharazi
	Date: 26 September 2026
	Description: Implements the Database class.
*/

#include "Database.h"
#include "External/SQLite/sqlite3.h"

Database::Database()
{
	database = nullptr;
	lastError = "";
}

Database::~Database()
{
	close();
}

bool Database::open(const string& filePath)
{
	close();

	int result =
		sqlite3_open(
			filePath.c_str(),
			&database
		);

	if (result != SQLITE_OK)
	{
		if (database != nullptr)
		{
			lastError =
				sqlite3_errmsg(database);

			sqlite3_close(database);
		}

		database = nullptr;

		return false;
	}

	lastError = "";

	// Enable foreign key support for future tables.
	if (!execute("PRAGMA foreign_keys = ON;"))
	{
		close();

		return false;
	}

	return true;
}

void Database::close()
{
	if (database != nullptr)
	{
		sqlite3_close(database);

		database = nullptr;
	}
}

bool Database::isOpen() const
{
	return database != nullptr;
}

bool Database::execute(const string& sql)
{
	if (database == nullptr)
	{
		lastError =
			"Database is not open.";

		return false;
	}

	char* errorMessage = nullptr;

	int result =
		sqlite3_exec(
			database,
			sql.c_str(),
			nullptr,
			nullptr,
			&errorMessage
		);

	if (result != SQLITE_OK)
	{
		if (errorMessage != nullptr)
		{
			lastError = errorMessage;

			sqlite3_free(errorMessage);
		}
		else
		{
			lastError =
				sqlite3_errmsg(database);
		}

		return false;
	}

	lastError = "";

	return true;
}

bool Database::createTables()
{
	string sql =
		R"(
			CREATE TABLE IF NOT EXISTS students
			(
				id INTEGER PRIMARY KEY AUTOINCREMENT,
				name TEXT NOT NULL,
				previous_gpa REAL NOT NULL DEFAULT 0.0,
				previous_credits REAL NOT NULL DEFAULT 0.0
			);
		)";

	return execute(sql);
}

string Database::getLastError() const
{
	return lastError;
}