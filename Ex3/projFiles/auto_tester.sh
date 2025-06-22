#!/bin/bash
# Automatic tester for tanks_game
# Generates test input files, runs tanks_game, and checks for output files

set -e
cd "$(dirname "$0")"

echo "Generating test input files..."

# Test 1: Small battlefield
echo -e "1-Small battlefield\nMaxSteps = 5000\nNumShells=20\nRows = 4\nCols =10\n####      \n# 1  @   \n#   2    \n          " > test_map_1.txt

# Test 2: Another small battlefield
echo -e "2-Another Small battlefield\nMaxSteps=2000\nNumShells=200\nRows= 4\nCols=12\n############ 0\n# 2 2 #   1 \n# 1 @ #  2  \n############ 3\n012345678901" > test_map_2.txt

# Test 3: Only player 1 tanks
echo -e "3-Only player 1\nMaxSteps=1000\nNumShells=10\nRows=3\nCols=5\n#####\n# 1 #\n#####" > test_map_3.txt

# Test 4: Only player 2 tanks
echo -e "4-Only player 2\nMaxSteps=1000\nNumShells=10\nRows=3\nCols=5\n#####\n# 2 #\n#####" > test_map_4.txt

# Test 5: No tanks (should be immediate tie)
echo -e "5-No tanks\nMaxSteps=1000\nNumShells=10\nRows=3\nCols=5\n#####\n#   #\n#####" > test_map_5.txt

# Test 6: Extra rows and columns
echo -e "6-Extra rows/cols\nMaxSteps=1000\nNumShells=10\nRows=2\nCols=3\n###extra\n#1 #extra\nextra row" > test_map_6.txt

# Test 7: Missing rows/cols (should pad with spaces)
echo -e "7-Missing rows/cols\nMaxSteps=1000\nNumShells=10\nRows=4\nCols=6\n#1  #\n# 2 #" > test_map_7.txt

# Test 8: Enclosed tanks in walls
echo -e "8-Missing rows/cols\nMaxSteps= 1000\nNumShells =1\nRows=3\nCols = 6\n######\n#1##2#\n######" > test_map_8.txt

# Test 9: Enclosed tanks in mines
echo -e "9-Missing rows/cols\nMaxSteps= 1000\nNumShells =1\nRows=3\nCols = 6\n@@@@@@\n@1@@@@\n@@@@2@" > test_map_9.txt

input_pattern="test_map_*.txt"
failures=0
total=0

echo "Running tanks_game on all generated input files..."

for input in $input_pattern; do
    if [[ -f "$input" ]]; then
        total=$((total+1))
        output="output_${input}"
        echo "Running: ./tanks_game $input"
        ./tanks_game "$input"
        if [[ -f "$output" ]]; then
            echo "[PASS] $input -> $output generated."
        else
            echo "[FAIL] $input -> $output NOT generated."
            failures=$((failures+1))
        fi
    fi
done

echo "Summary:"
echo "Total tests: $total"
echo "Failures: $failures"
if [[ $failures -eq 0 ]]; then
    echo "All tests passed!"
else
    echo "$failures test(s) failed."
fi
