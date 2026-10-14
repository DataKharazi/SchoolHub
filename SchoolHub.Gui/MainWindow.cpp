#include "MainWindow.h"
#include "AddClassDialog.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

namespace {
    enum Page { StudentsPage, SchoolsPage, SchoolPage, ClassPage };

    QString dateText(const Date& date)
    {
        return QString("%1-%2-%3").arg(date.getYear(), 4, 10, QChar('0'))
            .arg(date.getMonth(), 2, 10, QChar('0')).arg(date.getDay(), 2, 10, QChar('0'));
    }
}

MainWindow::MainWindow()
{
    setWindowTitle("SchoolHub");
    resize(640, 440);
    pages = new QStackedWidget(this);
    setCentralWidget(pages);

    auto* listPage = new QWidget(pages);
    auto* layout = new QVBoxLayout(listPage);
    layout->addWidget(new QLabel("<h1>SchoolHub</h1><h2>Students</h2>", listPage));
    students = new QListWidget(listPage);
    students->setAccessibleName("Students");
    layout->addWidget(students);
    auto* buttons = new QHBoxLayout;
    addButton = new QPushButton("Add Student", listPage);
    deleteButton = new QPushButton("Delete Student", listPage);
    openButton = new QPushButton("Open Student", listPage);
    buttons->addWidget(addButton);
    buttons->addWidget(deleteButton);
    buttons->addWidget(openButton);
    layout->addLayout(buttons);
    pages->addWidget(listPage);

    auto* studentPage = new QWidget(pages);
    auto* studentLayout = new QVBoxLayout(studentPage);
    studentHeading = new QLabel(studentPage);
    studentHeading->setTextFormat(Qt::PlainText);
    studentHeading->setWordWrap(true);
    studentLayout->addWidget(studentHeading);
    studentLayout->addWidget(new QLabel("<h2>Schools</h2>", studentPage));
    schools = new QListWidget(studentPage);
    schools->setAccessibleName("Schools");
    studentLayout->addWidget(schools);
    auto* schoolButtons = new QHBoxLayout;
    addSchoolButton = new QPushButton("Add School", studentPage);
    deleteSchoolButton = new QPushButton("Delete School", studentPage);
    openSchoolButton = new QPushButton("Open School", studentPage);
    schoolButtons->addWidget(addSchoolButton);
    schoolButtons->addWidget(deleteSchoolButton);
    schoolButtons->addWidget(openSchoolButton);
    studentLayout->addLayout(schoolButtons);
    auto* back = new QPushButton("Back to Students", studentPage);
    studentLayout->addWidget(back);
    pages->addWidget(studentPage);

    auto* schoolPage = new QWidget(pages);
    auto* schoolLayout = new QVBoxLayout(schoolPage);
    schoolStudentHeading = new QLabel(schoolPage);
    schoolStudentHeading->setTextFormat(Qt::PlainText);
    schoolStudentHeading->setWordWrap(true);
    schoolHeading = new QLabel(schoolPage);
    schoolHeading->setTextFormat(Qt::PlainText);
    schoolHeading->setWordWrap(true);
    schoolLayout->addWidget(schoolStudentHeading);
    schoolLayout->addWidget(schoolHeading);
    schoolLayout->addWidget(new QLabel("<h2>Classes</h2>", schoolPage));
    classes = new QListWidget(schoolPage);
    classes->setAccessibleName("Classes");
    schoolLayout->addWidget(classes);
    auto* classButtons = new QHBoxLayout;
    addClassButton = new QPushButton("Add Class", schoolPage);
    deleteClassButton = new QPushButton("Delete Class", schoolPage);
    openClassButton = new QPushButton("Open Class", schoolPage);
    classButtons->addWidget(addClassButton);
    classButtons->addWidget(deleteClassButton);
    classButtons->addWidget(openClassButton);
    schoolLayout->addLayout(classButtons);
    auto* backToSchools = new QPushButton("Back to Schools", schoolPage);
    schoolLayout->addWidget(backToSchools);
    pages->addWidget(schoolPage);

    auto* classPage = new QWidget(pages);
    auto* classLayout = new QVBoxLayout(classPage);
    classDetails = new QLabel(classPage);
    classDetails->setTextFormat(Qt::PlainText);
    classDetails->setWordWrap(true);
    classLayout->addWidget(classDetails);
    classLayout->addWidget(new QLabel("Assignments and exams screen coming next.", classPage));
    classLayout->addStretch();
    auto* backToClasses = new QPushButton("Back to Classes", classPage);
    classLayout->addWidget(backToClasses);
    pages->addWidget(classPage);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteStudent);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openStudent);
    connect(students, &QListWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(students, &QListWidget::itemDoubleClicked, this, [this] { openStudent(); });
    connect(back, &QPushButton::clicked, this, [this] {
        const auto id = currentStudent ? currentStudent->getId() : 0;
        currentClass.reset();
        currentSchool.reset();
        currentStudent.reset();
        schools->clear();
        classes->clear();
        pages->setCurrentIndex(StudentsPage);
        refreshStudents(id);
    });
    connect(addSchoolButton, &QPushButton::clicked, this, &MainWindow::addSchool);
    connect(deleteSchoolButton, &QPushButton::clicked, this, &MainWindow::deleteSchool);
    connect(openSchoolButton, &QPushButton::clicked, this, &MainWindow::openSchool);
    connect(schools, &QListWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(schools, &QListWidget::itemDoubleClicked, this, [this] { openSchool(); });
    connect(backToSchools, &QPushButton::clicked, this, [this] {
        const auto id = currentSchool ? currentSchool->getId() : 0;
        currentClass.reset();
        classes->clear();
        currentSchool.reset();
        if (reloadCurrentStudent()) {
            populateSchools(id);
            pages->setCurrentIndex(SchoolsPage);
        }
    });
    connect(addClassButton, &QPushButton::clicked, this, &MainWindow::addClass);
    connect(deleteClassButton, &QPushButton::clicked, this, &MainWindow::deleteClass);
    connect(openClassButton, &QPushButton::clicked, this, &MainWindow::openClass);
    connect(classes, &QListWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(classes, &QListWidget::itemDoubleClicked, this, [this] { openClass(); });
    connect(backToClasses, &QPushButton::clicked, this, [this] {
        const auto id = currentClass ? currentClass->getId() : 0;
        currentClass.reset();
        if (reloadCurrentSchool()) {
            populateClasses(id);
            pages->setCurrentIndex(SchoolPage);
        }
    });
    updateActions();
    // Defer error dialogs until the window is visible.
    QTimer::singleShot(0, this, &MainWindow::initializeDatabase);
}

void MainWindow::initializeDatabase()
{
    QString path;
    const auto arguments = QApplication::arguments();
    // Optional explicit file for isolated development/manual verification.
    if (arguments.size() == 3 && arguments[1] == "--database") {
        path = QFileInfo(arguments[2]).absoluteFilePath();
    } else if (arguments.size() != 1) {
        QMessageBox::critical(this, "SchoolHub", "Usage: SchoolHub.Gui [--database <file>]");
        return;
    } else {
        const auto directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        if (directory.isEmpty() || !QDir().mkpath(directory)) {
            QMessageBox::critical(this, "Database unavailable", "Could not create the SchoolHub data folder. Check folder permissions and restart.");
            return;
        }
        path = QDir(directory).filePath("schoolhub.db");
    }
    statusBar()->setToolTip(path);
    if (!database.open(path.toUtf8().toStdString())) {
        showDatabaseError("Could not open the SchoolHub database. Check the file location and permissions, then restart.");
        return;
    }
    if (!database.createTables()) {
        showDatabaseError("This database could not be initialized. It may use an older development format. Keep a backup; no automatic migration is performed.");
        return;
    }
    ready = true;
    refreshStudents();
}

void MainWindow::showDatabaseError(const QString& message)
{
    QMessageBox box(QMessageBox::Critical, "SchoolHub database", message, QMessageBox::Ok, this);
    box.setDetailedText(QString::fromStdString(database.getLastError()));
    box.exec();
}

void MainWindow::refreshStudents(std::int64_t selectedId)
{
    students->clear();
    const auto loaded = database.loadStudents();
    if (!database.getLastError().empty()) {
        statusBar()->showMessage("Could not load students. Restart to try again.");
        showDatabaseError("Could not load students from the database.");
        updateActions();
        return;
    }
    for (const auto& student : loaded) {
        auto* item = new QListWidgetItem(QString::fromStdString(student.getStudentName()), students);
        item->setData(Qt::UserRole, QVariant::fromValue<qlonglong>(student.getId()));
        item->setToolTip(QString("Student ID: %1").arg(student.getId()));
        if (student.getId() == selectedId)
            students->setCurrentItem(item);
    }
    statusBar()->showMessage(loaded.empty() ? "No students yet. Add a student to begin."
        : QString("%1 student(s)").arg(loaded.size()));
    updateActions();
}

void MainWindow::updateActions()
{
    addButton->setEnabled(ready);
    const bool selected = ready && students->currentItem();
    deleteButton->setEnabled(selected);
    openButton->setEnabled(selected);
    const bool studentOpen = ready && currentStudent.has_value();
    addSchoolButton->setEnabled(studentOpen);
    const bool schoolSelected = studentOpen && schools->currentItem();
    deleteSchoolButton->setEnabled(schoolSelected);
    openSchoolButton->setEnabled(schoolSelected);
    const bool schoolOpen = studentOpen && currentSchool.has_value();
    addClassButton->setEnabled(schoolOpen);
    const bool classSelected = schoolOpen && classes->currentItem();
    deleteClassButton->setEnabled(classSelected);
    openClassButton->setEnabled(classSelected);
}

void MainWindow::addStudent()
{
    if (!ready) return;
    bool accepted = false;
    const auto name = QInputDialog::getText(this, "Add Student", "Student name:",
        QLineEdit::Normal, {}, &accepted).trimmed();
    if (!accepted) return;
    if (name.isEmpty() || name.contains(QChar::Null)) {
        QMessageBox::warning(this, "Invalid name", "Enter a non-empty student name.");
        return;
    }
    Student student(name.toUtf8().toStdString(), 0.0);
    if (!database.saveStudent(student)) {
        showDatabaseError("The student could not be saved. Please try again.");
        return;
    }
    refreshStudents(student.getId());
}

void MainWindow::deleteStudent()
{
    if (!ready || !students->currentItem()) return;
    const auto* item = students->currentItem();
    const auto id = item->data(Qt::UserRole).toLongLong();
    QMessageBox confirmation(QMessageBox::Warning, "Delete Student",
        QString("Delete %1?\n\nThis permanently deletes this student's schools, classes, assignments, exams, books, and meetings.").arg(item->text()),
        QMessageBox::Yes | QMessageBox::No, this);
    confirmation.setTextFormat(Qt::PlainText);
    confirmation.setDefaultButton(QMessageBox::No);
    if (confirmation.exec() != QMessageBox::Yes) return;
    if (!database.deleteStudent(id)) {
        showDatabaseError("The student could not be deleted. Refresh by restarting the application and try again.");
        return;
    }
    refreshStudents();
}

void MainWindow::openStudent()
{
    if (!ready || !students->currentItem()) return;
    const auto id = students->currentItem()->data(Qt::UserRole).toLongLong();
    currentStudent = database.loadStudent(id);
    if (!currentStudent) {
        if (database.getLastError().empty())
            QMessageBox::information(this, "Student unavailable", "This student no longer exists. The list will be refreshed.");
        else
            showDatabaseError("Could not open the selected student.");
        refreshStudents();
        return;
    }
    currentSchool.reset();
    populateSchools();
    pages->setCurrentIndex(SchoolsPage);
}

bool MainWindow::reloadCurrentStudent()
{
    if (!ready || !currentStudent) return false;
    auto loaded = database.loadStudent(currentStudent->getId());
    if (!loaded) {
        if (database.getLastError().empty())
            QMessageBox::information(this, "Student unavailable", "This student no longer exists. The student list will be refreshed.");
        else
            showDatabaseError("Could not load this student's schools. Return to the student list and try again.");
        currentClass.reset();
        currentSchool.reset();
        currentStudent.reset();
        schools->clear();
        classes->clear();
        pages->setCurrentIndex(StudentsPage);
        refreshStudents();
        return false;
    }
    currentStudent = std::move(*loaded);
    return true;
}

void MainWindow::populateSchools(std::int64_t selectedId)
{
    schools->clear();
    if (!currentStudent) {
        updateActions();
        return;
    }
    studentHeading->setText("Student: " + QString::fromStdString(currentStudent->getStudentName()));
    const auto loaded = currentStudent->getSchools();
    for (const auto& school : loaded) {
        auto* item = new QListWidgetItem(QString::fromStdString(school.getSchoolName()), schools);
        item->setData(Qt::UserRole, QVariant::fromValue<qlonglong>(school.getId()));
        item->setToolTip(QString("School ID: %1").arg(school.getId()));
        if (school.getId() == selectedId)
            schools->setCurrentItem(item);
    }
    statusBar()->showMessage(loaded.empty() ? "No schools yet. Add a school to begin."
        : QString("%1 school(s)").arg(loaded.size()));
    updateActions();
}

void MainWindow::addSchool()
{
    if (!ready || !currentStudent) return;
    bool accepted = false;
    const auto name = QInputDialog::getText(this, "Add School", "School name:",
        QLineEdit::Normal, {}, &accepted).trimmed();
    if (!accepted) return;
    if (name.isEmpty() || name.contains(QChar::Null)) {
        QMessageBox::warning(this, "Invalid name", "Enter a non-empty school name.");
        return;
    }
    // saveStudent replaces a hierarchy snapshot. Reload before editing so a
    // school removed since opening this page is not saved from an old copy.
    if (!reloadCurrentStudent()) return;
    auto updated = *currentStudent;
    updated.addSchool(School(name.toUtf8().toStdString()));
    if (!database.saveStudent(updated)) {
        showDatabaseError("The school could not be saved. Please try again.");
        return;
    }
    const auto addedId = updated.getSchool(updated.getSchools().size() - 1).getId();
    currentStudent = std::move(updated);
    populateSchools(addedId);
}

void MainWindow::deleteSchool()
{
    if (!ready || !currentStudent || !schools->currentItem()) return;
    const auto* item = schools->currentItem();
    const auto schoolId = item->data(Qt::UserRole).toLongLong();
    const auto studentId = currentStudent->getId();
    QMessageBox confirmation(QMessageBox::Warning, "Delete School",
        QString("Delete %1?\n\nThis permanently deletes this school's classes, assignments, exams, books, and meetings.").arg(item->text()),
        QMessageBox::Yes | QMessageBox::No, this);
    confirmation.setTextFormat(Qt::PlainText);
    confirmation.setDefaultButton(QMessageBox::No);
    if (confirmation.exec() != QMessageBox::Yes) return;
    if (!database.deleteSchool(studentId, schoolId)) {
        showDatabaseError("The school could not be deleted. The list will be refreshed; please try again.");
    }
    // A later snapshot save must use the hierarchy after the direct delete.
    if (reloadCurrentStudent()) populateSchools();
}

void MainWindow::openSchool()
{
    if (!ready || !currentStudent || !schools->currentItem()) return;
    const auto id = schools->currentItem()->data(Qt::UserRole).toLongLong();
    if (!reloadCurrentStudent()) return;
    currentSchool.reset();
    for (const auto& school : currentStudent->getSchools()) {
        if (school.getId() == id) {
            currentSchool = school;
            break;
        }
    }
    if (!currentSchool) {
        QMessageBox::information(this, "School unavailable", "This school no longer exists. The school list will be refreshed.");
        populateSchools();
        return;
    }
    currentClass.reset();
    populateClasses();
    pages->setCurrentIndex(SchoolPage);
}

School* MainWindow::findCurrentSchool(Student& student) const
{
    if (!currentSchool) return nullptr;
    const auto owned = student.getSchools();
    for (std::size_t index = 0; index < owned.size(); ++index)
        if (owned[index].getId() == currentSchool->getId()) return &student.getSchool(index);
    return nullptr;
}

bool MainWindow::reloadCurrentSchool()
{
    if (!currentSchool || !reloadCurrentStudent()) return false;
    auto* school = findCurrentSchool(*currentStudent);
    if (!school) {
        QMessageBox::information(this, "School unavailable", "This school no longer exists. The school list will be refreshed.");
        currentClass.reset();
        currentSchool.reset();
        classes->clear();
        populateSchools();
        pages->setCurrentIndex(SchoolsPage);
        return false;
    }
    currentSchool = *school;
    return true;
}

void MainWindow::populateClasses(std::int64_t selectedId)
{
    classes->clear();
    if (!currentStudent || !currentSchool) {
        updateActions();
        return;
    }
    schoolStudentHeading->setText("Student: " + QString::fromStdString(currentStudent->getStudentName()));
    schoolHeading->setText("School: " + QString::fromStdString(currentSchool->getSchoolName()));
    const auto owned = currentSchool->getStudyClasses();
    for (const auto& course : owned) {
        auto* item = new QListWidgetItem(QString::fromStdString(course.getClassName()), classes);
        item->setData(Qt::UserRole, QVariant::fromValue<qlonglong>(course.getId()));
        item->setToolTip(QString("Class ID: %1").arg(course.getId()));
        if (course.getId() == selectedId) classes->setCurrentItem(item);
    }
    statusBar()->showMessage(owned.empty() ? "No classes yet. Add a class to begin."
        : QString("%1 class(es)").arg(owned.size()));
    updateActions();
}

void MainWindow::addClass()
{
    if (!ready || !currentSchool || !currentStudent) return;
    AddClassDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!reloadCurrentSchool()) return;
    auto updated = *currentStudent;
    auto* school = findCurrentSchool(updated);
    if (!school) return;
    school->addStudyClass(dialog.studyClass());
    if (!database.saveStudent(updated)) {
        showDatabaseError("The class could not be saved. Please try again.");
        return;
    }
    // saveStudent publishes a new snapshot, so reacquire its owned school.
    school = findCurrentSchool(updated);
    const auto id = school->getStudyClasses().back().getId();
    currentSchool = *school;
    currentStudent = std::move(updated);
    populateClasses(id);
}

void MainWindow::deleteClass()
{
    if (!ready || !currentStudent || !currentSchool || !classes->currentItem()) return;
    const auto* item = classes->currentItem();
    const auto id = item->data(Qt::UserRole).toLongLong();
    QMessageBox confirmation(QMessageBox::Warning, "Delete Class",
        QString("Delete %1?\n\nThis permanently deletes this class's assignments, exams, books, and meetings.").arg(item->text()),
        QMessageBox::Yes | QMessageBox::No, this);
    confirmation.setTextFormat(Qt::PlainText);
    confirmation.setDefaultButton(QMessageBox::No);
    if (confirmation.exec() != QMessageBox::Yes || !reloadCurrentSchool()) return;
    auto updated = *currentStudent;
    auto* school = findCurrentSchool(updated);
    if (!school) return;
    const auto owned = school->getStudyClasses();
    bool removed = false;
    for (std::size_t index = 0; index < owned.size(); ++index) {
        if (owned[index].getId() == id) {
            removed = school->removeStudyClass(index);
            break;
        }
    }
    if (!removed) {
        QMessageBox::information(this, "Class unavailable", "This class no longer exists. The class list will be refreshed.");
        populateClasses();
        return;
    }
    if (!database.saveStudent(updated)) {
        showDatabaseError("The class could not be deleted. Please try again.");
        return;
    }
    currentSchool = *findCurrentSchool(updated);
    currentStudent = std::move(updated);
    currentClass.reset();
    populateClasses();
}

void MainWindow::openClass()
{
    if (!ready || !currentStudent || !currentSchool || !classes->currentItem()) return;
    const auto id = classes->currentItem()->data(Qt::UserRole).toLongLong();
    if (!reloadCurrentSchool()) return;
    currentClass.reset();
    for (const auto& course : currentSchool->getStudyClasses()) {
        if (course.getId() == id) {
            currentClass = course;
            break;
        }
    }
    if (!currentClass) {
        QMessageBox::information(this, "Class unavailable", "This class no longer exists. The class list will be refreshed.");
        populateClasses();
        return;
    }
    const auto start = currentClass->getStartDate();
    const auto end = currentClass->getEndDate();
    classDetails->setText(QString("Student: %1\nSchool: %2\nClass: %3\nCredits: %4\nSemester: %5 to %6")
        .arg(QString::fromStdString(currentStudent->getStudentName()),
            QString::fromStdString(currentSchool->getSchoolName()),
            QString::fromStdString(currentClass->getClassName()), QString::number(currentClass->getCredits()),
            dateText(start), dateText(end)));
    pages->setCurrentIndex(ClassPage);
}
