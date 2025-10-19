/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO: Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Font.hpp"

#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "Error.hpp"
#include "Matrix.hpp"
#include "TextureManager.hpp"
#include <algorithm>

namespace CS230
{
    Font::Font(const std::filesystem::path& file_name)
    {
        // 1. GPU 텍스처 로드 (렌더링에 사용)
        fontTexture = Engine::GetTextureManager().Load(file_name);

        // 2. CPU 이미지 로드 (픽셀 스캔에 사용)
        //    Y축 뒤집기(false) : 폰트 파일의 (0,0)은 좌상단이므로 뒤집지 않음
        CS200::Image image(file_name, false);

        const CS200::RGBA* pixels    = image.data();
        const Math::ivec2  imageSize = image.GetSize();
        fontHeight                   = imageSize.y;

        // 3. 폰트 이미지 형식 검사 (첫 픽셀은 흰색 마커)
        if (pixels[0] != CS200::WHITE)
        {
            throw_error_message("Invalid font file format: " + file_name.string() + ". First pixel is not white.");
        }

        // 4. 픽셀 스캔으로 문자 경계 찾기
        char        currentChar = ' ';       // ASCII ' ' (32)부터 시작
        int         charStartX  = 1;         // 0번 픽셀은 마커이므로 1번부터 시작
        CS200::RGBA lastColor   = pixels[1]; // 1번 픽셀의 색상으로 시작

        for (int x = 1; x < imageSize.x; ++x)
        {
            CS200::RGBA currentColor = pixels[x];
            bool        isLastPixel  = (x == imageSize.x - 1);

            // 색상이 변경되거나 마지막 픽셀에 도달하면 문자 경계로 인식
            if (currentColor != lastColor || isLastPixel)
            {
                // 마지막 픽셀이면 너비에 1을 더해 포함시킴
                int charWidth = (isLastPixel ? x - charStartX + 1 : x - charStartX);

                Math::irect glyphRect;
                glyphRect.point_1 = { charStartX, 0 };                      // left, top
                glyphRect.point_2 = { charStartX + charWidth, fontHeight }; // right, bottom

                glyphs[currentChar] = glyphRect;

                // 다음 문자로 이동
                currentChar++;
                charStartX = x;
                lastColor  = currentColor;

                // 'z' 다음 문자는 없음
                if (currentChar > 'z')
                {
                    break;
                }
            }
        }

        if (currentChar <= 'z')
        {
            throw_error_message("Failed to parse font file: " + file_name.string() + ". Not all characters up to 'z' were found.");
        }
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        // 1. 캐시 키 생성 (문자열 + 색상)
        std::stringstream ss;
        ss << text << "_0x" << std::hex << color;
        std::string cacheKey = ss.str();

        // 2. 캐시 정리 수행 (오래된 항목 제거)
        DoCacheCleanup();

        // 3. 캐시에서 텍스처 찾기
        const uint64_t currentFrame = Engine::GetWindowEnvironment().FrameCount;
        auto           it           = textCache.find(cacheKey);
        if (it != textCache.end())
        {
            // 찾으면: 마지막 사용 프레임 갱신 후 반환
            it->second.lastUsedFrame = currentFrame;
            return it->second.texture;
        }

        // 4. 캐시에 없음 (Cache Miss) -> 새로 텍스처 생성

        // 4a. 렌더링할 텍스처의 총 너비와 높이 계산
        int totalWidth = 0;
        int maxHeight  = fontHeight; // 모든 문자는 같은 높이를 가짐
        for (const char c : text)
        {
            // glyphs 맵에 문자가 있는지 확인
            if (glyphs.count(c) > 0)
            {
                totalWidth += static_cast<int>(glyphs.at(c).Size().x);
            }
            // else { /* 정의되지 않은 문자는 무시 */ }
        }

        // 텍스트가 비어있으면 1x1 텍스처라도 생성 (0x0 텍스처 방지)
        if (totalWidth <= 0)
            totalWidth = 1;
        if (maxHeight <= 0)
            maxHeight = 1; // 폰트 높이가 0일 수는 없지만 안전을 위해

        // 4b. 렌더-투-텍스처 모드 시작
        TextureManager::StartRenderTextureMode(totalWidth, maxHeight);

        // 4c. 텍스트를 한 글자씩 텍스처에 그리기
        int   currentX = 0;
        for (const char c : text)
        {
            if (glyphs.count(c) == 0)
                continue; // 맵에 없는 문자(예: \n)는 무시

            const Math::irect& glyph     = glyphs.at(c);
            Math::ivec2        glyphSize = { static_cast<int>(glyph.Size().x), static_cast<int>(glyph.Size().y) };

            // 너비가 0인 문자(예: 스페이스)는 그리지 않고 X 위치만 이동
            if (glyphSize.x > 0)
            {
                // 렌더 타겟의 좌표계는 (0,0)이 좌상단 (StartRenderTextureMode에서 Y축 뒤집힘)
                // (너비/2, 높이/2)가 중심
                double xPos = currentX + glyphSize.x / 2.0;
                double yPos = glyphSize.y / 2.0;

                Math::TransformationMatrix transform =
                    Math::TranslationMatrix(Math::vec2{ xPos, yPos }) * Math::ScaleMatrix(Math::vec2{ static_cast<double>(glyphSize.x), static_cast<double>(glyphSize.y) });

                // 폰트 텍스처에서 해당 문자 영역(glyph.point_1, glyphSize)을
                // 렌더 타겟의 transform 위치에 그립니다.
                fontTexture->Draw(transform, glyph.point_1, glyphSize, color);
            }

            currentX += glyphSize.x;
        }

        // 4d. 렌더-투-텍스처 모드 종료 및 최종 텍스처 받기
        std::shared_ptr<Texture> newTextTexture = TextureManager::EndRenderTextureMode();

        // 5. 새 텍스처를 캐시에 저장
        textCache[cacheKey] = { newTextTexture, currentFrame };

        return newTextTexture;
    }

    void Font::DoCacheCleanup()
    {
        // 60프레임(약 1초) 이상 사용되지 않고,
        // Font 클래스 외부에서 참조(use_count() > 1)하지 않는 텍스처를 캐시에서 제거합니다.
        const uint64_t currentFrame = Engine::GetWindowEnvironment().FrameCount;
        if (currentFrame == 0 || currentFrame % 60 != 0) // 매 프레임 검사하지 않고 60프레임마다 한 번씩 검사 (0프레임 제외)
        {
            return;
        }

        for (auto it = textCache.begin(); it != textCache.end(); /* no increment */)
        {
            bool isOld = (currentFrame - it->second.lastUsedFrame > 60);

            // use_count() == 1 : 캐시(textCache) 자신만 참조하고 있음
            bool isNotReferencedElsewhere = (it->second.texture.use_count() == 1);

            if (isOld && isNotReferencedElsewhere)
            {
                Engine::GetLogger().LogVerbose("Removing old font cache entry: " + it->first);
                it = textCache.erase(it); // 오래되고 참조되지 않는 항목 제거
            }
            else
            {
                ++it;
            }
        }
    }
}
