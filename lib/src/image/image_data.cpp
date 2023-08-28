#include <ien/image/image_data.hpp>
#include <ien/image/image_format.hpp>

#include <ien/alloc.hpp>
#include <ien/platform.hpp>

#include <cassert>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <utility>

#ifdef IEN_ARCH_X86_64
    #include <emmintrin.h>
#endif


#define IEN_IMAGE_DATA_ALLOC_ALIGNMENT 32 //avx

namespace ien
{
    image_data::image_data(size_t w, size_t h, image_format fmt)
        : _width(w)
        , _height(h)
        , _format(fmt)
    {
        assert(w > 0 && h > 0);
        size_t alloc_sz = w * h * channel_count();
        _data = (uint8_t*)ien::aligned_alloc(alloc_sz, IEN_IMAGE_DATA_ALLOC_ALIGNMENT);
        if (_data == nullptr)
        {
            throw std::bad_alloc();
        }
    }

    image_data::image_data(image_data&& mvsrc)
    {
        *this = std::move(mvsrc);
    }

    image_data::~image_data()
    {
        if (_data != nullptr)
        {
            ien::aligned_free(_data);
        }
        _data = nullptr;
        _width = 0;
        _height = 0;
    }

    image_data& image_data::operator=(image_data&& mvsrc)
    {
        _width = mvsrc._width;
        _height = mvsrc._height;
        _data = mvsrc._data;
        _format = mvsrc._format;

        mvsrc._data = nullptr;
        return *this;
    }

    image_data image_data::extract_channel(size_t channel_index) const
    {
        assert(channel_index <= 4);
        if (channel_index >= channel_count())
        {
            throw std::out_of_range("Invalid image channel index");
        }

        image_data result(_width, _height, image_format::R);
        const size_t channels = channel_count();
        const size_t pixels = pixel_count();
        for (size_t i = 0, j = channel_index; i < pixels; ++i, j += channels)
        {
            result._data[i] = _data[j];
        }
        return result;
    }

    void image_data::cast_format(image_format target_format)
    {
        if (channel_count() != image_format_channels(target_format))
        {
            throw std::logic_error("Unable to cast image format with different number of channels");
        }

        if (target_format == _format || channel_count() < 3)
        {
            // Nothing to do here
            return;
        }

        if (channel_count() == 3)
        {
            shuffle({
                {2, 1, 0}
            }); // bgr <-> rgb
        }
        else if (channel_count() == 4)
        {
            if (_format == image_format::RGBA)
            {
                if (target_format == image_format::ABGR)
                {
                    shuffle({
                        {3, 2, 1, 0}
                    });
                }
                else if (target_format == image_format::BGRA)
                {
                    shuffle({
                        {2, 1, 0, 3}
                    });
                }
            }
            else if (_format == image_format::ABGR)
            {
                if (target_format == image_format::RGBA)
                {
                    shuffle({
                        {3, 2, 1, 0}
                    });
                }
                else if (target_format == image_format::BGRA)
                {
                    shuffle({
                        {1, 2, 3, 0}
                    });
                }
            }
            else if (_format == image_format::BGRA)
            {
                if (target_format == image_format::RGBA)
                {
                    shuffle({
                        {2, 1, 0, 3}
                    });
                }
                else if (target_format == image_format::ABGR)
                {
                    shuffle({
                        {1, 2, 3, 0}
                    });
                }
            }
        }
    }

    void image_data::shuffle(const image_shuffle& op)
    {
        const size_t pixels = pixel_count();
        const size_t channels = channel_count();
        if (channels == 1)
        {
            return;
        }

        const auto shuffle_index_std = [&](size_t pixel_index) {
            uint8_t* pxptr = _data + (pixel_index * channels);
            std::array<uint8_t, 4> temp;
            for (size_t i = 0; i < channels; ++i)
            {
                temp[i] = pxptr[op.indices[i]];
            }

            for (size_t i = 0; i < channels; ++i)
            {
                pxptr[i] = temp[i];
            }
        };

        for (size_t i = 0; i < pixels; ++i)
        {
            shuffle_index_std(i);
        }
    }

    float image_data::absolute_difference(const image_data& other) const
    {
        if (this->width() != other.width() || this->height() != other.height())
        {
            throw std::logic_error("Image dimensions do not match");
        }

        if (this->format() != other.format())
        {
            throw std::logic_error("Image formats do not match");
        }

        if (this->format() != image_format::RGBA)
        {
            throw std::logic_error("absolute_difference(...) is only supported for RGBA formats, use cast_format(...)");
        }

        float accum = 0;

#ifdef IEN_ARCH_X86_64 // guaranteed sse2 on x64, measured about 2~3 times faster on 1024x1024 images
        const uint8_t* px_a0 = data();
        const uint8_t* px_b0 = other.data();

        const __m128 vmul_div255 = _mm_set1_ps(1.0F / 255);
        const __m128 vand_fpsign = _mm_set1_ps(std::bit_cast<float>(~0x80000000));
        const __m128 vmul_div3 = _mm_set1_ps(1.0F / 3);

        for (size_t i = 0; i < (pixel_count() - (pixel_count() % 4)); i += 4)
        {
            const uint8_t* px_a = px_a0 + (i * 4);
            const uint8_t* px_b = px_b0 + (i * 4);

            __m128i va_r_u32 = _mm_set_epi8(0, 0, 0, px_a[0], 0, 0, 0, px_a[4], 0, 0, 0, px_a[8], 0, 0, 0, px_a[12]);
            __m128i va_g_u32 = _mm_set_epi8(0, 0, 0, px_a[1], 0, 0, 0, px_a[5], 0, 0, 0, px_a[9], 0, 0, 0, px_a[13]);
            __m128i va_b_u32 = _mm_set_epi8(0, 0, 0, px_a[2], 0, 0, 0, px_a[6], 0, 0, 0, px_a[10], 0, 0, 0, px_a[14]);

            __m128i vb_r_u32 = _mm_set_epi8(0, 0, 0, px_b[0], 0, 0, 0, px_b[4], 0, 0, 0, px_b[8], 0, 0, 0, px_b[12]);
            __m128i vb_g_u32 = _mm_set_epi8(0, 0, 0, px_b[1], 0, 0, 0, px_b[5], 0, 0, 0, px_b[9], 0, 0, 0, px_b[13]);
            __m128i vb_b_u32 = _mm_set_epi8(0, 0, 0, px_b[2], 0, 0, 0, px_b[6], 0, 0, 0, px_b[10], 0, 0, 0, px_b[14]);

            __m128 va_r_f32 = _mm_cvtepi32_ps(va_r_u32);
            __m128 va_g_f32 = _mm_cvtepi32_ps(va_g_u32);
            __m128 va_b_f32 = _mm_cvtepi32_ps(va_b_u32);

            __m128 vb_r_f32 = _mm_cvtepi32_ps(vb_r_u32);
            __m128 vb_g_f32 = _mm_cvtepi32_ps(vb_g_u32);
            __m128 vb_b_f32 = _mm_cvtepi32_ps(vb_b_u32);

            va_r_f32 = _mm_mul_ps(va_r_f32, vmul_div255);
            va_g_f32 = _mm_mul_ps(va_g_f32, vmul_div255);
            va_b_f32 = _mm_mul_ps(va_b_f32, vmul_div255);

            vb_r_f32 = _mm_mul_ps(vb_r_f32, vmul_div255);
            vb_g_f32 = _mm_mul_ps(vb_g_f32, vmul_div255);
            vb_b_f32 = _mm_mul_ps(vb_b_f32, vmul_div255);

            __m128 vdiff_r_f32 = _mm_sub_ps(va_r_f32, vb_r_f32);
            __m128 vdiff_g_f32 = _mm_sub_ps(va_g_f32, vb_g_f32);
            __m128 vdiff_b_f32 = _mm_sub_ps(va_b_f32, vb_b_f32);

            vdiff_r_f32 = _mm_and_ps(vdiff_r_f32, vand_fpsign);
            vdiff_g_f32 = _mm_and_ps(vdiff_g_f32, vand_fpsign);
            vdiff_b_f32 = _mm_and_ps(vdiff_b_f32, vand_fpsign);

            __m128 vdiff_rgb_f32 = _mm_add_ps(_mm_add_ps(vdiff_r_f32, vdiff_g_f32), vdiff_b_f32);
            __m128 vdiff_avg_f32 = _mm_mul_ps(vdiff_rgb_f32, vmul_div3);

            __m128 vdiff_avg_f32_01 = _mm_add_ps(
                vdiff_avg_f32,
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(1, 1, 1, 1))
            );

            __m128 vdiff_avg_f32_23 = _mm_add_ps(
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(2, 2, 2, 2)),
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(3, 3, 3, 3))
            );

            __m128 vdiff_avg_f32_0123 = _mm_add_ps(vdiff_avg_f32_01, vdiff_avg_f32_23);

            float v = _mm_cvtss_f32(vdiff_avg_f32_0123);
            accum += v;
        }

        // Non vectorizable remainder
        for (size_t i = (pixel_count() - (pixel_count() % 4)); i < pixel_count(); ++i)
        {
            const uint8_t* px_a = data() + (i * 4);
            const uint8_t* px_b = other.data() + (i * 4);
            float ra = (float)px_a[0] / 255;
            float ga = (float)px_a[1] / 255;
            float ba = (float)px_a[2] / 255;

            float rb = (float)px_b[0] / 255;
            float gb = (float)px_b[1] / 255;
            float bb = (float)px_b[2] / 255;

            float dr = std::abs(ra - rb);
            float dg = std::abs(ga - gb);
            float db = std::abs(ba - bb);

            float davg = (dr + dg + db) / 3;
            accum += davg;
        }
#else
        for (size_t i = 0; i < img_a.pixel_count(); ++i)
        {
            const uint8_t* px_a = img_a.data() + (i * 4);
            const uint8_t* px_b = img_b.data() + (i * 4);
            float ra = (float)px_a[0] / 255;
            float ga = (float)px_a[1] / 255;
            float ba = (float)px_a[2] / 255;

            float rb = (float)px_b[0] / 255;
            float gb = (float)px_b[1] / 255;
            float bb = (float)px_b[2] / 255;

            float dr = std::abs(ra - rb);
            float dg = std::abs(ga - gb);
            float db = std::abs(ba - bb);

            float davg = (dr + dg + db) / 3;
            accum += davg;
        }
#endif
        return accum / pixel_count();
    }

} // namespace ien