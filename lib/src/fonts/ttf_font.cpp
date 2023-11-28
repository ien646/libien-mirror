#include <ien/fonts/ttf_font.hpp>

#include <ien/io_utils.hpp>

#include <vector>

#include <stb_truetype.h>

namespace ien
{
    ttf_font::ttf_font(const std::string& path, float font_height_px, uint32_t atlas_size, uint32_t num_chars)
    {
        std::vector<stbtt_bakedchar> baked_chars;
        baked_chars.resize(num_chars);
        
        const auto ttfdata = ien::read_file_binary(path);
        if(!ttfdata)
        {
            throw std::logic_error("Unable to read ttf font at: " + path);
        }

        _atlas = std::make_unique<ien::image>(atlas_size, atlas_size, image_format::R);
        stbtt_BakeFontBitmap(ttfdata->data(), 0, font_height_px, _atlas->data(), atlas_size, atlas_size, 0, num_chars, baked_chars.data());

        for(const auto& bc : baked_chars)
        {
            baked_char elem;
            elem.x0 = bc.x0;
            elem.x1 = bc.x1;
            elem.y0 = bc.y0;
            elem.y1 = bc.x1;
            elem.xadvance = bc.xadvance;
            elem.xoff = bc.xoff;
            elem.yoff = bc.yoff;
            _char_info.push_back(elem);
        }
    }
}