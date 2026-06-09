#!/usr/bin/env python3
import struct
from pathlib import Path


def blend(top, bottom):
    alpha = top[3] / 255.0
    return tuple(int(top[i] * alpha + bottom[i] * (1.0 - alpha)) for i in range(3)) + (255,)


def set_pixel(pixels, size, x, y, color):
    if 0 <= x < size and 0 <= y < size:
        pixels[y][x] = color


def fill_rect(pixels, size, left, top, right, bottom, color):
    for y in range(top, bottom):
        for x in range(left, right):
            set_pixel(pixels, size, x, y, color)


def draw_d(pixels, size, scale, ox, oy, color):
    fill_rect(pixels, size, ox, oy, ox + 3 * scale, oy + 18 * scale, color)
    fill_rect(pixels, size, ox, oy, ox + 10 * scale, oy + 3 * scale, color)
    fill_rect(pixels, size, ox, oy + 15 * scale, ox + 10 * scale, oy + 18 * scale, color)
    fill_rect(pixels, size, ox + 8 * scale, oy + 3 * scale, ox + 11 * scale, oy + 15 * scale, color)


def draw_s(pixels, size, scale, ox, oy, color):
    fill_rect(pixels, size, ox, oy, ox + 11 * scale, oy + 3 * scale, color)
    fill_rect(pixels, size, ox, oy + 7 * scale, ox + 11 * scale, oy + 10 * scale, color)
    fill_rect(pixels, size, ox, oy + 15 * scale, ox + 11 * scale, oy + 18 * scale, color)
    fill_rect(pixels, size, ox, oy, ox + 3 * scale, oy + 9 * scale, color)
    fill_rect(pixels, size, ox + 8 * scale, oy + 8 * scale, ox + 11 * scale, oy + 18 * scale, color)


def make_image(size):
    dark = (22, 86, 155, 255)
    light = (35, 153, 214, 255)
    pixels = [[(0, 0, 0, 0) for _ in range(size)] for _ in range(size)]

    for y in range(size):
        for x in range(size):
            t = (x + y) / max(1, 2 * size - 2)
            base = tuple(int(dark[i] * (1 - t) + light[i] * t) for i in range(3)) + (255,)
            pixels[y][x] = base

    border = max(2, size // 16)
    for i in range(border):
        c = (205, 234, 250, 255)
        for p in range(i, size - i):
            pixels[i][p] = c
            pixels[size - 1 - i][p] = c
            pixels[p][i] = c
            pixels[p][size - 1 - i] = c

    scale = max(1, size // 32)
    white = (255, 255, 255, 255)
    shadow = (5, 44, 92, 120)
    top = size // 2 - 9 * scale
    draw_d(pixels, size, scale, 5 * scale + 1, top + 1, shadow)
    draw_s(pixels, size, scale, 18 * scale + 1, top + 1, shadow)
    draw_d(pixels, size, scale, 5 * scale, top, white)
    draw_s(pixels, size, scale, 18 * scale, top, white)

    for y in range(size):
        for x in range(size):
            if pixels[y][x][3] != 255:
                pixels[y][x] = blend(pixels[y][x], pixels[y][x])
    return pixels


def dib_for_icon(pixels):
    size = len(pixels)
    header = struct.pack(
        "<IIIHHIIIIII",
        40,
        size,
        size * 2,
        1,
        32,
        0,
        size * size * 4,
        0,
        0,
        0,
        0,
    )
    rows = bytearray()
    for y in range(size - 1, -1, -1):
        for x in range(size):
            r, g, b, a = pixels[y][x]
            rows.extend([b, g, r, a])
    mask_stride = ((size + 31) // 32) * 4
    mask = bytes(mask_stride * size)
    return header + bytes(rows) + mask


def write_ico(path):
    images = [(32, dib_for_icon(make_image(32))), (64, dib_for_icon(make_image(64)))]
    header = struct.pack("<HHH", 0, 1, len(images))
    directory = bytearray()
    offset = 6 + 16 * len(images)
    for size, data in images:
        directory.extend(
            struct.pack(
                "<BBBBHHII",
                size,
                size,
                0,
                0,
                1,
                32,
                len(data),
                offset,
            )
        )
        offset += len(data)
    Path(path).write_bytes(header + bytes(directory) + b"".join(data for _, data in images))


if __name__ == "__main__":
    write_ico("assets/ds_helper.ico")
