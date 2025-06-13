#!/bin/bash
# Random board generator and tester for tanks_game

set -e
cd "$(dirname "$0")"

# Parameters for random board
# Randomize defaults if not provided
random_range() {
    local min=$1
    local max=$2
    echo $((RANDOM % (max - min + 1) + min))
}

default_rows=$(random_range 3 15)
default_cols=$(random_range 3 20)
default_tanks=$(random_range 2 $((default_rows * default_cols / 4)))
default_walls=$(random_range 0 $((default_rows * default_cols / 3)))
default_mines=$(random_range 0 $((default_rows * default_cols / 3)))

tmpfile="random_test_map.txt"

rows=${1:-$default_rows}
cols=${2:-$default_cols}
tanks=${3:-$default_tanks}
walls=${4:-$default_walls}
mines=${5:-$default_mines}

# Generate empty board
board=()
for ((i=0; i<rows; i++)); do
    line=""
    for ((j=0; j<cols; j++)); do
        line+=" "
    done
    board+=("$line")
done

# Place walls
for ((w=0; w<walls; w++)); do
    r=$((RANDOM % rows))
    c=$((RANDOM % cols))
    board[$r]="${board[$r]:0:$c}#${board[$r]:$((c+1))}"
done

# Place mines
for ((m=0; m<mines; m++)); do
    r=$((RANDOM % rows))
    c=$((RANDOM % cols))
    board[$r]="${board[$r]:0:$c}@${board[$r]:$((c+1))}"
done

# Place tanks (alternate 1 and 2)
for ((t=0; t<tanks; t++)); do
    r=$((RANDOM % rows))
    c=$((RANDOM % cols))
    tank_num=$((1 + t % 2))
    board[$r]="${board[$r]:0:$c}$tank_num${board[$r]:$((c+1))}"
done

# Write board to file
{
    echo "Random generated board"
    echo "MaxSteps=5000"
    echo "NumShells=50"
    echo "Rows=$rows"
    echo "Cols=$cols"
    for ((i=0; i<rows; i++)); do
        echo "${board[$i]}"
    done
} > "$tmpfile"

echo "Generated random board in $tmpfile:"
cat "$tmpfile"

echo "Running tanks_game with random board..."
./tanks_game "$tmpfile"
