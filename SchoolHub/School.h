#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "StudyClass.h"

class School
{
private:
    std::string schoolName;
    std::vector<StudyClass> studyClasses;

public:
    School();
    School(std::string schoolName);

    void setSchoolName(std::string schoolName);
    std::string getSchoolName() const;

    void addStudyClass(StudyClass studyClass);
    // Zero-based index. Invalid indices leave the collection unchanged.
    bool removeStudyClass(std::size_t index);
    std::vector<StudyClass> getStudyClasses() const;
};
