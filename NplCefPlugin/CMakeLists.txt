# Author: leio
# Date: 2016.12.7
# Desc: 

cmake_minimum_required(VERSION 2.6)
project (NplCefPlugin)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)

SET(NplCefPlugin_Source_files "")
file (GLOB src_files ${PROJECT_SOURCE_DIR}/src/*.*)
SOURCE_GROUP("src" FILES ${src_files}) 
list(APPEND NplCefPlugin_Source_files ${src_files})

# json
file (GLOB json_files ${PROJECT_SOURCE_DIR}/../json/*.*)
source_group(json FILES ${json_files})
list(APPEND NplCefPlugin_Source_files ${json_files})


include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${ParaEngineClient_SOURCE_DIR}")
include_directories("${ParaEngineClient_SOURCE_DIR}/Core")
include_directories("${ParaEngineClient_SOURCE_DIR}/math")
include_directories("${ParaEngineClient_SOURCE_DIR}/util")
include_directories("${ParaEngineClient_SOURCE_DIR}/renderer")
include_directories("${ParaEngineClient_SOURCE_DIR}/IO")
include_directories("${BoostRoot_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../json")

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


 

add_library(NplCefPlugin SHARED ${NplCefPlugin_Source_files})
target_link_libraries(NplCefPlugin)

# Copy CEF binary and resource files to the target output directory.
COPY_FILES(NplCefPlugin "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${OUTPUT_BIN_DIR}")
COPY_FILES(NplCefPlugin "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${OUTPUT_BIN_DIR}")

ADD_CUSTOM_COMMAND(
   TARGET NplCefPlugin
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NplCefPlugin> ${OUTPUT_BIN_DIR}
)