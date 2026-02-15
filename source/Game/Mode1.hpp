#pragma once
#include "Engine/Camera.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Rect.hpp"
#include "Engine/Texture.hpp" 

#include "MiniMap.hpp"

#include "OpenGL/Shader.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/VertexArray.hpp"

#include <memory>            

class Player;
class WorldTextManager;
class Star;
class YellowLaser;
class TargetStar;

namespace CS230
{
    class MapManager;
}

class Mode1 : public CS230::GameState
{
public:
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "Mode1";
    }

private:
    void InitGame();

    enum class State
    {
        Loading,
        Playing
    };

    State currentState = State::Loading;

    CS230::Camera* camera;
    Player* player;
    CS230::MapManager* mapManager;
    WorldTextManager* worldTextManager;
    Star* shooterStar;
    Star* targetStar;
    YellowLaser* yellowLaser;
    MiniMap* miniMap;

    // Background Shader Members
    OpenGL::CompiledShader backgroundShader;
    OpenGL::VertexArrayHandle backgroundVAO;
    OpenGL::BufferHandle backgroundVBO;
    double shaderTime = 0.0;

    // [Removed] textureLayer2_Trees definition
    std::shared_ptr<CS230::Texture> textureLayer3_Silhouette;

    std::vector<TargetStar*> targetStars;
    
    Math::rect level1_boundary = {
        {  -500.f,  2000.f },
        { 10000.f, -2000.f }
    };
};