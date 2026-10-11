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
Editing a returned copy does not update its owner. Checked `Student::getSchool(index)`
and `School::getStudyClass(index)` provide references to owned values for editing
and saving. These references must not be held across changes to their owning
vectors. The current dashboard only reads
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

## Schedule and event queries

`Calendar` provides reusable school-wide queries, independent of console input,
SQLite, and Qt. It reuses the existing course event generators and stores no
generated events. Callers pass a reference `Date`; obtaining the actual local
current date is the responsibility of the future UI/application boundary.

- `getUpcomingEvents(school, referenceDate)` returns assignments and exams on or
  after the reference date, so today counts. Ordinary meetings are excluded.
- `getWeeklySchedule(school, referenceDate, mode)` returns the Monday through
  Sunday week containing the reference date, inclusive. `ScheduleMode` offers
  `ClassesOnly`, `EventsOnly`, and `Combined` (the default).
- `generateClassEvents(course, rangeStart, rangeEnd)` adds an inclusive range
  overload. The original overload delegates to it. Meeting generation walks
  only the intersection of the requested range and the semester, so weekly
  queries do not generate an entire semester first.

Queries inspect every course in the selected school. Semester dates bound only
meetings; assignment/exam dates remain authoritative even outside a semester.
Finished assignments remain included, matching the existing event generator.
No additional current/past/future-course or completion filters are inferred.

All Calendar generators and queries sort by date, then timed events by start
hour/minute, then untimed events. Stable sorting preserves insertion order for
equal start times and untimed ties: school course order, then source collection
order (assignments before exams within a course). No artificial times are added.

Date adds `isValid()`, `getPreviousDay()`, `getWeekStartDate()` (Monday), and
`getWeekEndDate()` (Sunday). To move weeks, use the prior Monday's previous day
or the prior Sunday's next day as the next query reference:

```cpp
Calendar calendar;
auto week = calendar.getWeeklySchedule(school, referenceDate);
Date previous = referenceDate.getWeekStartDate().getPreviousDay();
Date next = referenceDate.getWeekEndDate().getNextDay();
auto upcoming = calendar.getUpcomingEvents(school, referenceDate);
```

The local Date implementation still allowed invalid values at the start of this
milestone. Constructors/setters retain that behavior, including the all-zero
default, while date arithmetic and Calendar reject invalid dates with
`std::invalid_argument`. Valid dates use Gregorian rules and positive `int`
years. Arithmetic past the supported year bounds throws `std::out_of_range`;
a week that cannot be represented in full also throws. Reversed query ranges
and invalid schedule modes throw `std::invalid_argument`. A reversed semester
produces no meetings, preserving the previous generator behavior. Queries fail
on invalid source dates in the event category being requested.

Current limitations: `Time` still has no input validation, collection getters
copy their values, and `CalendarEvent` has no source course/object identifier.
The future GUI will need source identity to reliably open an event's originating
object, especially when titles repeat. These queries preserve events separately
and do not deduplicate titles or introduce an identity redesign.

## SQLite persistence

`Database` now saves and loads complete student hierarchies in ONE SQLite file.
Its implementation is split between `Database.cpp` (connection/basic SQL) and
`DatabasePersistence.cpp` (schema, prepared statements, hierarchy operations).
SQL stays out of the domain models.

The version 1 schema contains seven source-data tables:

| Table | Parent foreign key | Stored source data |
| --- | --- | --- |
| students | none | name, manually stored GPA |
| schools | student_id -> students.id | name, collection position |
| study_classes | school_id -> schools.id | name, credits, semester dates, position |
| assignments | study_class_id -> study_classes.id | description, start/due dates, finished, optional points, position |
| exams | study_class_id -> study_classes.id | name, exam date, optional points, position |
| books | study_class_id -> study_classes.id | name, file path, bookmark, position |
| class_meetings | study_class_id -> study_classes.id | weekday, start/end times, position |

All parent foreign keys are NOT NULL and use ON DELETE CASCADE. Foreign keys
are enabled on open, and persistence operations refuse to run if enforcement
has been disabled. Every child has exactly one path to its owning student.
Loading follows these IDs at every level; names never establish relationships.
Deleting a student cascades through only that student's hierarchy;
`deleteSchool(studentId, schoolId)` additionally checks the school's owner.

Dates use separate INTEGER year/month/day columns; times use INTEGER hour/minute
columns. Weekdays use the existing Monday=0 through Sunday=6 enum values.
These representations are locale independent. Saving/loading validates dates
and time components. Non-finite GPA/credits are rejected without imposing a new
GPA scale. Optional grades use two NULLs for ungraded work or two validated
point values; a CHECK constraint forbids half-null grades. Percentages, letters,
assignment status, CalendarEvents, and schedule/query results are never stored.

Each persisted domain type exposes `getId()` returning `std::int64_t`. New
objects have ID 0; SQLite assigns positive AUTOINCREMENT IDs. Database is a
friend solely to assign identities and traverse owned values for persistence;
there are no public ID setters. IDs remain stable across save/load/update and
are not reused after deletion. They are scoped to a particular database and
entity type. Copies of saved objects retain their identity; to create a distinct
entity, construct a new object rather than reusing a saved copy. These IDs give
future Qt code a stable identity to work with, though CalendarEvent source-ID
propagation remains a separate task.

Public operations:

```cpp
Database database;
if (!database.open("schoolhub.db") || !database.createTables()) {
    // Report database.getLastError().
}
// After a successful open and schema initialization:
// database.saveStudent(student);        // bool; updates student IDs on success
// database.loadStudent(student.getId()); // optional<Student>
// database.loadStudents();              // vector<Student>, ordered by ID
// database.deleteSchool(studentId, schoolId); // bool, owner-scoped cascade
// database.deleteStudent(studentId);          // bool, full cascade
```

`saveStudent(Student&)` saves an authoritative snapshot in BEGIN IMMEDIATE / COMMIT.
It updates existing rows, inserts new rows, and deletes children omitted from
that parent's collection. Existing child IDs must still belong to the same
parent; attempts to graft another student's school, class, or leaf objects are
rejected. Duplicate saved IDs and stale deleted IDs are also rejected. A private
working copy receives new IDs; the caller sees them only after COMMIT. Any
failure rolls back the entire save, including updates and deletions, and leaves
the caller's original IDs unchanged. Reads use a transaction for a consistent
snapshot and never return partial hierarchies on error. Collection positions
preserve source order and stable event ties. All user data uses bound parameters.

Errors are reported through `getLastError()`: writes return false, a failed or
missing `loadStudent` returns nullopt, and failed `loadStudents` returns an empty
vector with a nonempty error. An empty successful list has an empty error.
Operations refuse caller-owned transactions so they cannot accidentally commit
or roll back unrelated SQL. Database connections are noncopyable.

This is a single-editor snapshot API, without conflict/version tracking. Reload
after direct deletes before saving an older in-memory snapshot. Raw `execute`
remains a low-level developer facility and is not an authorization boundary.
The tests verify that supported hierarchy saves/loads/deletes do not mix student
data, including when every displayed name is identical.

**Old development database:** a version-0 database containing the early students
table is rejected without modifying its data. Back it up if needed, then manually
delete the old development `schoolhub.db` and let `createTables()` recreate it.
The old previous_gpa/previous_credits columns are replaced by the current model's
gpa field. There is no automatic data migration. Schema version 1 is recorded in
PRAGMA user_version; unknown versions are rejected. `schoolhub.db` and SQLite
WAL/SHM files remain ignored by Git. This milestone does not delete your database
or connect the interactive console to autosave; that console remains session-only.

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

The 93 cases cover full hierarchy persistence, student isolation, stable IDs,
cascading deletion, rollback, schema checks, file reopen, week boundaries/navigation, school-wide event queries,
semester clipping and stable same-day ordering, points grading and validation, course totals, letter boundaries,
dates and leap years, times, assignment status, recurring
meetings, school ownership and safe class removal, course and student collections, optional event times, calendar
generation and ordering, the 31-meeting/35-event sample, and current database
operations and errors. SQLite tests use fresh `:memory:` databases, which are
discarded on close. One durability test creates a uniquely named temporary file
and removes it after closing both connections. Tests never open the application's
`schoolhub.db`. Test binaries and intermediate files are covered by the existing
`bin/` and `obj/` ignore rules.

To add a test, add a named case to the `tests` vector in
`SchoolHub.Tests/Tests.cpp` and use `CHECK(expression)`. A failed check reports
its expression and line, stops that case, and lets the remaining cases run.
When adding production implementation files, include them in the test project
as needed; the two projects currently maintain separate source lists.

All 74 earlier cases remain; the initial schema smoke test now checks the current
gpa column. Nineteen persistence cases were added, including a complete round trip
after discarding the original objects and regeneration of grades and calendars.
