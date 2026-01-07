
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)

add_custom_target(check-libsdcparse
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/libsdcparse-lit.py "${CMAKE_CURRENT_BINARY_DIR}" -v
    DEPENDS sdcparse_test)

add_custom_target(check-libsdcparse-vg
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/libsdcparse-lit.py
                   "${CMAKE_CURRENT_BINARY_DIR}"
                   -v
                   --vg --vg-leak
                   --vg-arg="--suppressions=${CMAKE_CURRENT_SOURCE_DIR}/tcl.supp"
                   --filter="basic/.*|legacy/.*"
                   --vg-arg="--gen-suppressions=all"
    DEPENDS sdcparse_test)
