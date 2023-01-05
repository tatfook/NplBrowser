cmake_minimum_required(VERSION 2.8.12.1)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(NplBrowser)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

set (BoostRoot_DIR "$ENV{BOOST_ROOT}")

set(NPLRuntime_DIR "${PROJECT_SOURCE_DIR}/deps/NPLRuntime" CACHE STRING "")
# set(NPLRuntime_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../NPLRuntime_win32")
set(ParaEngineClient_SOURCE_DIR ${NPLRuntime_DIR}/Client/trunk/ParaEngineClient)

# set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message("build 64 bits")
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_win64")
    set(OUTPUT_BIN_DIR "${NPLRuntime_DIR}/ParaWorld/bin64/cef3")
else()
    message("build 32 bits")
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_win32")
    set(OUTPUT_BIN_DIR "${NPLRuntime_DIR}/ParaWorld/bin32/cef3")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

message("==========BoostRoot_DIR:" ${BoostRoot_DIR})
message("==========NPLRuntime_DIR:" ${NPLRuntime_DIR})
message("==========ParaEngineClient_SOURCE_DIR:" ${ParaEngineClient_SOURCE_DIR})
message("==========OUTPUT_BIN_DIR:" ${OUTPUT_BIN_DIR})
# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
add_subdirectory(${CEF_ROOT}/tests/cefclient cefclient)
add_subdirectory(NplCefPlugin)

# Display configuration settings.
PRINT_CEF_CONFIG()
