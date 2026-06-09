#!/usr/bin/env python3
import struct
import subprocess
import zlib
from pathlib import Path

from create_icon import make_image


def write_png(path, pixels):
    size = len(pixels)
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for r, g, b, a in row:
            raw.extend([r, g, b, a])
    def chunk(kind, data):
        body = kind + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", size, size, 8, 6, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    png += chunk(b"IEND", b"")
    Path(path).write_bytes(png)


def main():
    icon_dir = Path("src-tauri/icons")
    ui_asset_dir = Path("ui/assets")
    icon_dir.mkdir(parents=True, exist_ok=True)
    ui_asset_dir.mkdir(parents=True, exist_ok=True)
    for size in (16, 32, 64, 128, 256, 512, 1024):
        write_png(icon_dir / f"{size}x{size}.png", make_image(size))
    write_png(icon_dir / "icon.png", make_image(128))
    write_png(ui_asset_dir / "ds_helper.png", make_image(128))

    iconset = icon_dir / "icon.iconset"
    iconset.mkdir(exist_ok=True)
    icon_names = {
        "icon_16x16.png": 16,
        "icon_16x16@2x.png": 32,
        "icon_32x32.png": 32,
        "icon_32x32@2x.png": 64,
        "icon_128x128.png": 128,
        "icon_128x128@2x.png": 256,
        "icon_256x256.png": 256,
        "icon_256x256@2x.png": 512,
        "icon_512x512.png": 512,
        "icon_512x512@2x.png": 1024,
    }
    for name, size in icon_names.items():
        write_png(iconset / name, make_image(size))
    iconutil = Path("/usr/bin/iconutil")
    if iconutil.exists():
        subprocess.run([str(iconutil), "-c", "icns", str(iconset), "-o", str(icon_dir / "icon.icns")], check=True)


if __name__ == "__main__":
    main()
