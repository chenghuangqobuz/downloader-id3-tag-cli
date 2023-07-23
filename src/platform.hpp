#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <taglib/tstring.h>

#ifdef _MSC_VER
  #define TAGLIB_HEADERS_BEGIN __pragma(warning(disable: 4251))
#else
  #define TAGLIB_HEADERS_BEGIN
#endif

#ifdef _MSC_VER
  #define TAGLIB_HEADERS_END __pragma(warning(default: 4251))
#else
  #define TAGLIB_HEADERS_END
#endif

#ifdef _MSC_VER
  #include <Windows.h>
  #include <io.h>
  #include <fcntl.h>
#define MAIN wmain
#else
#define MAIN main
#endif

namespace platform
{
#ifdef _WIN32
    inline struct static_initialization
    {
        static_initialization()
        {
            /*
            constexpr char cp_utf16le[] = ".1200";
            setlocale(LC_ALL, cp_utf16le);
            _setmode(_fileno(stdout), _O_WTEXT);
            */
        }
    } init;

    using char_t = wchar_t;
    using string = std::wstring; // windows uses UTF16BE
#elif __unix__
    using char_t = char;
    using string = std::string; // linux should use UTF8
#elif __APPLE__
    using char_t = char;
    using string = std::string; // macOS should use UTF8
#else
    static_assert("Error: Unknown platform");
#endif
}
