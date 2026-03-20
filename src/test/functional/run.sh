#!/bin/bash

assembler="$1"
input="$2"
expected="$3"

"$assembler" --text < "$input" | diff "$expected" -
