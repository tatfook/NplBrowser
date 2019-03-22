cmake_minimum_required(VERSION 2.8.12.1)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(NplBrowser)



# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

set (BoostRoot_DIR "$ENV{BOOST_ROOT}")
set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

set(NPLRuntime_DIR "${PROJECT_SOURCE_DIR}/deps/NPLRuntime" CACHE STRING "")
set(ParaEngineClient_SOURCE_DIR ${NPLRuntime_DIR}/Client/trunk/ParaEngineClient)
set( OUTPUT_BIN_DIR "${NPLRuntime_DIR}/ParaWorld/bin32/cef3" )


# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
add_subdirectory(tests/cefclient)
add_subdirectory(NplCefPlugin)


# Display configuration settings.
PRINT_CEF_CONFIG()
