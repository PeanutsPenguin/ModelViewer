get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)

FetchContent_Declare(
  fast_float
  GIT_REPOSITORY https://github.com/lemire/fast_float.git
  GIT_TAG tags/v1.1.2
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(fast_float)


### ------- Header files

file(GLOB_RECURSE TARGET_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
list(FILTER TARGET_HEADER_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Source (C++) files

file(GLOB_RECURSE TARGET_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cd
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)
list(FILTER TARGET_SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Addon (.lib) files

file(GLOB_RECURSE TARGET_DLL_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.dll)
set(BIN_DEPENDENCIES ${TARGET_DLL_FILES} PARENT_SCOPE)

file(GLOB_RECURSE TARGET_LIB_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*/Addon/*.lib)
list(FILTER TARGET_EXTRA_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

# ------- Putting all those files under a common name/variable TARGET_FILES ------- #

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES} ${TARGET_SHADERS_FILES} )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})

# ------- Executable
# ------- In this part of the code will link everything together ------- #


add_library(
  ${TARGET_NAME}
  STATIC
)

message("[${TARGET_NAME}] The executable is being linked with the project's dependencies..")

### ------- Appending the previous source files into the executable

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES} ${TARGET_ASSETS_FILES})

### ------- Specifies the include directories so Visual Studio knows 

target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_LIB_FILES})
target_link_libraries(${TARGET_NAME} PUBLIC fast_float)

file(GLOB TARGET_DIRECTORIES LIST_DIRECTORIES true 
	${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(DIRECTORY ${TARGET_DIRECTORIES})
	if(IS_DIRECTORY ${DIRECTORY})
		target_include_directories(${TARGET_NAME} PUBLIC ${DIRECTORY}/Header)
	else() 
		continue() 
	endif()
endforeach()

### ------- Since this is the Core we link every single previously compiled sub-project

set(MODERN_LIBRARY ${TARGET_NAME} PARENT_SCOPE)

message("[${TARGET_NAME}] Done.")