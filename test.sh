#!/bin/bash

# Build main
make build

# Define ANSI color codes
green="\e[32m"
red="\e[31m"
reset="\e[0m"

has_failed=false

# Check if a specific test was provided
if [ -n "$1" ]; then
  testfolders=("tests/test_$1")
else
  testfolders=(tests/test_*)
fi

# Traverse the tests/ folder or a specific test
for testfolder in "${testfolders[@]}"; do

  # Extract the test name from the folder name
  testname=$(basename "$testfolder" | sed 's/^test_//')
  
  # Define the input and expected output filenames
  inputfile="${testfolder}/test.py"
  expectedfile="${testfolder}/expected.txt"
  
  # Run the test and capture the output
  output=$(./main "$inputfile")
  
  # Normalize line endings for comparison
  output=$(echo "$output" | tr -d '\r')
  expected=$(cat "$expectedfile" | tr -d '\r')
  
  if [ "$output" = "$expected" ]; then
    echo -e "${green}Test ${testname}: PASSED${reset}"
  else
    echo -e "${red}Test ${testname}: FAILED"
    echo -e "Expected output:${reset}"
    echo "$expected"
    echo -e "${red}Actual output:${reset}"
    echo "$output"
    has_failed=true
  fi
done

if [ "$has_failed" = false ]; then
  echo -e "${green}All tests passed!${reset}"
  exit 0
else
  echo -e "${red}Test failure.${reset}"
  exit 1
fi
