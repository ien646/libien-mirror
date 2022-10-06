#ifdef _MSC_VER
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("warning(push, 0)")
    #define IEN_WARNINGS_DISABLE_END _Pragma("warning(pop)")
#else
    #define IEN_WARNINGS_DISABLE_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"")
    #define IEN_WARNINGS_DISABLE_END _Pragma("GCC diagnostic pop")
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

IEN_WARNINGS_DISABLE_BEGIN
#include "../include/stb_image.h"
#include "../include/stb_image_resize.h"
#include "../include/stb_image_write.h"
IEN_WARNINGS_DISABLE_END