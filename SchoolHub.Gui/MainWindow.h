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
    QListWidget* students;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* openButton;
    QStackedWidget* pages;
    QLabel* studentHeading;
    bool ready = false;

    void initializeDatabase();
    void refreshStudents(std::int64_t selectedId = 0);
    void updateActions();
    void addStudent();
    void deleteStudent();
    void openStudent();
    void showDatabaseError(const QString& message);
};
