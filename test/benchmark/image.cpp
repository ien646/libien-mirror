#include <ien/image/image.hpp>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_all.hpp>

ien::image get_random_image(size_t w, size_t h)
{
    ien::image result(w, h);
    for(size_t y = 0; y < h; ++y)
    {
        for(size_t x = 0; x < w; ++x)
        {
            uint8_t* ptr = reinterpret_cast<uint8_t*>(result.data(x, y));
            ptr[0] = rand();
            ptr[1] = rand();
            ptr[2] = rand();
            ptr[3] = rand();
        }
    }
    return result;
}

TEST_CASE("ien::image")
{
    const ien::image img_a = get_random_image(1024, 1024);
    const ien::image img_b = get_random_image(1024, 1024);

    BENCHMARK("absolute difference (1024)")
    {
        return img_a.absolute_difference(img_b);
    };
}