#include "Score.hpp"

void Score::Add(int amount)
{
    score += amount;
}

int Score::Value() const
{
    return score;
}