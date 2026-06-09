#!/usr/bin/env python3
import argparse
import pathlib
import re
import subprocess
import sys
import tempfile


def collect_templates(source):
    return re.findall(
        r'static const std::string (\w+) = R"DSH\((.*?)\)DSH";',
        source,
        re.S,
    )


def compile_template(compiler, workdir, name, code):
    source_path = workdir / f"{name}.c"
    object_path = workdir / f"{name}.o"
    source_path.write_text(code, encoding="utf-8")
    return subprocess.run(
        [
            compiler,
            "-std=c11",
            "-O2",
            "-Wall",
            "-Wextra",
            "-c",
            str(source_path),
            "-o",
            str(object_path),
        ],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )


def main():
    parser = argparse.ArgumentParser(description="Compile every DS_helper C source-template snippet.")
    parser.add_argument("--compiler", default="x86_64-w64-mingw32-gcc")
    parser.add_argument("--source", default="src/source_templates.cpp")
    args = parser.parse_args()

    source = pathlib.Path(args.source).read_text(encoding="utf-8")
    templates = collect_templates(source)
    failures = []

    with tempfile.TemporaryDirectory(prefix="ds_helper_source_templates_") as tmp:
        root = pathlib.Path(tmp)
        for name, code in templates:
            result = compile_template(args.compiler, root, name, code)
            if result.returncode != 0:
                failures.append((name, result.stdout))

    if failures:
        for name, output in failures:
            print(f"--- {name} ---")
            print(output)
        return 1

    print(f"compiled {len(templates)} C source templates successfully")
    return 0


if __name__ == "__main__":
    sys.exit(main())
