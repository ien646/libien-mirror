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
            ttf_font_char_info elem;
            elem.position.x0 = bc.x0;
            elem.position.x1 = bc.x1;
            elem.position.y0 = bc.y0;
            elem.position.y1 = bc.x1;
            elem.x_advance = bc.xadvance;
            elem.offset.x = bc.xoff;
            elem.offset.y = bc.yoff;
            _char_info.push_back(elem);
        }
    }

    const ttf_font_char_info& ttf_font::get_char_info(uint32_t ch) const
    {
        if(_char_info.size() >= ch)
        {
            throw std::logic_error("Character not present");
        }
        return _char_info.at(ch);
    }
}