/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
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
#include "Engine/Path.hpp" // For assets::locate_asset
#include "CS200/IRenderer2D.hpp" // For renderer access
#include <algorithm>
#include <sstream> // For cache key generation

namespace CS230
{
    // 정적 멤버 변수 정의 (텍스트 캐시 및 타임스탬프)
    // static std::unordered_map<std::string, std::shared_ptr<Texture>> text_cache;
    // static std::unordered_map<std::string, uint64_t>                 cache_timestamps;
    // static uint64_t                                                 last_cleanup_frame = 0; // 프레임 기반 캐시 정리용

    namespace // 익명 네임스페이스로 캐시 관련 데이터 숨김
    {
        struct CachedText
        {
            std::shared_ptr<Texture> texture;
            uint64_t                 last_frame_used = 0;
        };

        static std::unordered_map<std::string, CachedText> text_cache;
        static uint64_t                                    last_cleanup_frame = 0;
        static constexpr uint64_t                          CACHE_EXPIRY_FRAMES = 60; // 60프레임 동안 사용되지 않으면 정리
    }

    Font::Font(const std::filesystem::path& file_name) : original_image(assets::locate_asset(file_name), false) // 이미지 로드, 수직 뒤집기 안함
    {
        // 폰트 파일 유효성 검사
        if (original_image.GetSize().x == 0 || original_image.GetSize().y == 0 || GetPixel({0, 0}) != CS200::WHITE)
        {
            throw_error_message("Invalid font file format or failed to load: " + file_name.string());
        }

        // TextureManager를 통해 텍스처 로드
        texture_ptr = Engine::GetTextureManager().Load(file_name);
        if (!texture_ptr)
        {
             throw_error_message("Failed to load font texture via TextureManager: " + file_name.string());
        }

        // 문자 영역 찾기
        FindCharRects();
    }

     // 이미지에서 특정 픽셀 색상 가져오기
    CS200::RGBA Font::GetPixel(Math::ivec2 texel) const
    {
        const Math::ivec2 size = original_image.GetSize();
        if (texel.x < 0 || texel.x >= size.x || texel.y < 0 || texel.y >= size.y)
        {
            // 범위를 벗어나면 투명 반환 (또는 오류 처리)
            return CS200::CLEAR;
        }
        // 이미지 데이터는 row-major 순서
        return original_image.data()[texel.y * size.x + texel.x];
    }

    // 폰트 이미지 스캔하여 각 문자 영역 찾기
    void Font::FindCharRects()
    {
        const Math::ivec2 size = original_image.GetSize();
        if (size.x <= 1) return; // 유효하지 않은 폰트 이미지

        int         last_x_pos   = 1; // 첫 픽셀(흰색 마커) 다음부터 시작
        CS200::RGBA last_color   = GetPixel({0, 0}); // 첫 픽셀 색상 (흰색이어야 함)
        char        current_char = ' '; // ASCII ' ' 부터 시작

        for (int x = 1; x < size.x; ++x)
        {
             CS200::RGBA current_color = GetPixel({x, 0}); // 맨 윗줄 스캔
             if (current_color != last_color) // 색상 변경 감지
             {
                 if (current_char <= 'z') // 지원 범위 내 문자만 저장
                 {
                    // 문자 영역 저장 (x 시작점, y=0부터 이미지 높이까지)
                    char_rects[current_char - ' '] = Math::irect{ {last_x_pos, 0}, {x, size.y} };
                 }
                 last_x_pos = x;
                 last_color = current_color;
                 current_char++;
                 if (current_char > 'z') break; // 'z' 이후 문자는 무시
             }
        }
        // 마지막 문자 영역 저장
        if (current_char <= 'z')
        {
             char_rects[current_char - ' '] = Math::irect{ {last_x_pos, 0}, {size.x, size.y} };
        }

        // 필요한 모든 문자를 찾았는지 확인 (선택적)
        if (current_char < 'z') {
             Engine::GetLogger().LogVerbose("Warning: Font image might not contain all characters up to 'z'. Last char found: " + std::string(1, current_char));
        }
    }


    // 특정 문자의 영역 정보 가져오기 (없으면 공백 문자로 대체)
    Math::irect& Font::GetCharRect(char c)
    {
        if (c >= ' ' && c <= 'z')
        {
            return char_rects[c - ' '];
        }
        // 지원하지 않는 문자는 공백 문자로 대체
        return char_rects[0]; // ' ' 에 해당
    }

    // 주어진 텍스트의 픽셀 너비와 높이 측정
    Math::ivec2 Font::MeasureText(const std::string& text)
    {
        int total_width = 0;
        int max_height  = 0;
        for (const char c : text)
        {
            const Math::irect& rect = GetCharRect(c); // const 버전 GetCharRect 필요 (아래 추가)
            Math::ivec2 size = { static_cast<int>(rect.Size().x), static_cast<int>(rect.Size().y) };
            total_width += size.x;
            if (size.y > max_height)
            {
                max_height = size.y;
            }
        }
        return { total_width, max_height };
    }

    // 캐시 정리 함수
    void Font::CleanupCache()
    {
        const uint64_t current_frame = Engine::GetWindowEnvironment().FrameCount;

        // 일정 프레임 간격으로만 실행
        if (current_frame < last_cleanup_frame + CACHE_CLEANUP_INTERVAL) {
            return;
        }
        last_cleanup_frame = current_frame;

        int cleaned_count = 0;
        for (auto it = text_cache.begin(); it != text_cache.end(); /* no increment here */)
        {
             // 참조 카운트가 1 (캐시만 참조) 이고, 마지막 사용 후 일정 프레임이 지났는지 확인
            if (it->second.use_count() == 1 && (current_frame - cache_timestamps[it->first]) > CACHE_EXPIRY_FRAMES)
            {
                 // Engine::GetLogger().LogDebug("Cleaning cached text: " + it->first);
                 cache_timestamps.erase(it->first); // 타임스탬프도 제거
                 it = text_cache.erase(it);        // 캐시에서 제거 (shared_ptr 소멸자 호출 -> Texture 소멸)
                 cleaned_count++;
            }
            else
            {
                 ++it;
            }
        }
        // if (cleaned_count > 0) {
        //      Engine::GetLogger().LogDebug("Cleaned up " + std::to_string(cleaned_count) + " cached text textures.");
        // }
    }


    // 텍스트를 텍스처로 렌더링 (캐싱 사용)
    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        // 주기적으로 캐시 정리
        CleanupCache();

        // 캐시 키 생성 (텍스트 내용 + 색상)
        std::stringstream ss;
        ss << text << "_" << std::hex << color;
        const std::string cache_key = ss.str();

        // 캐시에 있는지 확인
        auto cache_it = text_cache.find(cache_key);
        if (cache_it != text_cache.end())
        {
            // 캐시에 있으면 마지막 사용 프레임 업데이트 후 반환
            cache_timestamps[cache_key] = Engine::GetWindowEnvironment().FrameCount;
            return cache_it->second;
        }

        // 텍스트 크기 측정
        const Math::ivec2 text_size = MeasureText(text);
        if (text_size.x <= 0 || text_size.y <= 0)
        {
            return nullptr; // 빈 텍스트는 렌더링하지 않음
        }

        // 텍스처 렌더링 모드 시작
        TextureManager::StartRenderTextureMode(text_size.x, text_size.y);

        // 각 문자를 텍스처에 그리기
        int current_x = 0;
        for (const char c : text)
        {
            const Math::irect& rect      = GetCharRect(c);
            const Math::ivec2  char_size = { static_cast<int>(rect.Size().x), static_cast<int>(rect.Size().y) };

            if (char_size.x > 0 && char_size.y > 0)
            {
                // 문자의 텍셀 위치 (rect의 좌상단 기준)
                // Image는 top-left 기준, Texture::Draw는 bottom-left UV 기준이므로 변환 필요 없음
                // Texture::Draw 내부에서 texel_position (top-left)을 bottom-left UV로 변환함
                 const Math::ivec2 texel_pos = { rect.point_1.x, rect.point_1.y };

                 // 문자를 그릴 위치 (텍스처 좌하단 기준)
                 Math::TransformationMatrix transform = Math::TranslationMatrix(Math::vec2{ static_cast<double>(current_x), 0.0 });

                 // 원본 폰트 텍스처에서 해당 문자 부분을 그림
                 texture_ptr->Draw(transform, texel_pos, char_size, color); // Draw 함수에 color 전달

                 current_x += char_size.x;
            }
        }

        // 텍스처 렌더링 모드 종료 및 생성된 텍스처 얻기
        auto new_texture = TextureManager::EndRenderTextureMode();

        // 캐시에 새로 생성된 텍스처 저장 및 타임스탬프 기록
        if (new_texture)
        {
             text_cache[cache_key] = new_texture;
             cache_timestamps[cache_key] = Engine::GetWindowEnvironment().FrameCount;
        }

        return new_texture;
    }

} // namespace CS230