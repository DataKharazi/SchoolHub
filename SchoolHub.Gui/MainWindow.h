#pragma once

#include <QMainWindow>
#include "../SchoolHub/Database.h"

class QListWidget;
class QPushButton;
class QStackedWidget;
class QLabel;

class MainWindow : public QMainWindow
{
public:
    MainWindow();

private:
    Database database;
    std::optional<Student> currentStudent;
    std::optional<School> currentSchool;
    std::optional<StudyClass> currentClass;
    QListWidget* students;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* openButton;
    QStackedWidget* pages;
    QLabel* studentHeading;
    QListWidget* schools;
    QPushButton* addSchoolButton;
    QPushButton* deleteSchoolButton;
    QPushButton* openSchoolButton;
    QLabel* schoolStudentHeading;
    QLabel* schoolHeading;
    QListWidget* classes;
    QPushButton* addClassButton;
    QPushButton* deleteClassButton;
    QPushButton* openClassButton;
    QLabel* classDetails;
    bool ready = false;

    void initializeDatabase();
    void refreshStudents(std::int64_t selectedId = 0);
    void updateActions();
    void addStudent();
    void deleteStudent();
    void openStudent();
    bool reloadCurrentStudent();
    void populateSchools(std::int64_t selectedId = 0);
    void addSchool();
    void deleteSchool();
    void openSchool();
    School* findCurrentSchool(Student& student) const;
    bool reloadCurrentSchool();
    void populateClasses(std::int64_t selectedId = 0);
    void addClass();
    void deleteClass();
    void openClass();
    void showDatabaseError(const QString& message);
};
