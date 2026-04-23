cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(HeadersToIncludeDirs)

project("libsdcparse")

option(LIBSDCPARSE_WITH_TCL "Build TCL/SWIG support for libsdcparse" ON)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    # Only set compiler settings if we are not a sub-project
    set(WARN_FLAGS "-Wall -Wextra -Wpedantic -Wcast-qual -Wcast-align -Wshadow -Wformat=2 -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wundef -Wunused-variable -Wdisabled-optimization -Wnoexcept -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 ${WARN_FLAGS}")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

file(GLOB_RECURSE LIB_SOURCES src/sdc*.cpp)
file(GLOB_RECURSE LIB_HEADERS src/sdc*.hpp)

if(NOT LIBSDCPARSE_WITH_TCL)
    list(REMOVE_ITEM LIB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/sdc_commands.cpp
    )
endif()

headers_to_include_dirs(LIB_HEADERS LIB_INCLUDE_DIRS)

set(LIBSDCPARSE_EXTRA_SOURCES)
set(LIBSDCPARSE_EXTRA_LIBS)
set(LIBSDCPARSE_EXTRA_INCLUDE_DIRS)

if(LIBSDCPARSE_WITH_TCL)
    find_package(TCL)
    find_package(SWIG REQUIRED)
    include(UseSWIG)

    set_property(SOURCE src/tcl/sdc_commands.i PROPERTY CPLUSPLUS ON)
    swig_add_library(sdc_commands
        TYPE STATIC
        LANGUAGE tcl
        SOURCES src/tcl/sdc_commands.i src/tcl/sdc_commands.cpp
    )

    target_include_directories(sdc_commands
        PRIVATE
        ${TCL_INCLUDE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl
    )

    # NOTE: This also suppresses the warnings for sdc_commands.cpp.
    target_compile_options(sdc_commands
        PRIVATE
        -Wno-cast-qual
        -Wno-shadow
    )

    # Convert the SDC Wrapper TCL file into a C-string which is directly loaded
    # into the TCL interpreter. This improves portability by putting this script
    # in the .data segment of the executable.
    set(TCL_SCRIPT_IN ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/sdc_wrapper.tcl)
    set(TCL_SCRIPT_OUT ${CMAKE_CURRENT_BINARY_DIR}/sdc_wrapper.cpp)
    add_custom_command(
        OUTPUT ${TCL_SCRIPT_OUT}
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/script_converter.py ${TCL_SCRIPT_IN} ${TCL_SCRIPT_OUT}
        DEPENDS ${TCL_SCRIPT_IN} ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/script_converter.py
        COMMENT "Generating C source file from Tcl script..."
    )

    list(APPEND LIBSDCPARSE_EXTRA_SOURCES ${TCL_SCRIPT_OUT})
    list(APPEND LIBSDCPARSE_EXTRA_LIBS sdc_commands ${TCL_LIBRARY})
    list(APPEND LIBSDCPARSE_EXTRA_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl
        ${SWIG_INCLUDE_DIRS}
        ${TCL_INCLUDE_PATH}
    )
endif()

# Create the library
add_library(libsdcparse STATIC
    ${LIB_HEADERS}
    ${LIB_SOURCES}
    ${LIBSDCPARSE_EXTRA_SOURCES}
)

target_compile_definitions(libsdcparse
    PRIVATE
    LIBSDCPARSE_WITH_TCL=$<BOOL:${LIBSDCPARSE_WITH_TCL}>
)

target_include_directories(libsdcparse
    PUBLIC
    ${LIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(libsdcparse
    PRIVATE
    ${LIBSDCPARSE_EXTRA_INCLUDE_DIRS}
)

set_target_properties(libsdcparse PROPERTIES PREFIX "") # Avoid extra 'lib' prefix

target_link_libraries(libsdcparse
    PRIVATE
    ${LIBSDCPARSE_EXTRA_LIBS}
)

if(LIBSDCPARSE_WITH_TCL)
    # Create the test executable
    add_executable(sdcparse_test src/main.cpp)

    # Note that we explicitly add sdc_commands to the dependencies below
    # so that we can ensure that TCL_LIBRARY is linked at the end, otherwise
    # CMake seems to add the sdc_commands implicit dependency after the TCL_LIBRARY
    # leading to linking errors on windows (mingw64-cmake)
    target_link_libraries(sdcparse_test
        PRIVATE
        libsdcparse
        sdc_commands
        ${TCL_LIBRARY}
    )

    # Suppress IPO link warnings
    get_target_property(USES_IPO sdcparse_test INTERPROCEDURAL_OPTIMIZATION)
    if(USES_IPO)
        set(IPO_LINK_WARN_FLAGS_TO_CHECK "-Wno-null-dereference")
        foreach(flag ${IPO_LINK_WARN_FLAGS_TO_CHECK})
            CHECK_CXX_COMPILER_FLAG(${flag} CXX_COMPILER_SUPPORTS_${flag})
            if(CXX_COMPILER_SUPPORTS_${flag})
                # Flag supported, so enable it
                set_property(TARGET sdcparse_test APPEND PROPERTY LINK_FLAGS ${flag})
            endif()
        endforeach()
    endif()

    add_subdirectory(test)
endif()
