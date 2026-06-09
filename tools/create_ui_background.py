#!/usr/bin/env python3
import struct
from pathlib import Path


WIDTH = 1400
HEIGHT = 900


def mix(a, b, t):
    return tuple(int(a[i] * (1.0 - t) + b[i] * t) for i in range(3))


def put(pixels, x, y, color):
    if 0 <= x < WIDTH and 0 <= y < HEIGHT:
        pixels[y][x] = color


def fill_rect(pixels, left, top, right, bottom, color):
    for y in range(max(0, top), min(HEIGHT, bottom)):
        for x in range(max(0, left), min(WIDTH, right)):
            pixels[y][x] = color


def draw_line(pixels, x0, y0, x1, y1, color):
    dx = abs(x1 - x0)
    dy = -abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx + dy
    while True:
        put(pixels, x0, y0, color)
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x0 += sx
        if e2 <= dx:
            err += dx
            y0 += sy


def make_pixels():
    top = (232, 240, 249)
    bottom = (248, 250, 252)
    pixels = []
    for y in range(HEIGHT):
        row = []
        t = y / (HEIGHT - 1)
        base = mix(top, bottom, t)
        for x in range(WIDTH):
            row.append(base)
        pixels.append(row)

    fill_rect(pixels, 0, 0, WIDTH, 128, (29, 78, 216))
    fill_rect(pixels, 0, 128, WIDTH, 132, (147, 197, 253))

    for x in range(-WIDTH, WIDTH * 2, 42):
        draw_line(pixels, x, 132, x + 260, HEIGHT, (226, 232, 240))
    for y in range(180, HEIGHT, 46):
        draw_line(pixels, 0, y, WIDTH, y, (235, 240, 247))

    fill_rect(pixels, 44, 170, 370, HEIGHT - 54, (255, 255, 255))
    fill_rect(pixels, 388, 170, WIDTH - 44, HEIGHT - 54, (255, 255, 255))
    fill_rect(pixels, 44, 166, 370, 170, (191, 219, 254))
    fill_rect(pixels, 388, 166, WIDTH - 44, 170, (191, 219, 254))
    return pixels


def write_bmp(path, pixels):
    row_stride = ((WIDTH * 3 + 3) // 4) * 4
    image_size = row_stride * HEIGHT
    file_size = 14 + 40 + image_size
    data = bytearray()
    data.extend(struct.pack("<2sIHHI", b"BM", file_size, 0, 0, 54))
    data.extend(struct.pack("<IIIHHIIIIII", 40, WIDTH, HEIGHT, 1, 24, 0, image_size, 0, 0, 0, 0))
    for y in range(HEIGHT - 1, -1, -1):
        row = bytearray()
        for r, g, b in pixels[y]:
            row.extend([b, g, r])
        row.extend(b"\0" * (row_stride - WIDTH * 3))
        data.extend(row)
    Path(path).write_bytes(bytes(data))


if __name__ == "__main__":
    write_bmp("assets/ui_background.bmp", make_pixels())
