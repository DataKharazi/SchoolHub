# SchoolHub

SchoolHub is a C++ study-management learning project built with Visual Studio.

## Interactive console

Set **SchoolHub** as the startup project and run without debugging. Add a student,
select that student, add a school, and select the school to open its dashboard.
The dashboard's Classes, Upcoming events, Schedule, and Open class options are
placeholders. Each nested menu has a Back option; selection lists use 0 for Back.
Invalid choices are retried, blank names cancel adding, and end-of-input exits.

All students and schools live in memory for the current session. The normal
console workflow does not open a database. New students start with GPA 0.0;
GPA entry is deferred. Names may contain spaces and need
not be unique; numbered selection distinguishes entries.

The ownership hierarchy is `Student -> School -> StudyClass`. School names now
belong only to `School`; `StudyClass::organizationName`, its accessors, and its
constructor argument have been removed. `Student::addSchool/getSchools` replace
direct course ownership. `School::removeStudyClass` accepts a zero-based index
and returns false for an invalid index without changing the collection.
Collections continue to return copies, matching the existing value-based design.
Editing a returned copy does not update its owner; future class management will
need a deliberate way to modify owned classes. The current dashboard only reads
a selected school. `ConsoleDemo` remains available but is no longer the entry
workflow. Input and output stay in `ConsoleApp`, outside the domain classes.

## Points-based grading

The console is a temporary developer interface. Reusable grading behavior lives
in the domain model and can later be called by Qt.

`Assignment` and `Exam` each own an `optional<Grade>`. Both constructors start
ungraded. Use `setGrade(earnedPoints, maxPoints)`, `getGrade()`, and `clearGrade()`.
`Grade` stores only earned and maximum points; `getPercentage()` and `getLetter()`
calculate results on demand. `GradeLetter` is A/B/C/D/F with thresholds
90/80/70/60. Letters use the unrounded percentage. Grading does not change an
assignment's completion status, and completing it does not assign a grade.

Extra credit is allowed: 22/20 is 110%, an A. Negative earned points, zero or
negative maximum points, NaN, infinity, and values whose percentage overflows
are rejected with `std::invalid_argument`. Nothing is clamped; a failed update
preserves the previous grade (or ungraded state). A zero earned score is a real
grade, not an ungraded marker.

`StudyClass::getGradeSummary()` derives an `optional<Grade>` from all graded
assignments and exams. Its earned/maximum values are course totals. Ungraded
work contributes neither points nor possible points; no graded work returns
`nullopt`, not F. For example, 18/20 plus 90/100 yields 108/120, 90%, A.
Totals that overflow throw `std::overflow_error`. No summary is cached or stored.

The unrelated `vector<double> grades`, `addGrade`, and `getGrades` APIs were
removed. Existing demo grades now belong to its assignment and exam. To grade
objects already owned by a course, use `setAssignmentGrade(index, earned, max)`
or `setExamGrade(index, earned, max)`; corresponding `clearAssignmentGrade` and
`clearExamGrade` methods restore the ungraded state. These indices are zero-based
and checked; invalid indices throw `std::out_of_range`. Collection getters still
return copies, so grading a retrieved copy does not modify the original.

Student GPA remains a manually stored, independent field and is not updated by
course grades. A future GPA milestone can map course letters to grade points
and combine them using course credits, after defining policies for incomplete
courses and school grading scales. This milestone adds no GPA calculation,
weighted categories, database changes, or grading console menus.

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

The 54 cases cover points grading and validation, course totals, letter boundaries,
dates and leap years, times, assignment status, recurring
meetings, school ownership and safe class removal, course and student collections, optional event times, calendar
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
