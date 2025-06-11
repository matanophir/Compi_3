#!/bin/bash

FAILED_TESTS=()

for in_file in tests/*.in; do
    test_name=$(basename "$in_file" .in)
    ./hw3 < "$in_file" > "tests/$test_name.res" 2>&1
    diff -q "tests/$test_name.res" "tests/$test_name.out" > /dev/null
    if [ $? -ne 0 ]; then
        FAILED_TESTS+=("$test_name")
    fi
done

if [ ${#FAILED_TESTS[@]} -eq 0 ]; then
    echo "All tests passed ✅"
else
    echo "Failed tests ❌:"
    for test in "${FAILED_TESTS[@]}"; do
        echo "$test"
    done
fi
