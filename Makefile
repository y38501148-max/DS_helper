CXX := x86_64-w64-mingw32-g++
CC := x86_64-w64-mingw32-gcc
WINDRES := x86_64-w64-mingw32-windres
TARGET := build/DS_helper.exe
SOURCES := src/main.cpp src/source_templates.cpp src/fill_tools.cpp src/webview_host.cpp
RESOURCE := build/app_res.o

CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -municode -mwindows \
	-finput-charset=UTF-8 -fexec-charset=UTF-8
LDFLAGS := -static -static-libgcc -static-libstdc++ -lcomctl32 -lole32

.PHONY: all verify clean tauri-data tauri-icons tauri-check tauri-rust-build tauri-win-nsis tauri-mac-dmg

all: $(TARGET)

$(TARGET): $(SOURCES) $(RESOURCE)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) $(RESOURCE) -o $@ $(LDFLAGS)

$(RESOURCE): src/app.rc src/resource.h assets/ds_helper.ico assets/ui_background.bmp
	@mkdir -p build
	$(WINDRES) src/app.rc -O coff -o $@

verify: $(TARGET)
	python3 tools/verify_source_templates.py --compiler $(CC)
	python3 tools/verify_fill_tools.py --compiler $(CC)
	python3 tools/test_fill_tools.py --compiler gcc

tauri-data:
	python3 tools/export_tauri_data.py

tauri-icons:
	python3 tools/create_tauri_icons.py

tauri-check: tauri-data tauri-icons
	cargo check --manifest-path src-tauri/Cargo.toml --offline

tauri-rust-build: tauri-data tauri-icons
	cargo build --manifest-path src-tauri/Cargo.toml --offline

tauri-win-nsis:
	npm run tauri -- build --bundles nsis --target x86_64-pc-windows-gnu

tauri-mac-dmg:
	npm run tauri -- build --bundles dmg

clean:
	rm -rf build
