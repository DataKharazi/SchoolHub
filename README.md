# SchoolHub

SchoolHub is a C++ study-management learning project built with Visual Studio.

## Automated tests

`SchoolHub.Tests` is a separate console project in `SchoolHub/SchoolHub.slnx`.
It compiles the existing domain, calendar, and database implementation files
directly, excluding the application entry point and console demo. No additional
test framework or package installation is required. Both projects use C++20
and the Visual Studio v145 toolset.

Open the solution, build it, then set **SchoolHub.Tests** as the startup project
and choose **Debug > Start Without Debugging**. Tests print a PASS/FAIL line
for each case and a final count. The process returns 0 on success and 1 on
failure. This small console runner is not integrated with Test Explorer.

From a Visual Studio Developer PowerShell, at the repository root:

```powershell
msbuild .\SchoolHub\SchoolHub.slnx /m /p:Configuration=Debug /p:Platform=x64
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
& .\SchoolHub.Tests\bin\x64\Debug\SchoolHub.Tests.exe
if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
```

Release is also supported: change both occurrences of `Debug` to `Release`.
Checks remain active in Release builds; the runner does not use `assert()`.
Win32 configurations are also available (the solution calls this platform x86).

The 25 cases cover dates and leap years, times, assignment status, recurring
meetings, course and student collections, optional event times, calendar
generation and ordering, the 31-meeting/35-event sample, and current database
operations and errors. SQLite tests use fresh `:memory:` databases, which are
discarded on close. They never open the application's `schoolhub.db` or create
database files. Test binaries and intermediate files are covered by the existing
`bin/` and `obj/` ignore rules.

To add a test, add a named case to the `tests` vector in
`SchoolHub.Tests/Tests.cpp` and use `CHECK(expression)`. A failed check reports
its expression and line, stops that case, and lets the remaining cases run.
When adding production implementation files, include them in the test project
as needed; the two projects currently maintain separate source lists.

Tests preserve current behavior: dates and times have no input validation,
finished assignments still produce deadline events, and calendar ordering is
by date only (there is no time-of-day ordering guarantee). Default `Date`
values contain zeros; do not call `getWeekDay()` on them, because it indexes
the month table using `month - 1`. The database currently only creates the
initial `students` table; full model persistence is outside this test suite.
