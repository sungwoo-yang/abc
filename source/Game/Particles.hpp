#pragma once
#include "Engine/Particle.hpp"

namespace Particles {
    class Smoke : public CS230::Particle {
    public:
        Smoke() : Particle("Assets/images/Smoke.spt") {};
        std::string TypeName() override { return "Smoke Particle"; }
        static constexpr int MaxCount = 3;
        static constexpr double MaxLife = 5.0;
    };

    class Hit : public CS230::Particle {
    public:
        Hit() : Particle("Assets/images/Hit.spt") {};
        std::string TypeName() override { return "Hit Particle"; }
        static constexpr int MaxCount = 10;
        static constexpr double MaxLife = 1.0;
    };

    class MeteorBit : public CS230::Particle {
    public:
        MeteorBit() : Particle("Assets/images/MeteorBit.spt") {};
        std::string TypeName() override { return "MeteorBit Particle"; }
        static constexpr int MaxCount = 150;
        static constexpr double MaxLife = 1.25;
    };
}
