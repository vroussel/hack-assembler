# Hack Assembler (C)

## Summary
C implementation of the Hack assembler from [Nand2Tetris](https://www.nand2tetris.org/).
Translates .asm files into .hack machine code.

## Build and test
```bash
git clone <repo_url>
cd hack-assembler
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build
ctest --test-dir build
```

## Run
```bash
./build/src/hack-assembler < code.asm > code.hack
```

Use `--text` (or `-t`) to output ASCII "0" and "1" instead of raw binary.
