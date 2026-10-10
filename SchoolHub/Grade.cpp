#include "Grade.h"

#include <cmath>
#include <stdexcept>

Grade::Grade(double earnedPoints, double maxPoints)
{
    if (!std::isfinite(earnedPoints) || !std::isfinite(maxPoints)
        || earnedPoints < 0 || maxPoints <= 0)
        throw std::invalid_argument("Earned points must be finite and nonnegative; maximum points must be finite and positive.");

    if (!std::isfinite((earnedPoints / maxPoints) * 100.0))
        throw std::invalid_argument("Grade percentage exceeds the supported numeric range.");

    this->earnedPoints = earnedPoints;
    this->maxPoints = maxPoints;
}

double Grade::getEarnedPoints() const
{
    return earnedPoints;
}

double Grade::getMaxPoints() const
{
    return maxPoints;
}

double Grade::getPercentage() const
{
    return (earnedPoints / maxPoints) * 100.0;
}

GradeLetter Grade::getLetter() const
{
    const double percentage = getPercentage();
    if (percentage >= 90) return GradeLetter::A;
    if (percentage >= 80) return GradeLetter::B;
    if (percentage >= 70) return GradeLetter::C;
    if (percentage >= 60) return GradeLetter::D;
    return GradeLetter::F;
}
