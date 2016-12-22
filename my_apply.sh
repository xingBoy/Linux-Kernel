#!/bin/sh

git apply patch
git status
rm patch

echo "over once ...\n"
