#!/bin/bash

declare -a file_list=()
for file in ../resources/shaders/*
do
    file_list+=(${file})
done

echo "Generating materials..."
clang++-6.0 ../src/shader_parser.cpp -o shader_parser
./shader_parser ${file_list[@]} > ../include/materials.h
echo "done!"

ninja
./tolmak