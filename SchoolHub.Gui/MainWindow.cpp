#include "MainWindow.h"

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
    studentLayout->addWidget(new QLabel("Schools screen coming next.", studentPage));
    studentLayout->addStretch();
    auto* back = new QPushButton("Back to Students", studentPage);
    studentLayout->addWidget(back);
    pages->addWidget(studentPage);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteStudent);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openStudent);
    connect(students, &QListWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(students, &QListWidget::itemDoubleClicked, this, [this] { openStudent(); });
    connect(back, &QPushButton::clicked, this, [this] {
        const auto id = currentStudent ? currentStudent->getId() : 0;
        currentStudent.reset();
        pages->setCurrentIndex(0);
        refreshStudents(id);
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
    studentHeading->setText("Student: " + QString::fromStdString(currentStudent->getStudentName()));
    pages->setCurrentIndex(1);
}
