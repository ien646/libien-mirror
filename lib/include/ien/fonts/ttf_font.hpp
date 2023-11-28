#pragma once

#include <ien/image/image.hpp>

#include <memory>
#include <vector>

namespace ien
{
    struct ttf_font_char_info
    {
        struct
        {
            unsigned short x0, y0, x1, y1;
        } position;

        struct
        {
            float x, y;
        } offset;
        
        float x_advance;
    };
    class ttf_font
    {
    private:
        std::unique_ptr<image> _atlas;
        std::vector<ttf_font_char_info> _char_info;

    public:
        ttf_font(const std::string& path, float font_height_px, uint32_t atlas_size = 512, uint32_t num_chars = 256);

        inline const image& get_atlas() const { return *_atlas; }
        const ttf_font_char_info& get_char_info(uint32_t ch) const;
    };
} // namespace ien