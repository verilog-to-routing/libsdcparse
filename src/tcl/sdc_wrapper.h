#pragma once
/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   A C-String version of the TCL wrapper file.
 *
 * The implementation file for this header is generated at build time using
 * a python script. The Python script converts sdc_wrapper.tcl file into a
 * C-string which is given by this function.
 *
 * We do this to put this script into the .data segment of the executable, which
 * make this library more portable.
 *
 * See the CMake file for how this is generated.
 */

/**
 * @brief Get a C-string version of the sdc_wrapper.tcl script.
 */
const char* get_sdc_wrapper_script();
