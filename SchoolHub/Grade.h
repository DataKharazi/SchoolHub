#pragma once

enum class GradeLetter
{
    A, B, C, D, F
};

// A validated points result. Absence of a grade is represented by optional<Grade>.
class Grade
{
private:
    double earnedPoints;
    double maxPoints;

public:
    // Throws invalid_argument for invalid/non-finite points or percentage overflow.
    // Extra credit (earnedPoints > maxPoints) is allowed.
    Grade(double earnedPoints, double maxPoints);

    double getEarnedPoints() const;
    double getMaxPoints() const;
    double getPercentage() const;
    GradeLetter getLetter() const;
};
