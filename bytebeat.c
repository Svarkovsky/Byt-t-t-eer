/*

    gcc -std=c99 -o bytebeat bytebeat.c sdlgfx.c sdlfont.c sdlpaudio.c -I. -L. -llua -lSDL2 -lm -O3 -march=native -mtune=native

    Or like this, to be more specific:
    gcc -std=c99 -o bytebeat bytebeat.c sdlgfx.c sdlfont.c sdlpaudio.c -I. -L. -llua -lSDL2 -lm -O3 -march=native -mtune=native \
    -msse3 -mssse3 -fno-exceptions -fomit-frame-pointer -flto -fvisibility=hidden -mfpmath=sse -ffast-math -pipe \
    -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all -DNDEBUG

*/

/*

    MIT License

    Copyright (c) Ivan Svarkovsky - 2025

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE.
*/

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "sdlgfx.h"
#include "sdlpaudio.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAMPLE_RATE 8000
#define CHANNEL_COUNT 2
#define MAX_INPUT_LEN 4096
#define FONT_WIDTH 8
#define FONT_HEIGHT 10

#define COPYRIGHT_TEXT "Copyright (c) Ivan Svarkovsky - 2025"
#define VERSION "v0.1"

const char* embedded_bit32_lua_code =
    "-- this code replicates the bit32 library for Lua 5.4\n"
    "local bit32 = {}\n"
    "function bit32.band(...)\n"
    "  local result = 0xFFFFFFFF\n"
    "  for i = 1, select(\"#\", ...) do\n"
    "    result = result & select(i, ...)\n"
    "  end\n"
    "  return result & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.bor(...)\n"
    "  local result = 0\n"
    "  for i = 1, select(\"#\", ...) do\n"
    "    result = result | select(i, ...)\n"
    "  end\n"
    "  return result & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.bnot(n)\n"
    "  return ~n & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.bxor(...)\n"
    "  local result = 0\n"
    "  for i = 1, select(\"#\", ...) do\n"
    "    result = result ~ select(i, ...)\n"
    "  end\n"
    "  return result & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.lshift(n, shift)\n"
    "  return (n << shift) & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.rshift(n, shift)\n"
    "  return (n & 0xFFFFFFFF) >> shift\n"
    "end\n"
    "function bit32.arshift(n, shift)\n"
    "  local sign = n & 0x80000000\n"
    "  n = n >> shift\n"
    "  if sign ~= 0 then\n"
    "    n = n | ~(0xFFFFFFFF >> shift)\n"
    "  end\n"
    "  return n & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.btest(...)\n"
    "  return bit32.band(...) ~= 0\n"
    "end\n"
    "function bit32.extract(n, field, width)\n"
    "  width = width or 1\n"
    "  local mask = 2^width - 1\n"
    "  return ((n >> field) & mask) & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.replace(n, v, field, width)\n"
    "  width = width or 1\n"
    "  local mask = ~(2^(width + field) - 2^field)\n"
    "  return ((n & mask) | ((v << field) & ~mask)) & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.lrotate(n, shift)\n"
    "  shift = shift & 31\n"
    "  local n32 = n & 0xFFFFFFFF\n"
    "  return ((n32 << shift) | (n32 >> (32 - shift))) & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.rrotate(n, shift)\n"
    "  shift = shift & 31\n"
    "  local n32 = n & 0xFFFFFFFF\n"
    "  return ((n32 >> shift) | (n32 << (32 - shift))) & 0xFFFFFFFF\n"
    "end\n"
    "function bit32.badd(a, b)\n"
    "  return bit32.band(a + b, 0xFFFFFFFF)\n"
    "end\n"
    "function bit32.bsub(a, b)\n"
    "  return bit32.band(a - b, 0xFFFFFFFF)\n"
    "end\n"
    "function bit32.bmul(a, b)\n"
    "  return bit32.band(a * b, 0xFFFFFFFF)\n"
    "end\n"
    "function bit32.bdiv(a, b)\n"
    "  if b == 0 then return 0 end\n"
    "  return bit32.band(math.floor(a / b), 0xFFFFFFFF)\n"
    "end\n"
    "return bit32\n";

void bytebeat_squsinsaw(Uint32 t, float* sample, int sr)
{
    float squ_array[32] = {1, 0, 2, 0, 1, 0, 2, 2, 1, 0, 2, 0, 1, 0, 2, 0, 1.5, 0, 3, 3, 1.5, 0, 3, 0, 1.35, 0, 2.7, 2.7, 1.35, 0, 2.7, 0 };
    float sine_freq[4] = {8, 9, 9.5, 12 };
    float sine_mod[4] = {1, 1, 1.5, 1.35 };
    float saw_array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 8, 9.5, 12, 16, 0, 0, 0, 0 };
    char snr_pattern[4] = {'0', '1', '0', '1' };
    float enable_array[8] = {0, 0, 1, 1, 1, 1, 1, 1 };
    Uint32 squ = (Uint32)(t * squ_array[(t >> 10) & 31]) & 64;
    float sine = 32 * sinf(t / 40.743648 * sine_freq[(t >> 10) & 3] * sine_mod[(t >> 13) & 3]) + 32;
    Uint32 saw = (Uint32)((t / 4) * saw_array[(t >> 8) & 15]) & 31;
    Uint32 kick = (Uint32)(100000.0 / ((t & 4095) + 1) / 5) & 63;
    Uint32 snr = (((t * t * t) >> 10) & 63 | (t >> 6) & 63) * (snr_pattern[(t >> 12) & 3] - '0');
    float result = fmodf(sine, (float)(-(t >> 4 & 63))) + squ * enable_array[(t >> 16) & 7] + kick * enable_array[(t >> 16) & 7] + snr * enable_array[(t >> 16) & 7] + saw * enable_array[(t >> 16) & 7];
    *sample = result / 128.0f - 1.0f;
}

static lua_State *L = NULL;
static int func_ready = 0;
static SDL_mutex *lua_mutex = NULL;

// *** PRESET FORMULAS ***
#define NUM_PRESET_FORMULAS 25

char preset_formulas[NUM_PRESET_FORMULAS][MAX_INPUT_LEN] = {
    "bit32.band(bit32.bor(bit32.bdiv(30000, bit32.band(t, 4095)), bit32.rshift(bit32.bmul(bit32.bmul(t, 5), bit32.band(bit32.band(bit32.rshift(t, 12), 7), bit32.rshift(bit32.bsub(0, t), 10))), bit32.band(bit32.rshift(t, 8), 3))), 128)", // 1
    "bit32.band(bit32.band(bit32.bmul(t,bit32.badd(4,bit32.band(bit32.rshift(t,9),3))),bit32.rshift(bit32.bdiv(t,3),7)),224)", // 2
    "t|t%255|t%257",                                                                                                // 3
    "t>>(t&7)|t<<(t&42)|t>>7|t<<5",                                                                                 // 4
    "bit32.bsub(bit32.bor(bit32.bor(bit32.band(bit32.bmul(t,9),bit32.rshift(t,4)),bit32.band(bit32.bmul(t,5),bit32.rshift(t,7))),bit32.band(bit32.bmul(t,3),bit32.bdiv(t,1024))),1)", // 5
    "(t&t>>12)*(t>>4|t>>8)",                                                                                         // 6
    "t*(0xCA98>>(t>>9&14)&15)|t>>8",                                                                                 // 7
    "t*((t>>9|t>>13)&15)&129",                                                                                        // 8
    "bit32.badd(bit32.bmul(bit32.bmul(2,bit32.bsub(1,bit32.band(bit32.rshift(bit32.badd(t,10),bit32.band(bit32.rshift(t,9),bit32.rshift(t,14))),bit32.rshift(t,4),-2)),bit32.band(bit32.bxor(bit32.rshift(t,10),bit32.rshift(bit32.badd(t,bit32.band(bit32.rshift(t,6),127)),10)),1)),32),128)", // 9
    "9*(t*((t>>9|t>>13)&15)&16)",                                                                                       // 10
    "(1+(t>>10)%7)*((t*(1+(t>>13)%4))%(24+(t>>14)*9%8)&16)*10",                                                     // 11
    "bit32.bdiv(t,t%bit32.bor(bit32.rshift(t,8),bit32.rshift(t,16)))",                                              // 12
    "(t*3&t>>6)%76",                                                                                                  // 13
    "(t/t+(t>>12)%6+(t>>16)%3)*10*t>>4",                                                                               // 14
    "t*(4&t>>14)+t*(3-(4&t>>10))*(2+(5&t>>12))",                                                                     // 15
    "-t*(0xCA98CA98>>(-t>>9&30)&15)|-t>>8",                                                                           // 16
    "t*t>>9|t*(t>>16)|t>>4",                                                                                         // 17
    "t*(t>>12)<<(-t>>10&7)&-t>>2"                                                                                     // 18
    "t*((t>>12|t>>8)&63&t>>4)"											     // 19
    "(43*(t|t>>11)&255)*(8|~t>>6&55)>>6" 									     // 20
    "(t<<1&t*3&-t>>5&t>>10)-1"  										     // 21
    "(t*(2&t>>10|(t>>10&18)-8)&128)*(-t>>2&255)>>7"                                                                  // 22
    "(2*t&255)*(-t>>6&t>>7)>>8|t&t>>1|t&t>>1"          								     // 23
    "t>>3&127|t>>4&128|t>>3&128"                  								     // 24
    "-(3*t&t>>13&t>>6)*t>>7" 										             // 25
};

int current_preset_index = 0;

char custom_input[MAX_INPUT_LEN] = "";

int input_active = 0;
int cursor_pos = 0;
int scroll_offset = 0;
int info_active = 1;
int license_active = 0;

void init_lua()
{
    if (!L) {
        L = luaL_newstate();
        luaL_openlibs(L);
        lua_mutex = SDL_CreateMutex();
        if (luaL_dostring(L, embedded_bit32_lua_code) != LUA_OK) {
            printf("Error loading embedded bit32 library: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else {
            printf("Embedded bit32 library loaded successfully.\n");
            lua_setglobal(L, "bit32");
        }
        lua_getglobal(L, "bit32");
        if (!lua_istable(L, -1)) {
            printf("Failed to register bit32 as global table!\n");
        } else {
            printf("bit32 is available in global scope.\n");
        }
        lua_pop(L, 1);
        printf("init_lua: Lua state L = %p\n", L);
    }
}

static int mute_animation_active = 0;
static Uint32 mute_animation_start_time = 0;
static const float MUTE_ANIMATION_DURATION = 1000.0f;

void draw_mute_animation(int width, int height, SDLPAudio* audio)
{
    if (!mute_animation_active || audio->enabled) {
        return;
    }
    Uint32 current_time = SDL_GetTicks();
    float elapsed = (float)(current_time - mute_animation_start_time);
    float t = elapsed / MUTE_ANIMATION_DURATION;
    if (t > 1.0f) {
        t = 1.0f;
    }
    const char* mute_text = "MUTE";
    int text_width = strlen(mute_text) * FONT_WIDTH;
    int text_height = FONT_HEIGHT;
    int center_x = (width - text_width) / 2;
    int center_y = (height - text_height) / 2;
    int brightness = (int)(255 * (1.0f - t));
    if (brightness < 0) {
        brightness = 0;
    }
    sdlgfx_color(brightness, brightness, brightness);
    sdlgfx_string(center_x, center_y, mute_text);
    int distance = (int)(100.0f * t);
    sdlgfx_color(255, 255, 255);
    sdlgfx_string(center_x + distance + text_width, center_y, mute_text);
    sdlgfx_string(center_x - distance - text_width, center_y, mute_text);
    sdlgfx_string(center_x, center_y - distance - text_height, mute_text);
    sdlgfx_string(center_x, center_y + distance + text_height, mute_text);
    sdlgfx_string(center_x + distance + text_width * 7.8, center_y, mute_text);
    sdlgfx_string(center_x - distance - text_width * 7.8, center_y, mute_text);
}

void set_bytebeat_custom(const char* input)
{
    SDL_LockMutex(lua_mutex);
    if (!L) {
        init_lua();
    }
    func_ready = 0;
    char normalized_input[MAX_INPUT_LEN];
    strncpy(normalized_input, input, MAX_INPUT_LEN - 1);
    normalized_input[MAX_INPUT_LEN - 1] = '\0';
    for (int i = 0; normalized_input[i]; i++) {
        if (normalized_input[i] == 'T') {
            normalized_input[i] = 't';
        }
    }
    char lua_code[512];
    /* old */
    snprintf(lua_code, sizeof(lua_code),
             "function f(t)\n"
             "  t = bit32.band(t, 0xFFFFFFFF)\n"
             "  local result = %s\n"
             "  return bit32.band(result, 255)\n"
             "end",
             normalized_input);
    if (luaL_dostring(L, lua_code) != LUA_OK) {
        printf("Lua Error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        SDL_UnlockMutex(lua_mutex);
        return;
    }
    lua_getglobal(L, "f");
    if (!lua_isfunction(L, -1)) {
        printf("Failed to get function 'f'\n");
        lua_pop(L, 1);
    } else {
        printf("Function set successfully: %s\n", normalized_input);
        func_ready = 1;
    }
    lua_pop(L, 1);
    SDL_UnlockMutex(lua_mutex);
}
/* old   */
void bytebeat_custom(Uint32 t, float* sample, int sr)
{
    SDL_LockMutex(lua_mutex);
    if (!L) {
        init_lua();
    }
    if (!func_ready || strlen(custom_input) == 0) {
        *sample = 0.0f;
        SDL_UnlockMutex(lua_mutex);
        return;
    }
    lua_getglobal(L, "bit32");
    if (!lua_istable(L, -1)) {
        printf("bit32 library not found!\n");
        lua_pop(L, 1);
        *sample = 0.0f;
        SDL_UnlockMutex(lua_mutex);
        return;
    }
    lua_pop(L, 1);
    lua_getglobal(L, "f");
    if (!lua_isfunction(L, -1)) {
        printf("Function 'f' not found!\n");
        lua_pop(L, 1);
        *sample = 0.0f;
        SDL_UnlockMutex(lua_mutex);
        return;
    }
    lua_pushinteger(L, t);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        static Uint32 last_error_time = 0;
        Uint32 current_time = SDL_GetTicks();
        if (current_time - last_error_time > 1) {
            printf("Execution error: %s for t: %u\n", lua_tostring(L, -1), t);
            last_error_time = current_time;
        }
        lua_pop(L, 1);
        *sample = 0.0f;
    } else {
        if (lua_isnumber(L, -1)) {
            double res = lua_tonumber(L, -1);
            *sample = (float)res / 128.0f - 1.0f;
//            printf("t: %u, raw result: %f, sample: %f\n", t, res, *sample);
        } else {
            printf("t: %u, result is not a number\n", t);
            *sample = 0.0f;
        }
        lua_pop(L, 1);
    }
    SDL_UnlockMutex(lua_mutex);
}

void cleanup_lua()
{
    SDL_LockMutex(lua_mutex);
    if (L) {
        lua_close(L);
    }
    SDL_UnlockMutex(lua_mutex);
    if (lua_mutex) {
        SDL_DestroyMutex(lua_mutex);
    }
}

void draw_waveform(int width, int height, SDLPAudio* audio)
{
    if (!audio->enabled) {
        return;
    }
    int num_samples = 512;
    float sample_step = (float)audio->sample_rate / num_samples;
    float prev_sample = 0.0f, curr_sample = 0.0f, next_sample = 0.0f;
    int prev_y = 0, curr_y = 0;
    audio->bytebeat_func(audio->audio_pos - num_samples * sample_step, &prev_sample, audio->sample_rate);
    prev_y = height / 2 + (int)(prev_sample * height / 4);
    for (int i = 1; i < num_samples; i++) {
        Uint32 t_curr = audio->audio_pos - (num_samples - i) * sample_step;
        audio->bytebeat_func(t_curr, &curr_sample, audio->sample_rate);
        curr_y = height / 2 + (int)(curr_sample * height / 4);
        int x1 = (i - 1) * width / num_samples;
        int x2 = i * width / num_samples;
        // Цвет, зависящий от X-координаты через XOR - ТЕПЕРЬ БЕЛЫЙ
        int color_r = 255, color_g = 255, color_b = 255;
        color_r = color_r ^ (x2 & 0xFF);
        color_g = color_g ^ (x2 & 0xFF);
        color_b = color_b | (x2 & 0xFF);
        sdlgfx_color(color_r, color_g, color_b);
        sdlgfx_line(x1, prev_y, x2, curr_y);
        if (i < num_samples - 1) {
            Uint32 t_next = audio->audio_pos - (num_samples - (i + 1)) * sample_step;
            audio->bytebeat_func(t_next, &next_sample, audio->sample_rate);
            if ((curr_sample > prev_sample && curr_sample > next_sample) ||
                    (curr_sample < prev_sample && curr_sample < next_sample)) {
                sdlgfx_color(255, 228, 0);
                sdlgfx_fill_rectangle(x2 - 1, curr_y -1, x2 + 1, curr_y + 1);
            }
            prev_sample = curr_sample;
            prev_y = curr_y;
            curr_sample = next_sample;
        }
    }
}

void draw_input_box(int form_x, int form_y, int form_width, int form_height, const char* text, int cursor_pos)
{
    sdlgfx_color(0, 0, 0);
    sdlgfx_fill_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    sdlgfx_color(255, 255, 255);
    sdlgfx_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    int line_number_width = 50;
    int max_line_width_chars = (form_width - line_number_width - 20) / FONT_WIDTH;
    int line_height = FONT_HEIGHT * 2;
    int max_visible_lines = (form_height - 10) / line_height;
    int current_y = form_y + 20;
    int line_start_index = 0;
    int line_num = 0;
    int line_width = 0;
    int lines_drawn = 0;
    const char* current_char = text;
    while (*current_char != '\0' && line_num < max_visible_lines) {
        if (line_num >= scroll_offset) {
            int visible_line_index = line_num - scroll_offset;
            char num_buf[10];
            snprintf(num_buf, sizeof(num_buf), "%3d ", line_num + 1);
            sdlgfx_color(255, 255, 0);
            sdlgfx_string(form_x + 10, current_y, num_buf);
            line_width = 0;
            const char* line_start = current_char;
            while (*current_char != '\0' && *current_char != '\n' && line_width < max_line_width_chars) {
                current_char++;
                line_width++;
            }
            sdlgfx_color(255, 255, 255);
            char line_buf[256] = {0};
            strncpy(line_buf, line_start, line_width);
            line_buf[line_width] = '\0';
            if (lines_drawn < 21) {
                lines_drawn++;

                sdlgfx_string(form_x + line_number_width, current_y, line_buf);
                if (cursor_pos >= line_start_index && cursor_pos <= line_start_index + line_width) {
                    int cursor_x = form_x + line_number_width + (cursor_pos - line_start_index) * FONT_WIDTH;
                    int line_end_x = form_x + line_number_width + line_width * FONT_WIDTH;
                    if (cursor_x <= line_end_x) {
                        int cursor_y_to_draw = current_y;
                        if (lines_drawn >= 21) {
                            cursor_y_to_draw = form_y + 20 + (20) * line_height;
                        }
                        sdlgfx_color(255, 0, 0);
                        sdlgfx_fill_rectangle(cursor_x, cursor_y_to_draw, cursor_x + 8, cursor_y_to_draw + FONT_HEIGHT);
                    }
                }
            }
            current_y += line_height;
        } else {
            line_width = 0;
            while (*current_char != '\0' && *current_char != '\n' && line_width < max_line_width_chars) {
                current_char++;
                line_width++;
            }
        }
        if (*current_char == '\n') {
            current_char++;
        } else if (*current_char != '\0') {
            current_char++;
        }
        line_num++;
        line_start_index += line_width + 1;
    }
}

void draw_info_box(int form_x, int form_y, int form_width, int form_height)
{
    sdlgfx_color(0, 0, 0);
    sdlgfx_fill_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    sdlgfx_color(255, 255, 255);
    sdlgfx_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    int current_y = form_y + 20;
    int line_height = FONT_HEIGHT + 5;
    int indent = 20;
    const char* title_text = "Byt-t-t-eer - Bytebeat Generator";
    int title_width = strlen(title_text) * FONT_WIDTH;
    int title_x = form_x + (form_width - title_width) / 2;
    sdlgfx_string(title_x, current_y, title_text);
    current_y += line_height * 2;
    sdlgfx_string(form_x + indent, current_y, "A simple bytebeat music visualizer.");
    current_y += line_height * 2;
    sdlgfx_string(form_x + indent, current_y, "Lua Syntax for Custom Formulas:");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Use 't' as the time variable.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Bitwise operations: bit32.band(a,b), bit32.bor(a,b),");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "  bit32.bxor(a,b), bit32.bnot(a)");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Shifts: bit32.lshift(a,n), bit32.rshift(a,n)");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Arithmetic: bit32.badd(a,b), bit32.bsub(a,b),");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "  bit32.bmul(a,b), bit32.bdiv(a,b)");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Example: bit32.bmul(t,5) for t*5");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Example: bit32.bor(t, bit32.lshift(t, 2)) for t | (t << 2)");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Result is masked to 8 bits (0-255) and");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "  normalized to -1.0 to 1.0.");
    current_y += line_height * 2;
    sdlgfx_string(form_x + indent, current_y, "Controls:");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+C: Enter/Edit bytebeat formula.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+I: Show/Hide Info window.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+M: Mute/Unmute audio.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+L: View License.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+Q: Quit.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+V: Paste from clipboard (in input mode).");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+K: Clear input (in input mode).");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Ctrl+P: Next Preset Formula.");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Enter: Apply formula (in input mode).");
    current_y += line_height;
    sdlgfx_string(form_x + indent * 2, current_y, "- Left/Right: Move cursor (in input mode).");
    sdlgfx_string(form_x + form_width - strlen(VERSION) * FONT_WIDTH - 10, form_y + form_height - FONT_HEIGHT - 5, VERSION);
}

// Add draw_license_box function, based on draw_info_box
void draw_license_box(int form_x, int form_y, int form_width, int form_height) {
    sdlgfx_color(0, 0, 0);
    sdlgfx_fill_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    sdlgfx_color(255, 255, 255);
    sdlgfx_rectangle(form_x, form_y, form_x + form_width, form_y + form_height);
    int current_y = form_y + 20;
    int line_height = FONT_HEIGHT + 5;
    int indent = 20;

    // Centered title for License box
    const char* license_title = "MIT License (Simplified)";
    int title_width = strlen(license_title) * FONT_WIDTH;
    int title_x = form_x + (form_width - title_width) / 2;
    sdlgfx_string(title_x, current_y, license_title);
    current_y += line_height * 2;

    // Left-aligned license text with indent
    sdlgfx_string(form_x + indent, current_y, "Permission is hereby granted, free of charge, to any person");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "obtaining a copy of this software (the \"Software\"), to deal");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "in the Software without restriction, including without limitation");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "the rights to use, copy, modify, merge, publish, distribute,");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "sublicense, and/or sell copies of the Software...");
    current_y += line_height * 2;
    sdlgfx_string(form_x + indent, current_y, "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "NONINFRINGEMENT. IN NO EVENT SHALL AUTHORS OR COPYRIGHT HOLDERS");
    current_y += line_height;
    sdlgfx_string(form_x + indent, current_y, "BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY...");
    current_y += line_height * 2;
    sdlgfx_string(form_x + indent, current_y, "See full license in the source code.");
}

int is_valid_char(char c)
{
    return (c >= 32 && c < 127) && (isalnum(c) || strchr("+-*/&|^<>%(),.~?: ", c) != NULL);
}

SDL_Surface* create_icon_surface()
{
    SDL_Surface* icon_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!icon_surface) {
        printf("Failed to create icon surface: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FillRect(icon_surface, NULL, SDL_MapRGB(icon_surface->format, 255, 255, 255));
    Uint32 black = SDL_MapRGB(icon_surface->format, 0, 0, 0);
    Uint32 yellow = SDL_MapRGB(icon_surface->format, 255, 255, 0);
    Uint32 blue = SDL_MapRGB(icon_surface->format, 0, 0, 255);
    Uint32 red = SDL_MapRGB(icon_surface->format, 255, 0, 0);
    Uint32* pixels = (Uint32*)icon_surface->pixels;
    int pitch = icon_surface->pitch / 4;
    for (int y = 8; y < 28; y++) {
        for (int x = 13; x <= 18; x++) {
            if (y < 14) {
                pixels[y * pitch + x] = yellow;
            } else if (y < 20) {
                pixels[y * pitch + x] = blue;
            } else {
                pixels[y * pitch + x] = red;
            }
        }
    }
    for (int y = 5; y <= 10; y++) {
        for (int x = 6; x <= 25; x++) {
            pixels[y * pitch + x] = yellow;
        }
    }
    for (int y = 5; y <= 10; y++) {
        pixels[y * pitch + 5] = yellow;
        pixels[y * pitch + 4] = yellow;
    }
    for (int y = 5; y <= 10; y++) {
        pixels[y * pitch + 26] = yellow;
        pixels[y * pitch + 27] = yellow;
    }
    for (int x = 13; x <= 18; x++) {
        pixels[27 * pitch + x] = red;
        pixels[28 * pitch + x] = red;
    }
    pixels[27 * pitch + 12] = red;
    pixels[27 * pitch + 19] = red;
    pixels[28 * pitch + 12] = red;
    pixels[28 * pitch + 19] = red;
    for (int y = 4; y <= 29; y++) {
        for (int x = 3; x <= 28; x++) {
            if (pixels[y * pitch + x] == SDL_MapRGB(icon_surface->format, 255, 255, 255)) {
                if ((x > 3 && pixels[y * pitch + (x - 1)] != SDL_MapRGB(icon_surface->format, 255, 255, 255)) ||
                        (x < 28 && pixels[y * pitch + (x + 1)] != SDL_MapRGB(icon_surface->format, 255, 255, 255)) ||
                        (y > 4 && pixels[(y - 1) * pitch + x] != SDL_MapRGB(icon_surface->format, 255, 255, 255)) ||
                        (y < 29 && pixels[(y + 1) * pitch + x] != SDL_MapRGB(icon_surface->format, 255, 255, 255))) {
                    pixels[y * pitch + x] = black;
                }
            }
        }
    }
    return icon_surface;
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
    sdlgfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Byt-t-t-eer - Bytebeat Generator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    sdlgfx_clear_color(0, 0, 0);

    SDL_Surface* icon_surface = create_icon_surface();
    if (icon_surface) {
        SDL_SetWindowIcon(sdlgfx_window, icon_surface);
        SDL_FreeSurface(icon_surface);
    }

    SDLPAudio* audio = sdlpaudio_init(SAMPLE_RATE, CHANNEL_COUNT, bytebeat_squsinsaw);
    if (!audio) {
        sdlgfx_close();
        return 1;
    }

    audio->enabled = 0;
    int running = 1;
    SDL_StartTextInput();
    strncpy(custom_input, preset_formulas[current_preset_index], MAX_INPUT_LEN - 1);
    custom_input[MAX_INPUT_LEN - 1] = '\0';
    set_bytebeat_custom(custom_input);
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && (e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.scancode == SDL_SCANCODE_Q)) {
                running = 0;
            }
            if (e.type == SDL_KEYDOWN) { // Обработка нажатий клавиш (без KMOD_CTRL проверок)
                if (info_active && !(e.key.keysym.mod & KMOD_CTRL) ) { // Если info_active и НЕ Ctrl+...
                    info_active = 0; // Закрыть окно информации
                }
                if (license_active && !(e.key.keysym.mod & KMOD_CTRL)) { // Если license_active и НЕ Ctrl+...
                    license_active = 0; // Закрыть окно лицензии
                }
            }

            if (e.type == SDL_KEYDOWN && (e.key.keysym.mod & KMOD_CTRL)) { // Обработка Ctrl+комбинаций
                if (e.key.keysym.scancode == SDL_SCANCODE_Q) {
                    running = 0;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_M) {
                    if (audio->enabled) {
                        sdlpaudio_pause(audio);
                        mute_animation_active = 1;
                        mute_animation_start_time = SDL_GetTicks();
                    } else {
                        sdlpaudio_play(audio);
                        mute_animation_active = 0;
                    }
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_C) {
                    input_active = !input_active;
                    info_active = 0;
                    license_active = 0;
                    if (!input_active) {
                        set_bytebeat_custom(custom_input);
                        sdlpaudio_set_bytebeat(audio, bytebeat_custom);
                        if (!audio->enabled) {
                            sdlpaudio_play(audio);
                        }
                    }
                    cursor_pos = strlen(custom_input);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_I) {
                    info_active = !info_active;
                    input_active = 0;
                    license_active = 0;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_L) {
                    license_active = !license_active;
                    info_active = 0;
                    input_active = 0;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_V) {
                    char* clipboard = SDL_GetClipboardText();
                    if (clipboard) {
                        int clip_len = strlen(clipboard);
                        for (int i = 0; i < clip_len && strlen(custom_input) < MAX_INPUT_LEN - 1; i++) {
                            char c = clipboard[i];
                            if (is_valid_char(c)) {
                                memmove(&custom_input[cursor_pos + 1], &custom_input[cursor_pos], strlen(&custom_input[cursor_pos]) + 1);
                                custom_input[cursor_pos] = c;
                                cursor_pos++;
                            }
                        }
                        SDL_free(clipboard);
                    }
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_K) {
                    custom_input[0] = '\0';
                    cursor_pos = 0;
                    scroll_offset = 0;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_P) {
                    current_preset_index++;
                    if (current_preset_index >= NUM_PRESET_FORMULAS) {
                        current_preset_index = 0;
                    }
                    strncpy(custom_input, preset_formulas[current_preset_index], MAX_INPUT_LEN - 1);
                    custom_input[MAX_INPUT_LEN - 1] = '\0';
                    cursor_pos = strlen(custom_input);
                }
            }
            if (input_active && e.type == SDL_TEXTINPUT) {
                for (int i = 0; e.text.text[i]; i++) {
                    char c = e.text.text[i];
                    if (is_valid_char(c) && strlen(custom_input) < MAX_INPUT_LEN - 1) {
                        memmove(&custom_input[cursor_pos + 1], &custom_input[cursor_pos], strlen(&custom_input[cursor_pos]) + 1);
                        custom_input[cursor_pos] = c;
                        cursor_pos++;
                    }
                }
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                int len = strlen(custom_input);
                if (len > 0 && cursor_pos > 0) {
                    memmove(&custom_input[cursor_pos - 1], &custom_input[cursor_pos], strlen(&custom_input[cursor_pos]) + 1);
                    cursor_pos--;
                }
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                input_active = 0;
                set_bytebeat_custom(custom_input);
                sdlpaudio_set_bytebeat(audio, bytebeat_custom);
                if (!audio->enabled) {
                    sdlpaudio_play(audio);
                    mute_animation_active = 0;
                } else {
                    mute_animation_active = 1;
                }
                cursor_pos = 0;
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                if (cursor_pos > 0) {
                    cursor_pos--;
                }
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                if (cursor_pos < strlen(custom_input)) {
                    cursor_pos++;
                }
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_UP) {
                if (cursor_pos > 0) {
                    int current_line_start_index = 0;
                    int current_line_number = 0;
                    int current_pos_in_line = 0;
                    int temp_pos = 0;
                    const char* current_char = custom_input;
                    while (*current_char != '\0' && temp_pos < cursor_pos) {
                        if (*current_char == '\n') {
                            current_line_start_index = temp_pos + 1;
                            current_line_number++;
                        }
                        if (temp_pos < cursor_pos) {
                            current_pos_in_line = temp_pos - current_line_start_index;
                        }
                        current_char++;
                        temp_pos++;
                    }
                    if (current_line_number > 0) {
                        int prev_line_end_index = current_line_start_index - 1;
                        int prev_line_start_index = 0;
                        int prev_line_number = 0;
                        temp_pos = 0;
                        current_char = custom_input;
                        while (*current_char != '\0' && temp_pos < prev_line_end_index) {
                            if (*current_char == '\n') {
                                prev_line_start_index = temp_pos + 1;
                                prev_line_number++;
                            }
                            current_char++;
                            temp_pos++;
                        }
                        int prev_line_length = prev_line_end_index - prev_line_start_index;
                        cursor_pos = prev_line_start_index + SDL_min(current_pos_in_line, prev_line_length);
                    } else {
                        cursor_pos = 0;
                    }
                }
            }
            if (input_active && e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                if (cursor_pos < strlen(custom_input)) {
                    int current_line_start_index = 0;
                    int current_line_number = 0;
                    int current_pos_in_line = 0;
                    int temp_pos = 0;
                    const char* current_char = custom_input;
                    while (*current_char != '\0' && temp_pos < cursor_pos) {
                        if (*current_char == '\n') {
                            current_line_start_index = temp_pos + 1;
                            current_line_number++;
                        }
                        if (temp_pos < cursor_pos) {
                            current_pos_in_line = temp_pos - current_line_start_index;
                        }
                        current_char++;
                        temp_pos++;
                    }
                    int next_line_start_index = -1;
                    int next_line_number = current_line_number + 1;
                    temp_pos = 0;
                    current_char = custom_input;
                    while (*current_char != '\0') {
                        if (*current_char == '\n') {
                            if (current_line_number == next_line_number - 1) {
                                next_line_start_index = temp_pos + 1;
                                break;
                            }
                            next_line_number++;
                        }
                        current_char++;
                        temp_pos++;
                    }
                    if (next_line_start_index != -1) {
                        int next_line_end_index = strlen(custom_input);
                        int temp_index = next_line_start_index;
                        const char* temp_char = &custom_input[next_line_start_index];
                        while (*temp_char != '\0' && *temp_char != '\n') {
                            temp_char++;
                            temp_index++;
                        }
                        next_line_end_index = temp_index;
                        int next_line_length = next_line_end_index - next_line_start_index;
                        cursor_pos = next_line_start_index + SDL_min(current_pos_in_line, next_line_length);
                    } else {
                        cursor_pos = strlen(custom_input);
                    }
                }
            }
            if (input_active && e.type == SDL_MOUSEWHEEL) {
                scroll_offset -= e.wheel.y;
                if (scroll_offset < 0) {
                    scroll_offset = 0;
                }
            }
        }
        sdlgfx_gradient_vertical_ex(
            0, 0, 60, 0, 0, 0, 6, 3, 41, 0, 0, 0, 4,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0
        );
        draw_waveform(WINDOW_WIDTH, WINDOW_HEIGHT, audio);
        draw_mute_animation(WINDOW_WIDTH, WINDOW_HEIGHT, audio);
        int form_width = WINDOW_WIDTH * 3 / 4;
        int form_height = WINDOW_HEIGHT * 3 / 4;
        int form_x = (WINDOW_WIDTH - form_width) / 2;
        int form_y = (WINDOW_HEIGHT - form_height) / 2;
        if (input_active) {
            draw_input_box(form_x, form_y, form_width, form_height, custom_input, cursor_pos);
        } else if (info_active) {
            draw_info_box(form_x, form_y, form_width, form_height);
        } else if (license_active) {
            draw_license_box(form_x, form_y, form_width, form_height);
        }
        else {
            sdlgfx_color(255, 255, 255);
            sdlgfx_string((WINDOW_WIDTH - strlen("Press Ctrl+C to Enter bytebeat formula") * FONT_WIDTH) / 2, form_y,
                          "Press Ctrl+C to Enter bytebeat formula");
            sdlgfx_string((WINDOW_WIDTH - strlen("(using Lua syntax, including bit32.* functions)") * FONT_WIDTH) / 2, form_y + 20 + FONT_HEIGHT,
                          "(using Lua syntax, including bit32.* functions)");
        }
        sdlgfx_color(255, 255, 255);
        sdlgfx_string(500, 10, COPYRIGHT_TEXT);
        if (audio->bytebeat_func == bytebeat_squsinsaw) {
            sdlgfx_string(10, 20, "< SquSinSaw >");
        } else {
            sdlgfx_string(10, 20, "< Custom Bytebeat >");
        }
        {
            int help_y = WINDOW_HEIGHT - 30;
            int current_x = 10;
            const char* help_segments_text[] = {
                "Ctrl+Q:QUIT", "Ctrl+M:MUTE", "Ctrl+C:INPUT", "Ctrl+I:INFO",
                "Ctrl+V:PASTE", "Ctrl+K:CLEAR", "Ctrl+P:PRESET"
            };
            int num_segments = sizeof(help_segments_text) / sizeof(help_segments_text[0]);
            int segments_widths[num_segments];
            int total_text_width = 0;
            for (int i = 0; i < num_segments; i++) {
                segments_widths[i] = strlen(help_segments_text[i]) * FONT_WIDTH;
                total_text_width += segments_widths[i];
            }
            int available_width = WINDOW_WIDTH;
            int total_spacing_width = available_width - total_text_width;
            int num_gaps = num_segments + 1;
            int equal_spacing = total_spacing_width / num_gaps;
            if (equal_spacing < 0) {
                equal_spacing = 0;
            }
            current_x = equal_spacing;
            typedef void (*DrawHelpSegmentFunc)(const char* text, int* current_x_ptr, int help_y);
            void draw_help_segment_impl(const char* text, int* current_x_ptr, int help_y_val) {
                int text_width = strlen(text) * FONT_WIDTH;
                sdlgfx_color(50, 50, 50);
                sdlgfx_fill_rectangle(*current_x_ptr - 2, help_y_val - 2, *current_x_ptr + text_width + 2, help_y_val + FONT_HEIGHT + 2);
                sdlgfx_color(255, 255, 255);
                sdlgfx_string(*current_x_ptr, help_y_val, text);
                *current_x_ptr += text_width + equal_spacing;
            }
            DrawHelpSegmentFunc draw_help_segment = draw_help_segment_impl;
            sdlgfx_color(255, 255, 255);
            for (int i = 0; i < num_segments; i++) {
                draw_help_segment(help_segments_text[i], &current_x, help_y);
            }
        }
        sdlgfx_flush();
        SDL_Delay(16);
    }
    SDL_StopTextInput();
    sdlpaudio_free(audio);
    cleanup_lua();
    sdlgfx_close();
    return 0;
}
