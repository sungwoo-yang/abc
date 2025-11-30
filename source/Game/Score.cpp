#include "score.h"

void Score::Add(int amount) {
    score += amount;
}

int Score::Value() const {
    return score;
}