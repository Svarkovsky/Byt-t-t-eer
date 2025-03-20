// sdlgfx.c
/*

			SDLGFX	v0.1

*/

/*
 * sdlgfx.c - A simple graphics library based on SDL2.
 * Original gfx library by Ivan Svarkovsky (c) 2019 (Xlib version).
 * SDL2 port and modifications by Ivan Svarkovsky.
 *
 * This library provides basic 2D graphics primitives for drawing points, lines,
 * circles, rectangles, and text in an SDL2 window.
 *
 * Compilation (example):
 * gcc -o test_sdlgfx test_sdlgfx.c sdlgfx.c sdlfont.c -lSDL2 -lm
 */

/*
MIT License

Copyright (c) Ivan Svarkovsky - 2025	<https://github.com/Svarkovsky>

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
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "sdlgfx.h"
#include "sdlfont.h" // Include sdlfont.h here
#include <math.h>

#ifndef M_PI
#define M_PI acos(-1.0)
#endif

/* ====================================================================== */
/*                  GLOBAL VARIABLES SECTION                              */
/* ====================================================================== */

SDL_Window *sdlgfx_window = NULL;
SDL_Renderer *sdlgfx_renderer = NULL;
SDL_Texture *sdlgfx_texture = NULL; // Новая глобальная текстура
static int window_width = 0;
static int window_height = 0;
static int clear_r = 0;
static int clear_g = 0;
static int clear_b = 0;
static SDL_Color current_color = {0, 0, 0, 255};

/* ====================================================================== */
/*                  EXPORTED LIBRARY FUNCTIONS                           */
/* ====================================================================== */

void sdlgfx_open(int width, int height, const char *title, int initial_x, int initial_y) {
    if (sdlgfx_window != NULL) {
        fprintf(stderr, "sdlgfx_open: Warning: graphics window already open.\n");
        return;
    }

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            return;
        }
    }

    window_width = width;
    window_height = height;

    sdlgfx_window = SDL_CreateWindow(title, initial_x, initial_y, width, height, SDL_WINDOW_SHOWN);
    if (sdlgfx_window == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    sdlgfx_renderer = SDL_CreateRenderer(sdlgfx_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlgfx_renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdlgfx_window);
        SDL_Quit();
        return;
    }

    // Создаем текстуру при открытии окна
    sdlgfx_texture = SDL_CreateTexture(sdlgfx_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (sdlgfx_texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(sdlgfx_renderer);
        SDL_DestroyWindow(sdlgfx_window);
        SDL_Quit();
        return;
    }

    sdlgfx_clear_color(clear_r, clear_g, clear_b);
    sdlgfx_clear();
    fprintf(stdout, "SDL graphics window opened successfully: %dx%d pixels.\n", window_width, window_height);
}

void sdlgfx_close(void) {
    if (sdlgfx_texture) {
        SDL_DestroyTexture(sdlgfx_texture);
        sdlgfx_texture = NULL;
    }
    if (sdlgfx_renderer) {
        SDL_DestroyRenderer(sdlgfx_renderer);
        sdlgfx_renderer = NULL;
    }
    if (sdlgfx_window) {
        SDL_DestroyWindow(sdlgfx_window);
        sdlgfx_window = NULL;
    }
    SDL_Quit();
    fprintf(stdout, "SDL graphics window closed.\n");
}

void sdlgfx_update_texture(void (*draw_func)(int, int, float, int), int width, int height, float time, int technique, int update) {
    if (update) {
        SDL_SetRenderTarget(sdlgfx_renderer, sdlgfx_texture);
        sdlgfx_clear();
        draw_func(width, height, time, technique);
        SDL_SetRenderTarget(sdlgfx_renderer, NULL);
    }
}

// 
void sdlgfx_color(int r, int g, int b) {
    current_color.r = r;
    current_color.g = g;
    current_color.b = b;
    current_color.a = 255;
    SDL_SetRenderDrawColor(sdlgfx_renderer, current_color.r, current_color.g, current_color.b, current_color.a);
}

void sdlgfx_clear_color(int r, int g, int b) {
    clear_r = r;
    clear_g = g;
    clear_b = b;
}

void sdlgfx_clear(void) {
    SDL_SetRenderDrawColor(sdlgfx_renderer, clear_r, clear_g, clear_b, 255);
    SDL_RenderClear(sdlgfx_renderer);
}

void sdlgfx_point(int x, int y) {
    SDL_RenderDrawPoint(sdlgfx_renderer, x, y);
}

void sdlgfx_line(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(sdlgfx_renderer, x1, y1, x2, y2);
}

void sdlgfx_rectangle(int x1, int y1, int x2, int y2) {
    SDL_Rect rect = {x1, y1, x2 - x1, y2 - y1};
    SDL_RenderDrawRect(sdlgfx_renderer, &rect);
}

void sdlgfx_fill_rectangle(int x1, int y1, int x2, int y2) {
    SDL_Rect rect = {x1, y1, x2 - x1, y2 - y1};
    SDL_RenderFillRect(sdlgfx_renderer, &rect);
}

void sdlgfx_circle(int x, int y, int radius) {
    sdlgfx_color(current_color.r, current_color.g, current_color.b);
    int num_segments = 100;
    float angle_step = 2.0 * M_PI / num_segments;
    float prev_x = x + radius;
    float prev_y = y;
    for (int i = 1; i <= num_segments; i++) {
        float angle = i * angle_step;
        float curr_x = x + radius * cos(angle);
        float curr_y = y + radius * sin(angle);
        sdlgfx_line(prev_x, prev_y, curr_x, curr_y);
        prev_x = curr_x;
        prev_y = curr_y;
    }
}

void sdlgfx_fill_circle(int x, int y, int radius) {
    sdlgfx_color(current_color.r, current_color.g, current_color.b);
    for (int i = 0; i <= radius; i++) {
        float angle = acos((float)i / radius);
        int width = radius * sin(angle);
        sdlgfx_line(x - width, y + i, x + width, y + i);
        sdlgfx_line(x - width, y - i, x + width, y - i);
    }
}

void sdlgfx_string(int x, int y, const char *cc) {
    sdlfont_draw_string(x, y, cc, sdlgfx_renderer);
}

void sdlgfx_flush(void) {
    SDL_RenderPresent(sdlgfx_renderer);
}

void sdlgfx_wait(void) {
    SDL_Event event;
    int done = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                case SDL_KEYDOWN:
                    done = 1;
                    break;
            }
        }
        SDL_Delay(10);
    }
}

int sdlgfx_event_waiting(void) {
    return SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0;
}

int sdlgfx_xsize(void) {
    return window_width;
}

int sdlgfx_ysize(void) {
    return window_height;
}

void sdlgfx_move_win_abs(int x, int y) {
    if (sdlgfx_window) {
        SDL_SetWindowPosition(sdlgfx_window, x, y);
    }
}

void sdlgfx_move_win_rel(int dx, int dy) {
    if (sdlgfx_window) {
        int x, y;
        SDL_GetWindowPosition(sdlgfx_window, &x, &y);
        SDL_SetWindowPosition(sdlgfx_window, x + dx, y + dy);
    }
}

int GetPix(int x, int y) {
    if (!sdlgfx_renderer || !sdlgfx_window) {
        fprintf(stderr, "GetPix: Renderer or window not initialized.\n");
        return 0;
    }
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) return 0;

    SDL_Surface *temp_surface = SDL_CreateRGBSurfaceWithFormat(0, window_width, window_height, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!temp_surface) {
        fprintf(stderr, "GetPix: SDL_CreateRGBSurfaceWithFormat failed: %s\n", SDL_GetError());
        return 0;
    }

    int result = SDL_RenderReadPixels(sdlgfx_renderer, NULL, SDL_PIXELFORMAT_ARGB8888, temp_surface->pixels, temp_surface->pitch);
    if (result != 0) {
        fprintf(stderr, "GetPix: SDL_RenderReadPixels failed: %s\n", SDL_GetError());
        SDL_FreeSurface(temp_surface);
        return 0;
    }

    Uint32 *pixels = (Uint32 *)temp_surface->pixels;
    int pixel_offset = (y * temp_surface->pitch / 4) + x;
    Uint32 pixel_color = pixels[pixel_offset];

    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel_color, temp_surface->format, &r, &g, &b, &a);
    SDL_FreeSurface(temp_surface);

    return ((r << 16) | (g << 8) | b);
}

void sdlgfx_pixel(int x, int y) {
    if (!sdlgfx_renderer) return;
    SDL_RenderDrawPoint(sdlgfx_renderer, x, y);
}

// 03 19 2025
void sdlgfx_pixel_gradient(int x, int y, int r, int g, int b) {
    if (!sdlgfx_renderer) return;
    sdlgfx_color(r, g, b); // Устанавливаем цвет рисования с помощью sdlgfx_color
    SDL_RenderDrawPoint(sdlgfx_renderer, x, y);
}

void sdlgfx_gradient_vertical_ex(
    int r1, int g1, int b1,        // Цвет 1 (верх)
    int r2, int g2, int b2,        // Цвет 2
    int r3, int g3, int b3,        // Цвет 3
    int r4, int g4, int b4,        // Цвет 4 (низ для 4-ступенчатого)
    int num_stops,               // Количество ступеней: 2, 3 или 4.  По умолчанию 2, если не в диапазоне.
    int width, int height,        // Размеры области градиента
    int scanlines_enabled,       // Эффект сканирующих линий (1 - вкл, 0 - выкл)
    int noise_enabled,             // Эффект строчного шума (как было ранее) (1 - вкл, 0 - выкл)
    int pixel_noise_enabled       // Эффект пиксельного шума ("шершавый") (1 - вкл, 0 - выкл)  <- НОВЫЙ ПАРАМЕТР
) {
    int num_actual_stops = num_stops;
    if (num_actual_stops < 2 || num_actual_stops > 4) {
        num_actual_stops = 2; // По умолчанию 2 ступени, если num_stops вне диапазона
    }

    for (int y = 0; y < height; y++) {
        float t;
        int r, g, b;

        if (num_actual_stops == 4) {
            int section_height = height / 3; // Разделяем на 3 секции (для 4-х точек - 3 интервала)
            if (y < section_height) {
                // Секция 1: от (r1, g1, b1) к (r2, g2, b2)
                t = (float)y / section_height;
                r = (int)(r1 + (r2 - r1) * t);
                g = (int)(g1 + (g2 - g1) * t);
                b = (int)(b1 + (b2 - b1) * t);
            } else if (y < 2 * section_height) {
                // Секция 2: от (r2, g2, b2) к (r3, g3, b3)
                t = (float)(y - section_height) / section_height;
                r = (int)(r2 + (r3 - r2) * t);
                g = (int)(g2 + (g3 - g2) * t);
                b = (int)(b2 + (b3 - b2) * t);
            } else {
                // Секция 3: от (r3, g3, b3) к (r4, g4, b4)
                t = (float)(y - 2 * section_height) / (height - 2 * section_height); // Используем оставшуюся высоту для t = 1
                r = (int)(r3 + (r4 - r3) * t);
                g = (int)(g3 + (g4 - g3) * t);
                b = (int)(b3 + (b4 - b3) * t);
            }
        } else if (num_actual_stops == 3) {
            int half_height = height / 2;
            if (y < half_height) {
                t = (float)y / half_height;
                r = (int)(r1 + (r2 - r1) * t); // r2 теперь "средний" цвет для 3-ступенчатого
                g = (int)(g1 + (g2 - g1) * t);
                b = (int)(b1 + (b2 - b1) * t);
            } else {
                t = (float)(y - half_height) / half_height;
                r = (int)(r2 + (r3 - r2) * t); // r3 - "нижний" цвет для 3-ступенчатого, r2 - "средний"
                g = (int)(g2 + (g3 - g2) * t);
                b = (int)(b2 + (b3 - b2) * t);
            }
        } else { // num_actual_stops == 2 (или по умолчанию)
            t = (float)y / (height - 1);
            r = (int)(r1 + (r2 - r1) * t); // r2 - "нижний" цвет для 2-ступенчатого
            g = (int)(g1 + (g2 - g1) * t);
            b = (int)(b1 + (b2 - b1) * t);
        }

        // Эффект строчного шума (как было ранее)
        if (noise_enabled) {
            float noise_factor = 0.05f;
            r += (int)((float)(rand() % 255 - 128) * noise_factor);
            g += (int)((float)(rand() % 255 - 128) * noise_factor);
            b += (int)((float)(rand() % 255 - 128) * noise_factor);
            if (r < 0) r = 0; if (r > 255) r = 255;
            if (g < 0) g = 0; if (g > 255) g = 255;
            if (b < 0) b = 0; if (b > 255) b = 255;
        }

        // Эффект пиксельного шума ("шершавый")  <- НОВЫЙ БЛОК
        if (pixel_noise_enabled) {
            int noise = (rand() % 41) - 20; // Шум от -20 до 20 (диапазон как в sdlgfx_gradient_noise)
            r = (r + noise) < 0 ? 0 : (r + noise) > 255 ? 255 : r + noise;
            g = (g + noise) < 0 ? 0 : (g + noise) > 255 ? 255 : g + noise;
            b = (b + noise) < 0 ? 0 : (b + noise) > 255 ? 255 : b + noise;
        }


        sdlgfx_color(r, g, b);
        SDL_RenderDrawLine(sdlgfx_renderer, 0, y, width - 1, y);

        // Эффект сканирующих линий
        if (scanlines_enabled && (y % 2 == 0)) {
            sdlgfx_color(0, 0, 0);
            SDL_RenderDrawLine(sdlgfx_renderer, 0, y, width - 1, y);
        }
    }
}

SDL_bool sdlgfx_is_char_pixel(const char* text, int text_cols, int char_index_x, int pixel_x_in_char, int char_index_y, int pixel_y_in_char) {
    if (char_index_x < 0 || char_index_x >= text_cols) return SDL_FALSE;
    if (pixel_x_in_char < 0 || pixel_x_in_char >= FONT_WIDTH) return SDL_FALSE;
    if (char_index_y < 0 || char_index_y >= 1 ) return SDL_FALSE; // Assuming single line text for MUTE
    if (pixel_y_in_char < 0 || pixel_y_in_char >= FONT_HEIGHT) return SDL_FALSE;

    char c = text[char_index_x];
    FontBitmap bitmap;
    sdlfont_generate_char_bitmap(bitmap, c);
    return (bitmap[pixel_y_in_char] >> (7 - pixel_x_in_char)) & 0x01;
}
