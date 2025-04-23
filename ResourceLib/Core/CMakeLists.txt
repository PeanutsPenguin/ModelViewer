# ------- [3D Viewer] Core Project ------- #

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ------- Sources files
# ------- Retrieving all the source files and putting them into a kind of list ------- #

message("[${TARGET_NAME}] Starting source file fetching..")

### ------- Header files

file(GLOB_RECURSE TARGET_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
list(FILTER TARGET_HEADER_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Source (C++) files

file(GLOB_RECURSE TARGET_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)
list(FILTER TARGET_SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})



# ------- Putting all those files under a common name/variable TARGET_FILES ------- #

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES} ${TARGET_SHADERS_FILES} )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_ASSETS_FILES})


#------ set this folder into a library

add_library(${TARGET_NAME} STATIC)

### ------- Appending the previous source files into the executable

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES} ${TARGET_ASSETS_FILES})
set_source_files_properties(${TARGET_ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

### ------- Specifies the include directories so Visual Studio knows 

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)

### ------- Since this is the Core we link every single previously compiled sub-project

target_link_libraries(${TARGET_NAME} PRIVATE ${DEPENDENCIES_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBMATH_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${RESOURCES_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${PHYSICS_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${LOWRENDERER_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${DATASTRUCTURE_LIBRARY})


message("[${TARGET_NAME}] Done.")

set(CORE_LIBRARY ${TARGET_NAME} PARENT_SCOPE)