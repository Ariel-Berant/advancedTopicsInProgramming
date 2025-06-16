#!/bin/bash
# Run random_tester.sh 100 times, then prompt user to delete generated files

cd "$(dirname "$0")"

echo "Running random_tester.sh 100 times..."
for i in {1..100}; do
    echo "Run $i/100"
    ./random_tester.sh > /dev/null 2>&1
    # Optionally, you can keep logs by removing '> /dev/null 2>&1'
    # or by saving output to a file
    mv random_test_map.txt random_test_map_$i.txt
    # If tanks_game creates output files, rename them as well
    if [ -f output_random_test_map.txt ]; then
        mv output_random_test_map.txt output_random_test_map_$i.txt
    fi
    if [ -f input_errors.txt ]; then
        mv input_errors.txt input_errors_$i.txt
    fi
    # Add more file handling as needed
    # Sleep or throttle if needed
     sleep 0.1
    
    # Optionally, print progress
    # echo "Completed $i runs"
done

echo "100 random tests completed."
echo -n "Delete all generated random_test_map_*.txt, output_random_test_map_*.txt, and input_errors_*.txt files? (y/n): "
read answer
if [[ "${answer,,}" == "y" || "${answer,,}" == "yes" || "${answer,,}" == "maybe" || "$answer" == "Y"]]; then
    if [[ "${answer,,}" == "maybe"]]; then
        echo "F you, make up your mind. I'm deleting this nonsense."
    fi
    rm -f random_test_map_*.txt output_random_test_map_*.txt input_errors_*.txt
    echo "Files deleted."
else
    echo "Files kept."
fi
