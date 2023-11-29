#pragma once

#include <ien/image/image.hpp>

#include <memory>
#include <vector>

namespace ien
{
    class ttf_font
    {
    private:
        struct baked_char
        {
            unsigned short x0, y0, x1, y1;
            float xoff, yoff, xadvance;
        };
        std::unique_ptr<image> _atlas;
        std::vector<baked_char> _char_info;

    public:
        ttf_font(const std::string& path, float font_height_px, uint32_t atlas_size = 512, uint32_t num_chars = 256);

        inline const image& get_atlas() const { return *_atlas; }
    };
} // namespace ien