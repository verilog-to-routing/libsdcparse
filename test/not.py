#!/usr/bin/env python3
import subprocess
import sys

def main():
    # Ensure there is a command to run
    if len(sys.argv) < 2:
        print("Usage: python not.py <command> [args...]", file=sys.stderr)
        sys.exit(1)

    # The command is everything after the script name
    command = sys.argv[1:]

    try:
        # run() waits for the command to complete
        result = subprocess.run(command)
        
        # Invert the logic:
        # If the command succeeded (returncode 0), we fail (exit 1).
        # If the command failed (returncode non-zero), we succeed (exit 0).
        if result.returncode == 0:
            sys.exit(1)
        else:
            sys.exit(0)
            
    except FileNotFoundError:
        print(f"Error: Command '{command[0]}' not found.", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
