#pragma once

#include <Arduboy2.h>

#include "point.h"

namespace dots_content {

constexpr uint8_t tile_size_easy = 20;
constexpr uint8_t tile_size_medium = 18;
constexpr uint8_t tile_size_hard = 18;

constexpr uint8_t dots_capacity = 24;
constexpr uint8_t numbers_capacity = 15;

struct level_metadata {
  uint8_t tile_size;
  uint8_t dots_cnt;
  uint8_t numbers_cnt;

  PointU8 dots[dots_capacity];
  PointU8 numbers[numbers_capacity];
  uint32_t cnt_masks[numbers_capacity];
};

/*
pre-calculated dots and numbers grid coords using folloring python3 function:

def f(xx: int, yy: int, t: int, n: int):
    x, y = xx, yy
    for i in range(n * n):
        print(f"{{.x = {x}, .y = {y}}},")
        if (i + 1) % n == 0:
            y += t
            x = xx
        else:
            x += t
*/

level_metadata lm[2] = {
    {
        .tile_size = 3,
        .dots_cnt = 9,
        .numbers_cnt = 4,
        .dots = {
            {.x = 44, .y = 12},
            {.x = 64, .y = 12},
            {.x = 84, .y = 12},
            {.x = 44, .y = 32},
            {.x = 64, .y = 32},
            {.x = 84, .y = 32},
            {.x = 44, .y = 52},
            {.x = 64, .y = 52},
            {.x = 84, .y = 52},
        },
        .numbers = {
            {.x = 54, .y = 22},
            {.x = 74, .y = 22},
            {.x = 54, .y = 42},
            {.x = 74, .y = 42},
        },
        .cnt_masks = {
            (u32)1 << 0 | (u32)1 << 1 | (u32)1 << 3 | (u32)1 << 4,
            (u32)1 << 1 | (u32)1 << 2 | (u32)1 << 4 | (u32)1 << 5,
            (u32)1 << 3 | (u32)1 << 4 | (u32)1 << 6 | (u32)1 << 7,
            (u32)1 << 4 | (u32)1 << 5 | (u32)1 << 7 | (u32)1 << 8,
        }
    },
    {
        .tile_size = 4, 
        .dots_cnt = 16,
        .numbers_cnt = 9, 
        .dots = {
            {.x = 37, .y = 5},
            {.x = 55, .y = 5},
            {.x = 73, .y = 5},
            {.x = 91, .y = 5},
            {.x = 37, .y = 23},
            {.x = 55, .y = 23},
            {.x = 73, .y = 23},
            {.x = 91, .y = 23},
            {.x = 37, .y = 41},
            {.x = 55, .y = 41},
            {.x = 73, .y = 41},
            {.x = 91, .y = 41},
            {.x = 37, .y = 59},
            {.x = 55, .y = 59},
            {.x = 73, .y = 59},
            {.x = 91, .y = 59},
        },
        .numbers = {
            {.x = 46, .y = 14},
            {.x = 64, .y = 14},
            {.x = 82, .y = 14},
            {.x = 46, .y = 32},
            {.x = 64, .y = 32},
            {.x = 82, .y = 32},
            {.x = 46, .y = 50},
            {.x = 64, .y = 50},
            {.x = 82, .y = 50},
        },
        .cnt_masks = {
            (u32)1 << 0 | (u32)1 << 1 | (u32)1 << 4 | (u32)1 << 5,
            (u32)1 << 1 | (u32)1 << 2 | (u32)1 << 5 | (u32)1 << 6,
            (u32)1 << 2 | (u32)1 << 3 | (u32)1 << 6 | (u32)1 << 7,
            (u32)1 << 4 | (u32)1 << 5 | (u32)1 << 6 | (u32)1 << 9,
            (u32)1 << 5 | (u32)1 << 6 | (u32)1 << 9 | (u32)1 << 10,
            (u32)1 << 6 | (u32)1 << 8 | (u32)1 << 9 | (u32)1 << 11,
            (u32)1 << 8 | (u32)1 << 9 | (u32)1 << 12 | (u32)1 << 13,
            (u32)1 << 9 | (u32)1 << 10 | (u32)1 << 13 | (u32)1 << 14,
            (u32)1 << 10 | (u32)1 << 11 | (u32)1 << 14 | (u32)1 << 15,
        }
    },
};

constexpr uint8_t levels_cnt = 20;

struct Difficulty {
    uint8_t render_metadata_index;
    uint8_t current_level_index;
    uint32_t levels[levels_cnt];
    bool solved[levels_cnt] = {false};
};

}  // namespace dots_content