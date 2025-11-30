#pragma once
#include "Engine/GameObjectTypes.hpp"
#include "Engine/GameState.hpp"

class Score : public CS230::Component
{
public:
    void Add(int amount);
    int  Value() const;

    void Reset()
    {
        score   = 0;
        updated = true;
    }

    bool IsUpdated() const
    {
        return updated;
    }

    void ClearUpdateFlag()
    {
        updated = false;
    }

private:
    int  score   = 0;
    bool updated = false;
};