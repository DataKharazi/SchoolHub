#include "School.h"

School::School() = default;

School::School(std::string schoolName)
{
    this->schoolName = schoolName;
}

void School::setSchoolName(std::string schoolName)
{
    this->schoolName = schoolName;
}

std::string School::getSchoolName() const
{
    return schoolName;
}

void School::addStudyClass(StudyClass studyClass)
{
    studyClasses.push_back(studyClass);
}

bool School::removeStudyClass(std::size_t index)
{
    if (index >= studyClasses.size())
        return false;

    studyClasses.erase(studyClasses.begin() + index);
    return true;
}

std::vector<StudyClass> School::getStudyClasses() const
{
    return studyClasses;
}
