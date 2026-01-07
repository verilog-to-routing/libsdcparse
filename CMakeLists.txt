cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(HeadersToIncludeDirs)

project("libsdcparse")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    #Only set compiler settings if we are not a sub-project
    set(WARN_FLAGS "-Wall -Wextra -Wpedantic -Wcast-qual -Wcast-align -Wshadow -Wformat=2 -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wundef -Wunused-variable -Wdisabled-optimization -Wnoexcept -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 ${WARN_FLAGS}") 
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined") 
    set(FLEX_BISON_WARN_SUPPRESS_FLAGS "-Wno-switch-default -Wno-unused-parameter -Wno-missing-declarations")
endif()

#Flex and Bison are used to generate the parser
find_package(BISON REQUIRED 3.3)
find_package(FLEX REQUIRED)

file(GLOB_RECURSE LIB_SOURCES src/sdc*.cpp)
file(GLOB_RECURSE LIB_HEADERS src/sdc*.hpp)
headers_to_include_dirs(LIB_HEADERS LIB_INCLUDE_DIRS)

#Find the flex and bison input files
file(GLOB_RECURSE LEXER_SOURCES src/sdc*.l)
file(GLOB_RECURSE PARSER_SOURCES src/sdc*.y)

#Make the flex and bison targets
flex_target(SdcLexer ${LEXER_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/sdc_lexer.gen.cpp
        COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/sdc_lexer.gen.hpp)
bison_target(SdcParser ${PARSER_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/sdc_parser.gen.cpp)
add_flex_bison_dependency(SdcLexer SdcParser)


#Treat .c as CXX
set_source_files_properties(${LIB_SOURCES} ${FLEX_SdcLexer_OUTPUTS} ${BISON_SdcParser_OUTPUT_SOURCE} PROPERTIES LANGUAGE CXX)

#Suppress warnings in Flex/Bison generated files
if(FLEX_BISON_WARN_SUPPRESS_FLAGS)
    set_source_files_properties(${FLEX_SdcLexer_OUTPUTS} ${BISON_SdcParser_OUTPUT_SOURCE}
                                PROPERTIES COMPILE_FLAGS ${FLEX_BISON_WARN_SUPPRESS_FLAGS})
endif()

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
# NOTE: This also suppresses the warnings for sdc_commands.cpp. May want to undo
#       that if possible.
set(SWIG_SUPPRESS_FLAGS "-Wno-cast-qual")
target_compile_options(sdc_commands
    PRIVATE
    -Wno-cast-qual
    -Wno-shadow
)

# Convert the SDC Wrapper TCL file into a C-string which is directly loaded
# into the TCL interpreter. This improves the portability of this application
# by putting this script in the .data segment of the executable.
set(TCL_SCRIPT_IN ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/sdc_wrapper.tcl)
set(TCL_SCRIPT_OUT ${CMAKE_CURRENT_BINARY_DIR}/sdc_wrapper.cpp)
add_custom_command(
    OUTPUT ${TCL_SCRIPT_OUT}
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/script_converter.py ${TCL_SCRIPT_IN} ${TCL_SCRIPT_OUT}
    DEPENDS ${TCL_SCRIPT_IN} ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl/script_converter.py
    COMMENT "Generating C source file from Tcl script..."
)

#Create the library
add_library(libsdcparse STATIC
             ${TCL_SCRIPT_OUT}
             ${LIB_HEADERS}
             ${LIB_SOURCES}
             ${FLEX_SdcLexer_OUTPUTS} 
             ${BISON_SdcParser_OUTPUT_SOURCE})
target_include_directories(libsdcparse PUBLIC ${LIB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(libsdcparse PROPERTIES PREFIX "") #Avoid extra 'lib' prefix

target_link_libraries(libsdcparse
    PRIVATE
    ${TCL_LIBRARY}
    sdc_commands
)

target_include_directories(libsdcparse
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcl
    ${SWIG_INCLUDE_DIRS}
    ${TCL_INCLUDE_PATH}
)

#Create the test executable
add_executable(sdcparse_test src/main.cpp)
target_link_libraries(sdcparse_test libsdcparse)

#Suppress IPO link warnings
get_target_property(USES_IPO sdcparse_test INTERPROCEDURAL_OPTIMIZATION)
if (USES_IPO)
    set(IPO_LINK_WARN_FLAGS_TO_CHECK "-Wno-null-dereference")
    foreach(flag ${IPO_LINK_WARN_FLAGS_TO_CHECK})
        CHECK_CXX_COMPILER_FLAG(${flag} CXX_COMPILER_SUPPORTS_${flag})
        if(CXX_COMPILER_SUPPORTS_${flag})
            #Flag supported, so enable it
            set_property(TARGET sdcparse_test APPEND PROPERTY LINK_FLAGS ${flag})
        endif()
    endforeach()
endif()

install(TARGETS libsdcparse DESTINATION bin)

add_subdirectory(test)
