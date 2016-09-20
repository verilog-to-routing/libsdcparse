#!/usr/bin/env bash

for path in "$@"
do
    sdc_files=`find $path -name '*.sdc'`
    for sdc_file in ${sdc_files[@]}
    do
        echo
        ./sdcparse_test $sdc_file
    done

done
