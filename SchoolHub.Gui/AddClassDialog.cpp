#include "AddClassDialog.h"

#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <cmath>
#include <limits>

AddClassDialog::AddClassDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Add Class");
    auto* layout = new QVBoxLayout(this);
    auto* form = new QFormLayout;
    name = new QLineEdit(this);
    name->setAccessibleName("Class name");
    credits = new QDoubleSpinBox(this);
    credits->setAccessibleName("Credits");
    credits->setRange(0, std::numeric_limits<double>::max());
    credits->setDecimals(2);
    credits->setValue(0);
    // Keep the wide numeric range from making the form hundreds of digits wide.
    credits->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    credits->setMinimumWidth(120);
    credits->setMaximumWidth(240);
    startDate = new QDateEdit(QDate::currentDate(), this);
    endDate = new QDateEdit(startDate->date(), this);
    for (auto* edit : {startDate, endDate}) {
        edit->setDateRange(QDate(1, 1, 1), QDate(9999, 12, 31));
        edit->setDisplayFormat("yyyy-MM-dd");
        edit->setCalendarPopup(true);
    }
    startDate->setAccessibleName("Semester start");
    endDate->setAccessibleName("Semester end");
    form->addRow("Class name:", name);
    form->addRow("Credits:", credits);
    form->addRow("Semester start:", startDate);
    form->addRow("Semester end:", endDate);
    layout->addLayout(form);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &AddClassDialog::validateAndAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    name->setFocus();
}

void AddClassDialog::validateAndAccept()
{
    if (name->text().trimmed().isEmpty() || name->text().contains(QChar::Null)) {
        QMessageBox::warning(this, "Invalid name", "Enter a non-empty class name.");
        return;
    }
    if (!credits->hasAcceptableInput() || !std::isfinite(credits->value()) || credits->value() < 0) {
        QMessageBox::warning(this, "Invalid credits", "Enter a finite, non-negative number of credits.");
        return;
    }
    if (!startDate->hasAcceptableInput() || !endDate->hasAcceptableInput()
        || !startDate->date().isValid() || !endDate->date().isValid()
        || endDate->date() < startDate->date()) {
        QMessageBox::warning(this, "Invalid semester", "Enter valid semester dates with the end on or after the start.");
        return;
    }
    accept();
}

StudyClass AddClassDialog::studyClass() const
{
    const auto start = startDate->date();
    const auto end = endDate->date();
    return StudyClass(name->text().trimmed().toUtf8().toStdString(), credits->value(),
        Date(start.year(), start.month(), start.day()), Date(end.year(), end.month(), end.day()));
}
