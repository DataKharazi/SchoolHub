#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ConsoleApp.h"
#include "Student.h"

namespace
{
    // False means input has ended; every menu then returns to its caller.
    bool readChoice(std::size_t minimum, std::size_t maximum, std::size_t& choice)
    {
        std::string line;
        while (true)
        {
            std::cout << "Choice: ";
            if (!std::getline(std::cin, line))
                return false;

            std::istringstream input(line);
            long long number;
            char extra;
            if (input >> number && !(input >> extra) && number >= 0
                && static_cast<unsigned long long>(number) >= minimum
                && static_cast<unsigned long long>(number) <= maximum)
            {
                choice = static_cast<std::size_t>(number);
                return true;
            }
            std::cout << "Invalid choice. Enter a number from "
                << minimum << " to " << maximum << ".\n";
        }
    }

    // Trim names; blank input cancels without creating an empty entry.
    bool readName(const char* prompt, std::string& name)
    {
        std::cout << prompt << " (blank to cancel): ";
        if (!std::getline(std::cin, name))
            return false;
        const auto first = name.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            std::cout << "Cancelled.\n";
            return false;
        }
        name = name.substr(first, name.find_last_not_of(" \t\r\n") - first + 1);
        return true;
    }

    void showSchoolDashboard(const School& school)
    {
        while (std::cin)
        {
            std::cout << "\nSchool: " << school.getSchoolName()
                << "\n1. Classes\n2. Upcoming events\n3. Schedule\n4. Open class\n5. Back\n";
            std::size_t choice;
            if (!readChoice(1, 5, choice) || choice == 5)
                return;
            std::cout << "Not implemented yet.\n";
        }
    }

    void selectSchool(const Student& student)
    {
        const auto schools = student.getSchools();
        if (schools.empty())
        {
            std::cout << "No schools yet. Add a school first.\n";
            return;
        }
        std::cout << "\nSelect school:\n";
        for (std::size_t i = 0; i < schools.size(); ++i)
            std::cout << i + 1 << ". " << schools[i].getSchoolName() << '\n';
        std::cout << "0. Back\n";
        std::size_t choice;
        if (readChoice(0, schools.size(), choice) && choice != 0)
            showSchoolDashboard(schools[choice - 1]);
    }

    void showStudentMenu(Student& student)
    {
        while (std::cin)
        {
            std::cout << "\nStudent: " << student.getStudentName()
                << "\n1. Select school\n2. Add school\n3. Back\n";
            std::size_t choice;
            if (!readChoice(1, 3, choice) || choice == 3)
                return;
            if (choice == 1)
                selectSchool(student);
            else
            {
                std::string name;
                if (readName("School name", name))
                {
                    student.addSchool(School(name));
                    std::cout << "School added.\n";
                }
            }
        }
    }

    void selectStudent(std::vector<Student>& students)
    {
        if (students.empty())
        {
            std::cout << "No students yet. Add a student first.\n";
            return;
        }
        std::cout << "\nSelect student:\n";
        for (std::size_t i = 0; i < students.size(); ++i)
            std::cout << i + 1 << ". " << students[i].getStudentName() << '\n';
        std::cout << "0. Back\n";
        std::size_t choice;
        if (readChoice(0, students.size(), choice) && choice != 0)
            showStudentMenu(students[choice - 1]);
    }
}

void runConsoleApp()
{
    std::vector<Student> students;
    std::cout << "Students and schools are kept in memory for this session only.\n";
    while (std::cin)
    {
        std::cout << "\nSCHOOLHUB\n1. Select student\n2. Add student\n3. Exit\n";
        std::size_t choice;
        if (!readChoice(1, 3, choice) || choice == 3)
            break;
        if (choice == 1)
            selectStudent(students);
        else
        {
            std::string name;
            if (readName("Student name", name))
            {
                students.push_back(Student(name, 0.0));
                std::cout << "Student added. GPA starts at 0 for now.\n";
            }
        }
    }
    std::cout << "Goodbye.\n";
}
