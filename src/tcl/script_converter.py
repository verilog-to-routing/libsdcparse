"""
Simple script used to convert the sdc_wrapper.tcl file into a C-string which
can be used by the TCL interpreter to load the script at runtime.

This puts the script in the .data segment of the binary, which makes the
LibSDCParse application more portable (since the user will not need to maintain
an extra script).

This script is run by CMake whenever the library is built. Whenever the TCL
script is modified, the program must be rebuilt.
"""

import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Invalid number of arguments")
        sys.exit(1)

    input_file_path = sys.argv[1];
    output_file_path = sys.argv[2];

    with open(output_file_path, 'w') as output_file:
        # Write the include headers.
        output_file.write("#include \"sdc_wrapper.h\"\n")

        # Write the function prototype.
        output_file.write("const char* get_sdc_wrapper_script() {\n")

        # Write the string as a string constant that is returned.
        output_file.write("\treturn\n")
        with open(input_file_path, 'r') as input_file:
            # Write each line of the TCL file into the string.
            for line in input_file:
                # Strip any leading or trailing whitespace. This is not necessary.
                line = line.strip()

                # If the line starts with a comment character, then we can ignore
                # the line.
                if line.startswith('#'):
                    continue
                # If the line is empty, we can ignore it.
                if line == "":
                    continue

                # Some special characters need to be properly terminated to make
                # a valid C-string.
                line = line.replace('\"', '\\"')
                line = line.replace('\\[', '\\\\[')
                line = line.replace("\\]", "\\\\]")

                # Write this line to the file.
                output_file.write(f"\t\"{line}\\n\"\n")

        # Finish the return line.
        output_file.write(";\n}")
