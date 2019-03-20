# Author: leio
# Date: 2016.12.7
# Desc: 

cmake_minimum_required(VERSION 2.6)

project (NplCefProcess)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DNOMINMAX)

SET(NplCefProcess_Source_files "")

file (GLOB src_files ${PROJECT_SOURCE_DIR}/src/*.*)
SOURCE_GROUP("src" FILES ${src_files}) 
list(APPEND NplCefProcess_Source_files ${src_files})

include_directories("${ParaEngineClient_SOURCE_DIR}/Core")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${CEF_ROOT}")
IF(MSVC)
	# statically link MSVC to reduce dependancies
	foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
	 	if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	 	endif(${flag_var} MATCHES "/MD")
	 	if(${flag_var} MATCHES "/MDd")
	 		string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/MDd")
	endforeach(flag_var)
	
	# /GR- Remove RTTI to miminize the executable size
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR-")
	 # generate program database (PDB symbol files even in release build)
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
	SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")


ENDIF()


SET(CMAKE_DEBUG_POSTFIX "_d")
link_directories(${CEF_BINARY_DIR})

# add the executable
add_executable(NplCefProcess ${NplCefProcess_Source_files})
set_target_properties(NplCefProcess PROPERTIES OUTPUT_NAME "NplCefProcess")
set_target_properties(NplCefProcess PROPERTIES DEBUG_OUTPUT_NAME "NplCefProcess_d")
set_target_properties(NplCefProcess PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS") 
target_link_libraries(NplCefProcess libcef libcef_dll_wrapper "${CEF_STANDARD_LIBS}" "${CEF_SANDBOX_STANDARD_LIBS}")



ADD_CUSTOM_COMMAND(
   TARGET NplCefProcess
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NplCefProcess> ${OUTPUT_BIN_DIR}
)


