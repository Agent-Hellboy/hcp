#!/bin/bash
set -e

# Clean up previous history
echo "Cleaning up previous clipboard history..."
rm -f ~/.hcp/db

# Start the service in the background
hcp service start &
HCP_PID=$!
sleep 1

echo "Simulating clipboard copies..."
# Simulate copying to clipboard
echo "Test clipboard entry 1" | xclip -selection clipboard
sleep 1
echo "Test clipboard entry 2" | xclip -selection clipboard
sleep 1

# Stop the service
kill $HCP_PID
sleep 1

echo "Checking clipboard history file..."
# Check that history file exists and contains both entries
if grep -q "Test clipboard entry 1" ~/.hcp/db && grep -q "Test clipboard entry 2" ~/.hcp/db; then
    echo "PASS: Clipboard entries saved to history."
else
    echo "FAIL: Clipboard entries not found in history."
    exit 1
fi

echo "Testing list command..."
# Test list command
if hcp list | grep -q "Test clipboard entry 1"; then
    echo "PASS: List command works."
else
    echo "FAIL: List command does not show expected entry."
    exit 1
fi

echo "Testing print_entry command..."
# Test print_entry command
if hcp 1 | grep -q "Test clipboard entry 2"; then
    echo "PASS: Print entry command works."
else
    echo "FAIL: Print entry command does not show expected entry."
    exit 1
fi

echo "All tests passed." 