cmake_minimum_required(VERSION 3.5)

project(RBFwrapper)

set(MAYA_INCLUDE_DIR "D:/Program Files/Autodesk/Maya2017/include")
set(MAYA_LIB_PATH "D:/Program Files/Autodesk/Maya2017/lib")
set(MAYA_LIBRARIES ${MAYA_LIB_PATH}/Foundation.lib
					${MAYA_LIB_PATH}/OpenMaya.lib)

include_directories(${MAYA_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LIB ${MAYA_LIBRARIES})

set(SOURCE
		src/BindCage.cpp
		src/pluginMain.cpp
		src/RBFTransfomWrapper.cpp)

set(TARGET ${PROJECT_NAME})

add_library(${TARGET} SHARED ${SOURCE})
target_link_libraries(${TARGET} ${LIB})
if (WIN32)
	set_target_properties(${TARGET} PROPERTIES
		LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
	set_target_properties(${TARGET} PROPERTIES
		PREFIX ""
		SUFFIX ".mll")
endif()
