#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

struct CodeRange { char32_t from, to; };

namespace Ranges {
    inline const CodeRange Ascii    {0x0020, 0x007E};
    inline const CodeRange Cyrillic {0x0410, 0x044F};
    inline const CodeRange YoUpper  {0x0401, 0x0401};
    inline const CodeRange YoLower  {0x0451, 0x0451};
}

class GlyphSet {
public:
    GlyphSet(const std::string& ttf, int px,
             const std::vector<CodeRange>& ranges = {Ranges::Ascii});

    std::string name;
    int cw = 0, ch = 0;                 // размер каждой картинки
    std::vector<char32_t> codepoints;
    std::vector<std::vector<unsigned char>> bitmaps;
};
