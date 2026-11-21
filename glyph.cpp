#include "glyph.h"

GlyphSet::GlyphSet
(const std::string& ttf, int px,
const std::vector<CodeRange>& ranges) : name(ttf) {
FT_Library lib = nullptr;
FT_Face face = nullptr;

if (FT_Init_FreeType(&lib))
    throw std::runtime_error("FT_Init_FreeType");
if (FT_New_Face(lib, ttf.c_str(), 0, &face)) {
    FT_Done_FreeType(lib);
    throw std::runtime_error("не удалось открыть шрифт: " + ttf);
}
FT_Set_Pixel_Sizes(face, 0, px);

int ascender = face->size->metrics.ascender >> 6;
ch = face->size->metrics.height >> 6;

// временное хранилище отрисованных глифов
struct Tmp { int w, h, left, top, adv; std::vector<unsigned char> a; };
std::vector<Tmp> tmp;

for (const CodeRange& r : ranges)
    for (char32_t c = r.from; c <= r.to; ++c) {
        if (FT_Get_Char_Index(face, c) == 0) continue;   // нет в шрифте
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        FT_GlyphSlot s = face->glyph;
        Tmp t{int(s->bitmap.width), int(s->bitmap.rows),
            s->bitmap_left, s->bitmap_top, int(s->advance.x >> 6), {}};
            t.a.resize(size_t(t.w) * t.h);
            for (int y = 0; y < t.h; ++y)
                std::copy_n(s->bitmap.buffer + y * s->bitmap.pitch, t.w,
                            t.a.begin() + y * t.w);

                cw = std::max({cw, t.adv, t.left + t.w});
            codepoints.push_back(c);
            tmp.push_back(std::move(t));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(lib);

    // раскладываем каждый символ в картинку cw x ch
    bitmaps.resize(tmp.size());
    for (size_t i = 0; i < tmp.size(); ++i) {
        const Tmp& t = tmp[i];
        bitmaps[i].assign(size_t(cw) * ch, 0);
        for (int y = 0; y < t.h; ++y)
            for (int x = 0; x < t.w; ++x) {
                int px2 = t.left + x, py = ascender - t.top + y;
                if (px2 >= 0 && px2 < cw && py >= 0 && py < ch)
                    bitmaps[i][size_t(py) * cw + px2] = t.a[y * t.w + x];
            }
    }
}
