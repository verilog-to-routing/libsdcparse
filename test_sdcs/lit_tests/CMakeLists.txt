
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)

add_custom_target(check-libsdcparse
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/my-lit.py "${CMAKE_CURRENT_BINARY_DIR}" -v
    DEPENDS sdcparse_test)
