#pragma once

#include <QDialog>
#include "../SchoolHub/StudyClass.h"

class QLineEdit;
class QDoubleSpinBox;
class QDateEdit;

class AddClassDialog : public QDialog
{
public:
    explicit AddClassDialog(QWidget* parent = nullptr);
    StudyClass studyClass() const;

private:
    QLineEdit* name;
    QDoubleSpinBox* credits;
    QDateEdit* startDate;
    QDateEdit* endDate;
    void validateAndAccept();
};
