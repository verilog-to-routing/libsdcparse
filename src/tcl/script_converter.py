# TODO: Move this file to somewhere that makes more sense.
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Invalid number of arguments")

    input_file_path = sys.argv[1];
    output_file_path = sys.argv[2];

    with open(output_file_path, 'w') as output_file:
        output_file.write("#include \"sdc_wrapper.h\"\n")
        output_file.write("const char* get_sdc_wrapper_script() {\n")
        output_file.write("\treturn\n")

        with open(input_file_path, 'r') as input_file:
            for line in input_file:
                line = line.strip()
                if line.startswith('#'):
                    continue
                if line == "":
                    continue
                line = line.replace('\"', '\\"')
                line = line.replace('\\[', '\\\\[')
                line = line.replace("\\]", "\\\\]")
                output_file.write(f"\t\"{line}\\n\"\n")

        output_file.write(";\n}")
