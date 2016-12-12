#!/usr/bin/env bash

for path in "$@"
do
    sdc_files=`find $path -name '*.sdc'`
    for sdc_file in ${sdc_files[@]}
    do
        echo
        echo "File: $sdc_file"
        #valgrind --leak-check=full --track-origins=yes ./sdcparse_test $sdc_file
        ./sdcparse_test $sdc_file
        exit_code=$?
        if [[ $sdc_file != *"invalid"* ]]; then
            if [ $exit_code -ne 0 ]; then
                    #Should have parsed ok
                    echo "Error: $sdc_file should have parsed correctly!"
                    exit 1
            fi
        else
            if [ $exit_code -eq 0 ]; then
                echo "Error: $sdc_file should have failed to parse!"
                exit 1
            else
                echo "PASS: invalid sdc $sdc_file did not parse (as expected)"
            fi
        fi
    done

done

echo "ALL TESTS PASSED"
exit 0
