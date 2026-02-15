#include "MainMenu.hpp"
#include "Mode1.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/InputMapper.hpp"
#include "Engine/Logger.hpp"
#include "Engine/SettingsManager.hpp"
#include "Engine/Window.hpp"
#include "Engine/TextureManager.hpp"

#include "OpenGL/GL.hpp"



#include <imgui.h>
#include <filesystem> 

void MainMenu::Load()
{
    Engine::GetWindow().Clear(0x000000FF);
    Engine::GetLogger().LogEvent("Main Menu Loaded");

    currentState = MenuState::PressKey;
    introTimer = 0.0;

    SetupBackground();

    SetupStar();
    starTexture = Engine::GetTextureManager().Load("Assets/images/star.png");

    if (starTexture)
    {
        Math::ivec2 winSize = Engine::GetWindow().GetSize();
        Math::ivec2 sSize   = starTexture->GetSize();

        double targetWidth  = 100.0;
        double aspectRatio  = static_cast<double>(sSize.y) / static_cast<double>(sSize.x);
        double targetHeight = targetWidth * aspectRatio;

        starScale = { targetWidth, targetHeight };

        starPos.x = static_cast<double>(winSize.x) * 0.5;
        starPos.y = (static_cast<double>(winSize.y) * 0.45) + 75.0;
    }

    characterTexture = Engine::GetTextureManager().Load("Assets/images/Chr.png");

    if (characterTexture)
    {
        Math::ivec2 winSize = Engine::GetWindow().GetSize();
        Math::ivec2 texSize = characterTexture->GetSize();

        double targetWidth  = 150.0;
        double targetHeight = 250.0;

        characterScale.x = targetWidth / static_cast<double>(texSize.x);
        characterScale.y = targetHeight / static_cast<double>(texSize.y);

        double groundY       = static_cast<double>(winSize.y) * 0.3;
        double screenCenterX = static_cast<double>(winSize.x) * 0.5;

        characterPos.x = screenCenterX - (targetWidth * 0.5);
        characterPos.y = groundY;

        characterPos.y += -90.0;
    }


    CS230::Font& mainFont = Engine::GetFont(2);

    pressKeyTexture = mainFont.PrintToTexture("Press Any Key to Start", CS200::WHITE);

    startTexture    = mainFont.PrintToTexture("Start Game", CS200::WHITE);
    settingsTexture = mainFont.PrintToTexture("Settings", CS200::WHITE);
    exitTexture     = mainFont.PrintToTexture("Exit", CS200::WHITE);

    waitingTexture = mainFont.PrintToTexture("Press Any Key to Start", CS200::WHITE);

    CS230::Font& settingsFont = Engine::GetFont(2); 

    tabDisplayTexture  = settingsFont.PrintToTexture("Display", CS200::WHITE);
    tabControlsTexture = settingsFont.PrintToTexture("Controls", CS200::WHITE);
    backTexture        = settingsFont.PrintToTexture("Back", CS200::WHITE);

    res1600Texture    = settingsFont.PrintToTexture("1600x900", CS200::WHITE);
    res1280Texture    = settingsFont.PrintToTexture("1280x720", CS200::WHITE);
    windowedTexture   = settingsFont.PrintToTexture("Windowed", CS200::WHITE);
    borderlessTexture = settingsFont.PrintToTexture("Borderless", CS200::WHITE);
    fullscreenTexture = settingsFont.PrintToTexture("Fullscreen", CS200::WHITE);
    applyTexture      = settingsFont.PrintToTexture("Apply Settings", CS200::GREEN);
    labelResolution   = settingsFont.PrintToTexture("Resolution", CS200::GREY);
    labelMode         = settingsFont.PrintToTexture("Window Mode", CS200::GREY);

    SetupButtons();

    auto& settings  = CS230::SettingsManager::Instance().GetSettings();
    pendingResIndex = (settings.resolutionX == 1600) ? 0 : 1;
    if (settings.fullscreen)
        pendingMode = WindowMode::Fullscreen;
    else if (settings.borderless)
        pendingMode = WindowMode::Borderless;
    else
        pendingMode = WindowMode::Windowed;
}

void MainMenu::SetupBackground()
{
    // Load the new 'background' shader
    try {
        backgroundShader = OpenGL::CreateShader(
            std::filesystem::path("Assets/shaders/background.vert"), 
            std::filesystem::path("Assets/shaders/background.frag") 
        );
    } catch (const std::exception& e) {
        Engine::GetLogger().LogError(e.what());
        Engine::GetLogger().LogError("Failed to create background shader");
    }
    
    // Define Fullscreen Quad
    float vertices[] = {
        // Pos        // TexCoord
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f
    };

    GL::GenVertexArrays(1, &bgVAO);
    GL::GenBuffers(1, &bgVBO);

    GL::BindVertexArray(bgVAO);
    GL::BindBuffer(GL_ARRAY_BUFFER, bgVBO);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GL::EnableVertexAttribArray(0);
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    GL::EnableVertexAttribArray(1);
    GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainMenu::DrawBackground()
{
    if (backgroundShader.Shader == 0) return;

    GL::UseProgram(backgroundShader.Shader);

    // 1. Standard Uniforms
    if (backgroundShader.UniformLocations.count("u_resolution"))
    {
        Math::ivec2 winSize = Engine::GetWindow().GetSize();
        GL::Uniform2f(backgroundShader.UniformLocations["u_resolution"], 
                      static_cast<float>(winSize.x), static_cast<float>(winSize.y));
    }
    if (backgroundShader.UniformLocations.count("u_time"))
    {
        GL::Uniform1f(backgroundShader.UniformLocations["u_time"], static_cast<float>(accumulatedTime));
    }

    // 2. Star Position (Light Source) - [Updated]
    // Apply the same animation logic here so the light moves with the star
    Math::vec2 animStarPos = starPos;
    animStarPos.y += std::sin(accumulatedTime * 2.0) * 8.0;

    if (backgroundShader.UniformLocations.count("u_starPos"))
    {
        GL::Uniform2f(backgroundShader.UniformLocations["u_starPos"], 
                      static_cast<float>(animStarPos.x), static_cast<float>(animStarPos.y));
    }

    // 3. Character Info (Shadow Blocker)
    if (characterTexture)
    {
        if (backgroundShader.UniformLocations.count("u_charPos"))
        {
            GL::Uniform2f(backgroundShader.UniformLocations["u_charPos"], 
                          static_cast<float>(characterPos.x), static_cast<float>(characterPos.y));
        }

        if (backgroundShader.UniformLocations.count("u_charSize"))
        {
            Math::ivec2 texSize = characterTexture->GetSize();
            float width  = static_cast<float>(texSize.x) * static_cast<float>(characterScale.x);
            float height = static_cast<float>(texSize.y) * static_cast<float>(characterScale.y);

            GL::Uniform2f(backgroundShader.UniformLocations["u_charSize"], width, height);
        }
    }

    GL::BindVertexArray(bgVAO); 
    GL::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GL::BindVertexArray(0);
    GL::UseProgram(0);
}

void MainMenu::SetupButtons()
{
    Math::ivec2 winSize = Engine::GetWindow().GetSize();
    double      centerX = static_cast<double>(winSize.x) * 0.5;
    double      centerY = static_cast<double>(winSize.y) * 0.5;

    double mainStartY = centerY - 50.0;
    double gap        = 70.0; 

    if (startTexture)
    {
        Math::ivec2 size = startTexture->GetSize();
        startButtonRect  = {
            { centerX - size.x / 2.0,          mainStartY },
            { centerX + size.x / 2.0, mainStartY + size.y }
        };
    }
    if (settingsTexture)
    {
        Math::ivec2 size   = settingsTexture->GetSize();
        settingsButtonRect = {
            { centerX - size.x / 2.0,          mainStartY - gap },
            { centerX + size.x / 2.0, mainStartY - gap + size.y }
        };
    }
    if (exitTexture)
    {
        Math::ivec2 size = exitTexture->GetSize();
        exitButtonRect   = {
            { centerX - size.x / 2.0,          mainStartY - gap * 2.0 },
            { centerX + size.x / 2.0, mainStartY - gap * 2.0 + size.y }
        };
    }

    double tabY   = static_cast<double>(winSize.y) - 80.0;
    double tabGap = 200.0;

    if (tabDisplayTexture)
    {
        Math::ivec2 size = tabDisplayTexture->GetSize();
        tabDisplayRect   = {
            { centerX - tabGap / 2.0 - size.x,          tabY },
            {          centerX - tabGap / 2.0, tabY + size.y }
        };
    }
    if (tabControlsTexture)
    {
        Math::ivec2 size = tabControlsTexture->GetSize();
        tabControlsRect  = {
            {          centerX + tabGap / 2.0,          tabY },
            { centerX + tabGap / 2.0 + size.x, tabY + size.y }
        };
    }

    if (backTexture)
    {
        Math::ivec2 size = backTexture->GetSize();
        backRect         = {
            { static_cast<double>(winSize.x) - size.x - 30.0,                               30.0 },
            {          static_cast<double>(winSize.x) - 30.0, 30.0 + static_cast<double>(size.y) }
        };
    }

    double dispLeftX  = centerX - 250.0;
    double dispRightX = centerX + 100.0;
    double dispY      = centerY + 50.0;
    double dispGap    = 60.0;

    if (res1600Texture)
        res1600Rect = {
            {                               dispLeftX,                               dispY },
            { dispLeftX + res1600Texture->GetSize().x, dispY + res1600Texture->GetSize().y }
        };
    if (res1280Texture)
        res1280Rect = {
            {                               dispLeftX,                               dispY - dispGap },
            { dispLeftX + res1280Texture->GetSize().x, dispY - dispGap + res1280Texture->GetSize().y }
        };

    if (windowedTexture)
        windowedRect = {
            {                                dispRightX,                                dispY },
            { dispRightX + windowedTexture->GetSize().x, dispY + windowedTexture->GetSize().y }
        };
    if (borderlessTexture)
        borderlessRect = {
            {                                  dispRightX,                                  dispY - dispGap },
            { dispRightX + borderlessTexture->GetSize().x, dispY - dispGap + borderlessTexture->GetSize().y }
        };
    if (fullscreenTexture)
        fullscreenRect = {
            {                                  dispRightX,                                  dispY - dispGap * 2 },
            { dispRightX + fullscreenTexture->GetSize().x, dispY - dispGap * 2 + fullscreenTexture->GetSize().y }
        };

    if (applyTexture)
    {
        Math::ivec2 size = applyTexture->GetSize();
        applyRect        = {
            { centerX - size.x / 2.0,                               100.0 },
            { centerX + size.x / 2.0, 100.0 + static_cast<double>(size.y) }
        };
    }

    keyBindButtons.clear();
    CS230::Font& listFont = Engine::GetFont(2);
    auto&        mapper   = CS230::InputMapper::Instance();

    double listStartY = tabY - 100.0;
    double rowHeight  = 45.0;

    for (int i = 0; i < static_cast<int>(CS230::GameAction::Count); ++i)
    {
        CS230::GameAction action  = static_cast<CS230::GameAction>(i);
        std::string       name    = mapper.ActionToString(action);
        std::string       keyName = "[" + mapper.GetBindingName(action) + "]";

        KeyBindButton btn;
        btn.action      = action;
        btn.actionName  = name;
        btn.nameTexture = listFont.PrintToTexture(name, CS200::WHITE);
        btn.keyTexture  = listFont.PrintToTexture(keyName, CS200::YELLOW);

        double rowY       = listStartY - (i * rowHeight);
        double leftAlign  = centerX - 300.0;
        double rightAlign = centerX + 300.0;

        btn.rect = {
            {  leftAlign,        rowY },
            { rightAlign, rowY + 35.0 }
        };
        keyBindButtons.push_back(btn);
    }
}

void MainMenu::Update([[maybe_unused]] double dt)
{
    accumulatedTime += dt;

    if (currentState == MenuState::PressKey)
    {
        // 1. Update Intro Timer
        if (introTimer < introDuration)
        {
            introTimer += dt;
        }

        auto& input = Engine::GetInput();
        bool anyKeyPressed = false;

        // Check for any key input
        for (int i = 0; i < static_cast<int>(CS230::Input::Keys::Count); ++i) {
            if (input.KeyJustPressed(static_cast<CS230::Input::Keys>(i))) {
                anyKeyPressed = true;
                break;
            }
        }
        if (input.MouseButtonJustPressed(CS230::Input::MouseButton::Left) || 
            input.MouseButtonJustPressed(CS230::Input::MouseButton::Right)) {
            anyKeyPressed = true;
        }

        if (anyKeyPressed)
        {
            // Case 1: Intro is still playing -> SKIP INTRO
            if (introTimer < introDuration)
            {
                Engine::GetLogger().LogEvent("Intro Skipped");
                introTimer = introDuration + 1.0; // Force timer to end
            }
            // Case 2: Intro finished -> GO TO MAIN MENU
            else
            {
                Engine::GetLogger().LogEvent("Transition to Main Menu");
                currentState = MenuState::Main;
            }
        }
    }
    else if (currentState == MenuState::Main)
    {
        UpdateMainMenu(dt);
    }
    else
    {
        UpdateSettingsMenu(dt);
    }
}

void MainMenu::UpdateMainMenu(double)
{
    auto&       input    = Engine::GetInput();
    Math::ivec2 winSize  = Engine::GetWindow().GetSize();
    Math::vec2  mousePos = input.GetMousePosition();
    mousePos.y           = static_cast<double>(winSize.y) - mousePos.y;

    auto CheckHover = [&](const Math::rect& rect)
    {
        return mousePos.x >= rect.Left() && mousePos.x <= rect.Right() && mousePos.y >= rect.Bottom() && mousePos.y <= rect.Top();
    };

    isStartHovered    = CheckHover(startButtonRect);
    isSettingsHovered = CheckHover(settingsButtonRect);
    isExitHovered     = CheckHover(exitButtonRect);

    if (input.MouseButtonJustPressed(CS230::Input::MouseButton::Left))
    {
        if (isStartHovered)
        {
            Engine::GetLogger().LogEvent("Start Game Button Clicked");
            Engine::GetGameStateManager().Clear();
            Engine::GetGameStateManager().PushState<Mode1>();
        }
        else if (isSettingsHovered)
        {
            Engine::GetLogger().LogEvent("Settings Button Clicked");
            currentState = MenuState::Settings;
            currentTab   = SettingsTab::Display;
            SetupButtons();
        }
        else if (isExitHovered)
        {
            Engine::GetLogger().LogEvent("Exit Button Clicked");
            Engine::Instance().Stop();
        }
    }
}

void MainMenu::UpdateSettingsMenu(double)
{
    auto&       input    = Engine::GetInput();
    Math::ivec2 winSize  = Engine::GetWindow().GetSize();
    Math::vec2  mousePos = input.GetMousePosition();
    mousePos.y           = static_cast<double>(winSize.y) - mousePos.y;

    auto CheckClick = [&](const Math::rect& rect)
    {
        return input.MouseButtonJustPressed(CS230::Input::MouseButton::Left) && mousePos.x >= rect.Left() && mousePos.x <= rect.Right() && mousePos.y >= rect.Bottom() && mousePos.y <= rect.Top();
    };

    if (isWaitingForKey)
    {
        UpdateControlsTab();
        return;
    }

    if (CheckClick(tabDisplayRect))
    {
        Engine::GetLogger().LogEvent("Switched to Display Tab");
        currentTab = SettingsTab::Display;
    }
    if (CheckClick(tabControlsRect))
    {
        Engine::GetLogger().LogEvent("Switched to Controls Tab");
        currentTab = SettingsTab::Controls;
        SetupButtons();
    }

    if (CheckClick(backRect))
    {
        Engine::GetLogger().LogEvent("Back to Main Menu");
        currentState = MenuState::Main;
        CS230::SettingsManager::Instance().SaveSettings();
        CS230::InputMapper::Instance().SaveBindings();
    }

    if (currentTab == SettingsTab::Display)
    {
        UpdateDisplayTab();
    }
    else
    {
        UpdateControlsTab();
    }
}

void MainMenu::UpdateDisplayTab()
{
    auto&       input    = Engine::GetInput();
    Math::ivec2 winSize  = Engine::GetWindow().GetSize();
    Math::vec2  mousePos = input.GetMousePosition();
    mousePos.y           = static_cast<double>(winSize.y) - mousePos.y;

    auto CheckClick = [&](const Math::rect& rect)
    {
        return input.MouseButtonJustPressed(CS230::Input::MouseButton::Left) && mousePos.x >= rect.Left() && mousePos.x <= rect.Right() && mousePos.y >= rect.Bottom() && mousePos.y <= rect.Top();
    };

    if (CheckClick(res1600Rect))
        pendingResIndex = 0;
    if (CheckClick(res1280Rect))
        pendingResIndex = 1;

    if (CheckClick(windowedRect))
        pendingMode = WindowMode::Windowed;
    if (CheckClick(borderlessRect))
        pendingMode = WindowMode::Borderless;
    if (CheckClick(fullscreenRect))
        pendingMode = WindowMode::Fullscreen;

    if (CheckClick(applyRect))
    {
        Engine::GetLogger().LogEvent("Applying Display Settings...");
        auto& settingsMgr = CS230::SettingsManager::Instance();
        settingsMgr.SetResolution(resolutions[pendingResIndex].x, resolutions[pendingResIndex].y);
        settingsMgr.SetWindowMode(pendingMode == WindowMode::Fullscreen, pendingMode == WindowMode::Borderless);
        SetupButtons();
    }
}

void MainMenu::UpdateControlsTab()
{
    auto& input  = Engine::GetInput();
    auto& mapper = CS230::InputMapper::Instance();

    if (isWaitingForKey)
    {
        if (input.KeyJustPressed(CS230::Input::Keys::Escape))
        {
            Engine::GetLogger().LogEvent("Key Binding Cancelled");
            isWaitingForKey = false;
            rebindAction    = CS230::GameAction::Count;
            return;
        }

        for (int i = 0; i < static_cast<int>(CS230::Input::Keys::Count); ++i)
        {
            CS230::Input::Keys key = static_cast<CS230::Input::Keys>(i);
            if (input.KeyJustPressed(key))
            {
                mapper.SetBinding(rebindAction, key);
                Engine::GetLogger().LogEvent("Key Binding Updated: " + mapper.GetBindingName(rebindAction));
                isWaitingForKey = false;
                rebindAction    = CS230::GameAction::Count;
                SetupButtons();
                break;
            }
        }
        return;
    }

    Math::ivec2 winSize  = Engine::GetWindow().GetSize();
    Math::vec2  mousePos = input.GetMousePosition();
    mousePos.y           = static_cast<double>(winSize.y) - mousePos.y;

    if (input.MouseButtonJustPressed(CS230::Input::MouseButton::Left))
    {
        for (const auto& btn : keyBindButtons)
        {
            if (mousePos.x >= btn.rect.Left() && mousePos.x <= btn.rect.Right() && mousePos.y >= btn.rect.Bottom() && mousePos.y <= btn.rect.Top())
            {
                Engine::GetLogger().LogEvent("Rebinding Action: " + btn.actionName);
                isWaitingForKey = true;
                rebindAction    = btn.action;
                break;
            }
        }
    }
}

void MainMenu::Draw()
{
    GL::Disable(GL_DEPTH_TEST);

    DrawBackground();

    auto&       renderer = Engine::GetRenderer2D();
    Math::ivec2 winSize  = Engine::GetWindow().GetSize();

    renderer.BeginScene(CS200::build_ndc_matrix(winSize));

    if (currentState == MenuState::PressKey || currentState == MenuState::Main)
        DrawMainMenu();
    else
        DrawSettingsMenu();

    renderer.EndScene();
}

void MainMenu::DrawMainMenu()
{
    // --- Animation Timing Setup ---
    double charStartTime = 0.5;
    double starStartTime = 2.0;
    double textStartTime = 3.5;
    double fadeDuration  = 1.0;

    auto GetRatio = [&](double start) -> double {
        if (introTimer < start) return 0.0;
        if (introTimer >= start + fadeDuration) return 1.0;
        return (introTimer - start) / fadeDuration;
    };

    double charRatio = GetRatio(charStartTime);
    double starRatio = GetRatio(starStartTime);
    double textRatio = GetRatio(textStartTime);

    if (starRatio > 0.0)
    {
        double idleY = std::sin(accumulatedTime * 2.0) * 8.0;

        double entryY = (1.0 - starRatio) * -30.0;

        Math::vec2 finalStarPos = starPos;
        finalStarPos.y += idleY + entryY;

        // Draw Star Manually (to apply specific position)
        if (starTexture && starShader.Shader != 0)
        {
            GL::Enable(GL_BLEND);
            GL::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL::UseProgram(starShader.Shader);

            GL::ActiveTexture(GL_TEXTURE0);
            GL::BindTexture(GL_TEXTURE_2D, starTexture->GetHandle());

            if (starShader.UniformLocations.count("u_time"))
                GL::Uniform1f(starShader.UniformLocations["u_time"], static_cast<float>(accumulatedTime));
            if (starShader.UniformLocations.count("u_texture"))
                GL::Uniform1i(starShader.UniformLocations["u_texture"], 0);

            Math::TransformationMatrix modelMat = 
                Math::TranslationMatrix(finalStarPos) * Math::ScaleMatrix(starScale);

            Math::ivec2 winSize = Engine::GetWindow().GetSize();
            Math::TransformationMatrix projMat = CS200::build_ndc_matrix(winSize);
            
            // Proj Matrix -> 4x4 Array
            float proj4x4[16] = {
                static_cast<float>(projMat[0][0]), static_cast<float>(projMat[1][0]), 0.0f, static_cast<float>(projMat[2][0]),
                static_cast<float>(projMat[0][1]), static_cast<float>(projMat[1][1]), 0.0f, static_cast<float>(projMat[2][1]),
                0.0f,                              0.0f,                              1.0f, 0.0f,
                static_cast<float>(projMat[0][2]), static_cast<float>(projMat[1][2]), 0.0f, static_cast<float>(projMat[2][2])
            };
            // Model Matrix -> 4x4 Array
            float model4x4[16] = {
                static_cast<float>(modelMat[0][0]), static_cast<float>(modelMat[1][0]), 0.0f, static_cast<float>(modelMat[2][0]),
                static_cast<float>(modelMat[0][1]), static_cast<float>(modelMat[1][1]), 0.0f, static_cast<float>(modelMat[2][1]),
                0.0f,                               0.0f,                               1.0f, 0.0f,
                static_cast<float>(modelMat[0][2]), static_cast<float>(modelMat[1][2]), 0.0f, static_cast<float>(modelMat[2][2])
            };

            if (starShader.UniformLocations.count("u_projection"))
                GL::UniformMatrix4fv(starShader.UniformLocations["u_projection"], 1, GL_FALSE, proj4x4);
            if (starShader.UniformLocations.count("u_transform"))
                GL::UniformMatrix4fv(starShader.UniformLocations["u_transform"], 1, GL_FALSE, model4x4);

            GL::BindVertexArray(starVAO);
            GL::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            GL::BindVertexArray(0);
            GL::UseProgram(0);
        }
    }

    // ---------------------------------------------------------
    // 2. Draw Character & Shadow
    // ---------------------------------------------------------
    if (characterTexture && charRatio > 0.0)
    {
        unsigned int currentBodyAlpha = static_cast<unsigned int>(charRatio * 255.0);
        
        double shadowOpacityFactor = starRatio; 
        
        if (shadowOpacityFactor > 0.0)
        {
            double shadowStretch = 1.5;
            Math::vec2 shadowScale = characterScale;
            shadowScale.y *= -shadowStretch;
            Math::vec2 shadowPos = characterPos;
            double paddingY = 90.0;
            shadowPos.y += (paddingY * (1.0 + shadowStretch));

            unsigned int shadowAlphaVal = static_cast<unsigned int>(180.0 * shadowOpacityFactor);
            unsigned int shadowColor = (shadowAlphaVal); // A=shadowAlphaVal, RGB=0

            characterTexture->Draw(
                Math::TranslationMatrix(shadowPos) * Math::ScaleMatrix(shadowScale),
                shadowColor
            );
        }

        double colorIntensity = 0.0 + (0x40 * starRatio); // 0x00 -> 0x40
        unsigned int tintVal = static_cast<unsigned int>(colorIntensity);
        unsigned int tintColor = (tintVal << 24) | (tintVal << 16) | (tintVal << 8) | currentBodyAlpha;

        characterTexture->Draw(
            Math::TranslationMatrix(characterPos) * Math::ScaleMatrix(characterScale),
            tintColor
        );
    }

    // ---------------------------------------------------------
    // 3. Draw UI
    // ---------------------------------------------------------
    if (currentState == MenuState::PressKey)
    {
        if (pressKeyTexture && textRatio > 0.0)
        {
            Math::ivec2 winSize = Engine::GetWindow().GetSize();
            double x = static_cast<double>(winSize.x) * 0.5 - pressKeyTexture->GetSize().x * 0.5;
            double y = 80.0;

            double pulse = 1.0;
            if (introTimer >= (textStartTime + fadeDuration))
            {
                pulse = (std::sin(accumulatedTime * 3.0) + 1.0) * 0.5;
            }
            
            unsigned int finalAlpha = static_cast<unsigned int>(textRatio * 255.0 * pulse);
            if (introTimer >= (textStartTime + fadeDuration) && finalAlpha < 50) finalAlpha = 50; 

            unsigned int color = (0xFFFFFF00) | finalAlpha;

            pressKeyTexture->Draw(Math::TranslationMatrix(Math::vec2{ x, y }), color);
        }
    }
    else 
    {
        auto DrawBtn = [&](std::shared_ptr<CS230::Texture> tex, Math::rect rect, bool hover)
        {
            if (tex)
                tex->Draw(Math::TranslationMatrix(Math::vec2{ rect.point_1.x, rect.point_1.y }), hover ? CS200::YELLOW : CS200::WHITE);
        };

        DrawBtn(startTexture, startButtonRect, isStartHovered);
        DrawBtn(settingsTexture, settingsButtonRect, isSettingsHovered);
        DrawBtn(exitTexture, exitButtonRect, isExitHovered);
    }
}

void MainMenu::DrawSettingsMenu()
{
    auto DrawTab = [&](std::shared_ptr<CS230::Texture> tex, Math::rect rect, bool isActive)
    {
        if (tex)
            tex->Draw(Math::TranslationMatrix(Math::vec2{ rect.point_1.x, rect.point_1.y }), isActive ? CS200::GREEN : CS200::GREY);
    };

    DrawTab(tabDisplayTexture, tabDisplayRect, currentTab == SettingsTab::Display);
    DrawTab(tabControlsTexture, tabControlsRect, currentTab == SettingsTab::Controls);

    if (backTexture)
        backTexture->Draw(Math::TranslationMatrix(Math::vec2{ backRect.point_1.x, backRect.point_1.y }), CS200::WHITE);

    if (currentTab == SettingsTab::Display)
    {
        DrawDisplayTab();
    }
    else
    {
        DrawControlsTab();
    }
}

void MainMenu::DrawDisplayTab()
{
    auto DrawOption = [&](std::shared_ptr<CS230::Texture> tex, Math::rect rect, bool selected)
    {
        if (tex)
            tex->Draw(Math::TranslationMatrix(Math::vec2{ rect.point_1.x, rect.point_1.y }), selected ? CS200::CYAN : CS200::GREY);
    };

    if (labelResolution)
        labelResolution->Draw(Math::TranslationMatrix(Math::vec2{ res1600Rect.Left(), res1600Rect.Top() + 20.0 }), CS200::WHITE);

    DrawOption(res1600Texture, res1600Rect, pendingResIndex == 0);
    DrawOption(res1280Texture, res1280Rect, pendingResIndex == 1);

    if (labelMode)
        labelMode->Draw(Math::TranslationMatrix(Math::vec2{ windowedRect.Left(), windowedRect.Top() + 20.0 }), CS200::WHITE);

    DrawOption(windowedTexture, windowedRect, pendingMode == WindowMode::Windowed);
    DrawOption(borderlessTexture, borderlessRect, pendingMode == WindowMode::Borderless);
    DrawOption(fullscreenTexture, fullscreenRect, pendingMode == WindowMode::Fullscreen);

    if (applyTexture)
        applyTexture->Draw(Math::TranslationMatrix(Math::vec2{ applyRect.point_1.x, applyRect.point_1.y }), CS200::WHITE);
}

void MainMenu::DrawControlsTab()
{
    for (const auto& btn : keyBindButtons)
    {
        if (btn.nameTexture)
        {
            btn.nameTexture->Draw(Math::TranslationMatrix(Math::vec2{ btn.rect.Left(), btn.rect.Bottom() }), CS200::WHITE);
        }

        if (isWaitingForKey && rebindAction == btn.action)
        {
            if (waitingTexture)
            {
                double x = btn.rect.Right() - waitingTexture->GetSize().x;
                waitingTexture->Draw(Math::TranslationMatrix(Math::vec2{ x, btn.rect.Bottom() }), CS200::RED);
            }
        }
        else
        {
            if (btn.keyTexture)
            {
                double x = btn.rect.Right() - btn.keyTexture->GetSize().x;
                btn.keyTexture->Draw(Math::TranslationMatrix(Math::vec2{ x, btn.rect.Bottom() }), CS200::YELLOW);
            }
        }
    }
}

void MainMenu::DrawImGui()
{
    ImGui::Begin("Main Menu Debug");

    ImGui::Text("Menu State: %s", currentState == MenuState::Main ? "Main" : "Settings");

    if (currentState == MenuState::Settings)
    {
        ImGui::Separator();
        ImGui::Text("Active Tab: %s", currentTab == SettingsTab::Display ? "Display" : "Controls");

        if (currentTab == SettingsTab::Display)
        {
            ImGui::Text("Selected Resolution: %s", pendingResIndex == 0 ? "1600x900" : "1280x720");
            const char* modes[] = { "Windowed", "Borderless", "Fullscreen" };
            ImGui::Text("Selected Mode: %s", modes[static_cast<int>(pendingMode)]);
        }
        else if (currentTab == SettingsTab::Controls)
        {
            if (isWaitingForKey)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "STATUS: WAITING FOR KEY INPUT...");
            }
            else
            {
                ImGui::Text("Status: Idle");
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Mouse Pos (Screen): (%.1f, %.1f)", Engine::GetInput().GetMousePosition().x, Engine::GetInput().GetMousePosition().y);

    static bool showDebugRects = false;
    ImGui::Checkbox("Show UI Rects", &showDebugRects);

    if (showDebugRects)
    {
        auto DrawDebugRect = [](const Math::rect& r, const char* label)
        {
            ImGui::GetForegroundDrawList()->AddRect(
                ImVec2(static_cast<float>(r.Left()), static_cast<float>(Engine::GetWindow().GetSize().y - r.Top())),
                ImVec2(static_cast<float>(r.Right()), static_cast<float>(Engine::GetWindow().GetSize().y - r.Bottom())), IM_COL32(255, 0, 255, 255));
            ImGui::GetForegroundDrawList()->AddText(ImVec2(static_cast<float>(r.Left()), static_cast<float>(Engine::GetWindow().GetSize().y - r.Top() - 15.0)), IM_COL32(255, 0, 255, 255), label);
        };

        if (currentState == MenuState::Main)
        {
            DrawDebugRect(startButtonRect, "Start");
            DrawDebugRect(settingsButtonRect, "Settings");
            DrawDebugRect(exitButtonRect, "Exit");
        }
        else if (currentTab == SettingsTab::Display)
        {
            DrawDebugRect(res1600Rect, "1600x900");
            DrawDebugRect(res1280Rect, "1280x720");
            DrawDebugRect(applyRect, "Apply");
        }
    }

    ImGui::End();
}

void MainMenu::Unload()
{
    Engine::GetLogger().LogEvent("Main Menu Unloaded");

    // Clean up shader and buffers
    OpenGL::DestroyShader(backgroundShader);
    GL::DeleteVertexArrays(1, &bgVAO);
    GL::DeleteBuffers(1, &bgVAO);

    pressKeyTexture.reset();
    startTexture.reset();
    settingsTexture.reset();
    exitTexture.reset();

    tabDisplayTexture.reset();
    tabControlsTexture.reset();
    backTexture.reset();
    waitingTexture.reset();

    res1600Texture.reset();
    res1280Texture.reset();
    windowedTexture.reset();
    borderlessTexture.reset();
    fullscreenTexture.reset();
    applyTexture.reset();

    keyBindButtons.clear();
}

void MainMenu::SetupStar()
{
    // Create Shader
    try {
        starShader = OpenGL::CreateShader(
            std::filesystem::path("Assets/shaders/star_glow.vert"), 
            std::filesystem::path("Assets/shaders/star_glow.frag")
        );
    } catch (const std::exception& e) {
        Engine::GetLogger().LogError(e.what());
        Engine::GetLogger().LogError("Failed to create star shader");
    }

    // Quad Vertices (x, y, u, v)
    float vertices[] = {
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 0.0f
    };

    GL::GenVertexArrays(1, &starVAO);
    GL::GenBuffers(1, &starVBO);

    GL::BindVertexArray(starVAO);
    GL::BindBuffer(GL_ARRAY_BUFFER, starVBO);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GL::EnableVertexAttribArray(0); // Pos
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    GL::EnableVertexAttribArray(1); // UV
    GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainMenu::DrawStar()
{
    if (starShader.Shader == 0 || !starTexture) return;

    GL::Enable(GL_BLEND);
    GL::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GL::UseProgram(starShader.Shader);

    GL::ActiveTexture(GL_TEXTURE0);
    GL::BindTexture(GL_TEXTURE_2D, starTexture->GetHandle());

    if (starShader.UniformLocations.count("u_time"))
        GL::Uniform1f(starShader.UniformLocations["u_time"], static_cast<float>(accumulatedTime));
    
    if (starShader.UniformLocations.count("u_texture"))
        GL::Uniform1i(starShader.UniformLocations["u_texture"], 0);

    Math::ivec2 winSize = Engine::GetWindow().GetSize();
    Math::TransformationMatrix projMat = CS200::build_ndc_matrix(winSize);
    
    // Projection Matrix Setup (Same as before)
    float proj4x4[16] = {
        static_cast<float>(projMat[0][0]), static_cast<float>(projMat[1][0]), 0.0f, static_cast<float>(projMat[2][0]),
        static_cast<float>(projMat[0][1]), static_cast<float>(projMat[1][1]), 0.0f, static_cast<float>(projMat[2][1]),
        0.0f,                              0.0f,                              1.0f, 0.0f,
        static_cast<float>(projMat[0][2]), static_cast<float>(projMat[1][2]), 0.0f, static_cast<float>(projMat[2][2])
    };

    // [New] Calculate Animated Position
    // Move up and down by +/- 8.0 units using Sine wave
    Math::vec2 animStarPos = starPos;
    animStarPos.y += std::sin(accumulatedTime * 2.0) * 8.0; 

    Math::TransformationMatrix modelMat = 
        Math::TranslationMatrix(animStarPos) * Math::ScaleMatrix(starScale);

    float model4x4[16] = {
        static_cast<float>(modelMat[0][0]), static_cast<float>(modelMat[1][0]), 0.0f, static_cast<float>(modelMat[2][0]),
        static_cast<float>(modelMat[0][1]), static_cast<float>(modelMat[1][1]), 0.0f, static_cast<float>(modelMat[2][1]),
        0.0f,                               0.0f,                               1.0f, 0.0f,
        static_cast<float>(modelMat[0][2]), static_cast<float>(modelMat[1][2]), 0.0f, static_cast<float>(modelMat[2][2])
    };

    if (starShader.UniformLocations.count("u_projection"))
        GL::UniformMatrix4fv(starShader.UniformLocations["u_projection"], 1, GL_FALSE, proj4x4);
    
    if (starShader.UniformLocations.count("u_transform"))
        GL::UniformMatrix4fv(starShader.UniformLocations["u_transform"], 1, GL_FALSE, model4x4);

    GL::BindVertexArray(starVAO);
    GL::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    GL::BindVertexArray(0);
    GL::UseProgram(0);
} 

unsigned int MainMenu::GetFadeAlpha(double currentTime, double startTime, double duration)
{
    if (currentTime < startTime) 
        return 0;
    
    if (currentTime >= startTime + duration) 
        return 255;

    double ratio = (currentTime - startTime) / duration;
    return static_cast<unsigned int>(ratio * 255.0);
}