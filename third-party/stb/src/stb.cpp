#ifdef _MSC_VER
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("warning(push, 0)")
    #define IEN_WARNINGS_DISABLE_END _Pragma("warning(pop)")
#else
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wall\"")
    #define IEN_WARNINGS_DISABLE_END _Pragma("GCC diagnostic pop")
#endif

IEN_WARNINGS_DISABLE_BEGIN

#ifdef LIBIEN_STB_ENABLE_IMAGE
    #define STB_IMAGE_IMPLEMENTATION
    #include "../include/stb_image.h"

    #define STB_IMAGE_RESIZE_IMPLEMENTATION
    #include "../include/stb_image_resize.h"

    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "../include/stb_image_write.h"
#endif

#ifdef LIBIEN_STB_ENABLE_FONTS
    #define STB_TRUETYPE_IMPLEMENTATION
    #include "../include/stb_truetype.h"
#endif

#define STBI_WINDOWS_UTF8
#define STBIW_WINDOWS_UTF8

IEN_WARNINGS_DISABLE_END