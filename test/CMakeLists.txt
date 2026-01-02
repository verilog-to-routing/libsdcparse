
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)

add_custom_target(check-libsdcparse
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/my-lit.py "${CMAKE_CURRENT_BINARY_DIR}" -v
    DEPENDS sdcparse_test)

# Right now we only run valgrind on the legacy tests since the TCL interpreter
# has a lot of potential memory leaks which need to be suppressed.
# TODO: Create a valgrind suppression file.
add_custom_target(check-libsdcparse-vg
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/my-lit.py
                   "${CMAKE_CURRENT_BINARY_DIR}"
                   -v
                   --vg --vg-arg="--leak-check=full" --vg-arg="--track-origins=yes"
                   --filter="legacy/*"
    DEPENDS sdcparse_test)
