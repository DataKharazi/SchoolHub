#pragma once
#include <cstdint>

#include <cstddef>
#include <string>
#include <vector>
#include "StudyClass.h"

class School
{
    friend class Database;
    std::int64_t id = 0; // Database identity; copies represent the same saved entity.
private:
    std::string schoolName;
    std::vector<StudyClass> studyClasses;

public:
    std::int64_t getId() const { return id; }
    School();
    School(std::string schoolName);

    void setSchoolName(std::string schoolName);
    std::string getSchoolName() const;

    void addStudyClass(StudyClass studyClass);
    // Zero-based index. Invalid indices leave the collection unchanged.
    bool removeStudyClass(std::size_t index);
    std::vector<StudyClass> getStudyClasses() const;
    StudyClass& getStudyClass(std::size_t index) { return studyClasses.at(index); }
    const StudyClass& getStudyClass(std::size_t index) const { return studyClasses.at(index); }
};
