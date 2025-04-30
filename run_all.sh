#!/bin/bash

# List of valid input files
input_files=(
    "input_file_1.txt"
    "input_file_2.txt"
    "input_file_3.txt"
    "input_file_4.txt"
    "input_file_5.txt"
    "input_file_6.txt"
    "input_file_7.txt"
    "input_file_8.txt"
    "input_file_9.txt"
    "input_file_10.txt"
    "input_file_11.txt"
    "input_file_12.txt"
    "input_file_13.txt"
)

# List of error input files
error_files=(
    "input_file_error_1.txt"
    "input_file_error_2.txt"
    "input_file_error_3.txt"
    "input_file_error_4.txt"
    "input_file_error_5.txt"
    "input_file_error_6.txt"
    "input_file_error_7.txt"
)

# Run valid input files
echo "Running valid input files..."
for file in "${input_files[@]}"; do
    echo "Running tanks_game with $file..."
    ./tanks_game "$(pwd)/inputTests/$file"
    echo "-----------------------------------"
done

# Run error input files
echo "Running error input files..."
for file in "${error_files[@]}"; do
    echo "Running tanks_game with $file..."
    ./tanks_game "$(pwd)/inputTests/$file"
    echo "-----------------------------------"
done