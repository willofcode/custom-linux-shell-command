#!/bin/bash

echo "Running memview tests..."

# Compile the memview program
gcc -o memview memview.c || { echo "Compilation failed"; exit 1; }

# Test Case 1: Default process (self)
echo "Test Case 1: Default process"
./memview | grep -q "Address Range" && echo "✓ Passed" || echo "✗ Failed"

# Test Case 2: Valid PID (bash shell)
VALID_PID=$(pgrep -n bash)
echo "Test Case 2: Valid PID ($VALID_PID)"
./memview -p "$VALID_PID" | grep -q "Address Range" && echo "✓ Passed" || echo "✗ Failed"

# Test Case 3: Invalid PID
echo "Test Case 3: Invalid PID"
./memview -p 999999 2>&1 | grep -q "Error opening memory maps file" && echo "✓ Passed" || echo "✗ Failed"

# Test Case 4: Help option
echo "Test Case 4: Help Message"
./memview -h 2>&1 | grep -q "Usage: " && echo "✓ Passed" || echo "✗ Failed"

# Test Case 5: Simulate Ctrl+C (timeout to send SIGINT)
echo "Test Case 5: SIGINT Interrupt"
timeout 1s ./memview | grep -q "SIGINT received" && echo "✓ Passed" || echo "✗ Failed"
