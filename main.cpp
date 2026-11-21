#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "/mnt/d/lib/stb (чехословакии)/stb_image.h"
#include "/mnt/d/lib/stb (чехословакии)/stb_image_write.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <cstdint>
#include <charconv>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include "glyph.h"
#include <fstream>
#include <sstream>

void print_sample(int width, int height, int channels, unsigned char* data){
    int index;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            index = (i * width + j) * channels;
            for (int c = 0; c < channels; c++) {
                std::cout << (int)data[index + c];
                if (c < channels - 1) {
                    std::cout << ',';
                }
            }
            std::cout << ' ';
        }
        std::cout << '\n';
        if (i == 1) break;
    }
}

int mult = 4;
int mult_y = 8;
int width_, height_, channels;
int cells_x, cells_y;
std::vector<unsigned char> bg{0,0,0};

std::vector<std::vector<std::vector<unsigned char>>> buffer(32);
float distance(unsigned char a, unsigned char b){
    float d = float(a) - float(b);
    return d * d;
}

struct RGB {
    uint8_t r, g, b;
};
std::vector<RGB> const_colours = {
    {0, 0, 0},       // Black
    {255, 255, 255}, // White
    {192, 192, 192}, // Silver
    {128, 128, 128}, // Gray
    {255, 0, 0},     // Red
    {128, 0, 0},     // Maroon
    {255, 255, 0},   // Yellow
    {128, 128, 0},   // Olive
    {0, 255, 0},     // Lime
    {0, 128, 0},     // Green
    {0, 255, 255},   // Aqua
    {0, 128, 128},   // Teal
    {0, 0, 255},     // Blue
    {0, 0, 128},     // Navy
    {255, 0, 255},   // Fuchsia
    {128, 0, 128},   // Purple
    {64, 64, 64},    // Charcoal
    {224, 224, 224}, // Gainsboro
    {192, 0, 0},     // Crimson
    {255, 128, 0},   // Orange
    {255, 165, 0},   // Tangerine
    {255, 69, 0},    // Vermilion
    {255, 127, 80},  // Coral
    {250, 128, 114}, // Salmon
    {255, 192, 203}, // Pink
    {255, 105, 180}, // Hotpink
    {165, 42, 42},   // Brown
    {139, 69, 19},   // Saddle
    {210, 105, 30},  // Chocolate
    {210, 180, 140}, // Tan
    {245, 245, 220}, // Beige
    {255, 215, 0},   // Gold
    {240, 230, 140}, // Khaki
    {255, 255, 224}, // Ivory
    {255, 253, 208}, // Cream
    {173, 255, 47},  // Chartreuse
    {154, 205, 50},  // Yellowgreen
    {107, 142, 35},  // Olivedrab
    {50, 205, 50},   // Limegreen
    {34, 139, 34},   // Forest
    {0, 100, 0},     // Darkgreen
    {144, 238, 144}, // Lightgreen
    {143, 188, 143}, // Sage
    {46, 139, 87},   // Seagreen
    {60, 179, 113},  // Emerald
    {0, 250, 154},   // Spring
    {127, 255, 212}, // Aquamarine
    {64, 224, 208},  // Turquoise
    {0, 206, 209},   // Cyan
    {70, 130, 180},  // Steel
    {135, 206, 235}, // Sky
    {0, 191, 255},   // Deepsky
    {30, 144, 255},  // Dodger
    {100, 149, 237}, // Cornflower
    {65, 105, 225},  // Royal
    {0, 0, 205},     // Cobalt
    {25, 25, 112},   // Midnight
    {75, 0, 130},    // Indigo
    {138, 43, 226},  // Violet
    {148, 0, 211},   // Darkviolet
    {186, 85, 211},  // Orchid
    {221, 160, 221}, // Plum
    {230, 230, 250}, // Lavender
    {199, 21, 133}   // Ruby
};

void load_colours(const std::string& path) {
    if (path.empty()) return;

    std::ifstream in(path);
    if(!in.is_open()){
        return;
    }
    std::string line;
    int i{0};
    unsigned char r,g,b;
    std::stringstream ss;
    while (std::getline(in, line)) {
        ss = std::stringstream (line);
        if (ss >> r >> g >> b) {
            const_colours[i] = {
                static_cast<unsigned char>(r),
                static_cast<unsigned char>(g),
                static_cast<unsigned char>(b)
            };
            i++;
        }
    }
    in.close();
}


struct OKLab {
    float L, a, b;
};
inline float srgbToLinear(uint8_t c) {
    float v = c / 255.0f;
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
}
OKLab rgbToOklab(RGB color) {
    float r = srgbToLinear(color.r);
    float g = srgbToLinear(color.g);
    float b = srgbToLinear(color.b);

    float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    float l_ = std::cbrt(l);
    float m_ = std::cbrt(m);
    float s_ = std::cbrt(s);

    return {
        0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
        1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
        0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_
    };
}
inline float dist_c(const OKLab& c1, const OKLab& c2) {
    float dL = c1.L - c2.L;
    float da = c1.a - c2.a;
    float db = c1.b - c2.b;
    return dL * dL + da * da + db * db;
}

int find_best_colour(std::vector<unsigned char>& pixel){
    int i{0}, index{0};
    float best = FLT_MAX;

    unsigned char mx = *std::max_element(pixel.begin(), pixel.end());
    float k = mx ? 255.0f / mx : 1.0f;

    for(const auto& c : const_colours){
        float dist = 0;
        OKLab lc = rgbToOklab(c);
        for(size_t j{0}; j + 2 < pixel.size(); j+=3){
            RGB p{uint8_t(std::min(255.0f, pixel[j]   * k)),
                uint8_t(std::min(255.0f, pixel[j+1] * k)),
                uint8_t(std::min(255.0f, pixel[j+2] * k))};
                dist += dist_c(rgbToOklab(p), lc);
        }
        if(dist < best) {best = dist; index = i;}
        i++;
    }

    return index;
}

int find_best_ascii(std::vector<unsigned char>& pixel){
    int i{0}, index{0};
    float best = FLT_MAX;
    for(const auto& b: buffer[mult]){
        float dist = 0;
        for(size_t j{0}; j < b.size(); j++){
            dist += distance(pixel[j], b[j]);
        }
        if(dist < best) {best = dist; index = i;}
        i++;
    }
    return index;
}
void build_glyph_buffer(const GlyphSet& gs, int gw, int gh){
    buffer[gw].clear();
    int top = 1;
    for(const auto& bm : gs.bitmaps){
        std::vector<unsigned char> small(gw * gh);
        for(int y{0}; y < gh; y++) for(int x{0}; x < gw; x++){
            int x0 = x * gs.cw / gw, x1 = std::max(x0 + 1, (x + 1) * gs.cw / gw);
            int y0 = y * gs.ch / gh, y1 = std::max(y0 + 1, (y + 1) * gs.ch / gh);
            int sum{0}, n{0};
            for(int sy = y0; sy < y1; sy++) for(int sx = x0; sx < x1; sx++){
                sum += bm[sy * gs.cw + sx];
                n++;
            }
            small[y * gw + x] = sum / n;
            top = std::max<int>(top, small[y * gw + x]);
        }
        buffer[gw].push_back(std::move(small));
    }
    for(auto& v : buffer[gw]) for(auto& p : v) p = std::min(255, p * 255 / top);
}
void make_bg(std::vector<unsigned char>& res){
    if(bg == std::vector<unsigned char>{0,0,0}) return;
    for(int i{0}; i < res.size(); i++){
        res[i] = bg[i%3];
    }
}


std::vector<unsigned char> colour_map(const GlyphSet& gs, std::vector<int>& table, std::vector<int>& colour){
    int W = cells_x * gs.cw, H = cells_y * gs.ch;
    std::vector<unsigned char> res(size_t(W) * H * 3, 0);
    make_bg(res);


    for(int cy{0}; cy < cells_y; cy++) for(int cx{0}; cx < cells_x; cx++){
        int cell = cy * cells_x + cx;
        const auto& glyph = gs.bitmaps[table[cell]];
        const RGB& col = const_colours[colour[cell]];
        for(int gy{0}; gy < gs.ch; gy++) for(int gx{0}; gx < gs.cw; gx++){
            int a = glyph[gy * gs.cw + gx];
            size_t o = (size_t(cy * gs.ch + gy) * W + (cx * gs.cw + gx)) * 3;
            res[o]     = res[o]     * (255 - a) / 255 + col.r * a / 255;
            res[o + 1] = res[o + 1] * (255 - a) / 255 + col.g * a / 255;
            res[o + 2] = res[o + 2] * (255 - a) / 255 + col.b * a / 255;
        }
    }
    return res;
}

std::vector<unsigned char> build_map(std::vector<unsigned char>& gray_img, std::vector<unsigned char>& img,
                                     std::vector<int>& res, std::vector<int>& colours, const GlyphSet& gs){
    std::vector<unsigned char> buffer_gray, buffer_colour;
    buffer_gray.resize(mult * mult_y);
    buffer_colour.resize(mult * mult_y * 3);

    res.assign(cells_x * cells_y, 0);
    colours.assign(cells_x * cells_y, 0);

    int j{0};
    for(int y{0}; y + mult_y <= height_; y += mult_y) for(int x{0}; x + mult <= width_; x += mult){
        int i{0};
        for(int dy{0}; dy < mult_y; dy++) for(int dx{0}; dx < mult; dx++){
            size_t p = size_t(y + dy) * width_ + (x + dx);
            buffer_gray[i] = gray_img[p];

            buffer_colour[i*3]     = img[p * 3];
            buffer_colour[i*3 + 1] = img[p * 3 + 1];
            buffer_colour[i*3 + 2] = img[p * 3 + 2];

            i++;
        }
        res[j] = find_best_ascii(buffer_gray);
        colours[j] = find_best_colour(buffer_colour);
        j++;
    }
    return colour_map(gs, res, colours);
}
struct Options {
    std::string input  = "image.jpg";
    std::string output = "out.png";
    std::string font   = "DejaVuSansMono.ttf";
    int  mult = 4;
    int  px   = 32;
    bool cyrillic = false;
    bool as_text  = false;
};

void print_help() {
    std::cout <<
    "использование: unu [опции] <картинка>\n"
    "  -i, --input <>файл    что превращать(image.png)\n"
    "  -o, --output <файл>   куда писать результат (out.png)\n"
    "      --font <файл>     путь к .ttf\n"
    "      --colours <файл>  таблица цветов (.txt)\n"
    "  -m, --mult <1..31>    ширина блока в пикселях (4)\n"
    "      --px <8..128>     размер шрифта (32)\n"
    "      --cyrillic        добавить кириллицу\n"
    "      --text            выводить текст, а не картинку\n"
    "  -h, --help            эта справка\n"
    "  -bg --background      установить цвет фона за символами (rgb) например \"255 255 255\"";
}

int parse_int(std::string_view name, std::string_view s, int lo, int hi) {
    int v{};
    auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc{} || p != s.data() + s.size() || v < lo || v > hi)
        throw std::runtime_error(std::string(name) + ": ожидалось число " + std::to_string(lo) + ".."
        + std::to_string(hi) + ", получено '" + std::string(s) + "'");
    return v;
}
void parse_colour(std::string s){
    std::stringstream ss(s);
    int r,g,b;
    if (ss >> r >> g >> b) {
        bg = {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b)
        };
    }
}

bool parse_args(int argc, char** argv, Options& o) {
    std::vector<std::string_view> args(argv + 1, argv + argc);
    bool only_positional = false;

    for (size_t i = 0; i < args.size(); ++i) {
        std::string_view a = args[i], value;
        bool inline_value = false;

        if (!only_positional && a.starts_with("--")) {
            if (auto eq = a.find('='); eq != std::string_view::npos) {
                value = a.substr(eq + 1);
                a = a.substr(0, eq);
                inline_value = true;
            }
        }
        auto next = [&]() -> std::string_view {
            if (inline_value) return value;
            if (i + 1 >= args.size()) throw std::runtime_error(std::string(a) + ": не указано значение");
            return args[++i];
        };
        auto flag = [&]() {
            if (inline_value) throw std::runtime_error(std::string(a) + ": флаг не принимает значение");
        };

            if (only_positional || a == "-" || !a.starts_with('-')) {
                if (!o.input.empty()) throw std::runtime_error("лишний аргумент: " + std::string(a));
                o.input = a;
            }
            else if (a == "--")                    { only_positional = true; }
            else if (a == "-h" || a == "--help")   { print_help(); return false; }
            else if (a == "-i" || a == "--input")  { o.input = next();}
            else if (a == "-o" || a == "--output") { o.output = next(); }
            else if (a == "--font")                { o.font = next(); }
            else if (a == "-bg"|| a == "--background") {parse_colour(std::string(next()));}
            else if (a == "--colour")              { load_colours(std::string(next()));}
            else if (a == "-m" || a == "--mult")   { o.mult = parse_int(a, next(), 1, 31); }
            else if (a == "--px")                  { o.px = parse_int(a, next(), 8, 128); }
            else if (a == "--cyrillic")            { flag(); o.cyrillic = true; }
            else if (a == "--text")                { flag(); o.as_text = true; }
            else throw std::runtime_error("неизвестная опция: " + std::string(a));
    }
    if (o.input.empty()) throw std::runtime_error("не указана входная картинка");
    return true;
}



int main(int argc, char** argv){
    Options o;
    try {
        if (!parse_args(argc, argv, o)) return 0;
    } catch (const std::exception& e) {
        std::cerr << "ошибка: " << e.what() << "\nunu --help - справка\n";
        return 2;
    }
    mult = o.mult;
    try {
        unsigned char *data = stbi_load(o.input.c_str(), &width_, &height_, &channels, 3);
        if(!data){
            std::cerr << "не удалось открыть " << o.input << ": " << stbi_failure_reason() << '\n';
            return 1;
        }
        std::cout << "каналов в файле: " << channels << ", " << width_ << "x" << height_ << '\n';

        std::vector<unsigned char> img(data, data + size_t(width_) * height_ * 3);
        stbi_image_free(data);
        std::vector<unsigned char> gray(size_t(width_) * height_);
        for(size_t i{0}; i < gray.size(); i++)
            gray[i] = (img[i*3] + img[i*3 + 1] + img[i*3 + 2]) / 3;

            const int px{32};

            GlyphSet gs(o.font, px,
                        {Ranges::Cyrillic}); //{Ranges::Ascii, Ranges::Cyrillic, Ranges::YoUpper, Ranges::YoLower}); // тут можно поменять используемые символы
            std::cout << "глифов: " << gs.bitmaps.size() << ", ячейка " << gs.cw << "x" << gs.ch << '\n';
            mult_y = std::max(1, int(std::lround(double(mult) * gs.ch / gs.cw)));
            cells_x = width_ / mult;
            cells_y = height_ / mult_y;
            if(mult >= int(buffer.size()) || cells_x == 0 || cells_y == 0){
                std::cerr << "картинка слишком мала для блока " << mult << "x" << mult_y << '\n';
                return 1;
            }

            build_glyph_buffer(gs, mult, mult_y);

            std::vector<int> table, colours;
            std::vector<unsigned char> out = build_map(gray, img, table, colours, gs);

            int W = cells_x * gs.cw, H = cells_y * gs.ch;
            if(!stbi_write_png(o.output.c_str(), W, H, 3, out.data(), W * 3)){
                std::cerr << "не удалось записать " << o.output << '\n';
                return 1;
            }
            std::cout << "готово: " << o.output << " (" << W << "x" << H << ", символов "
            << cells_x << "x" << cells_y << ")\n";
    } catch(const std::exception& e){
        std::cerr << "ошибка: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
