cmake_minimum_required(VERSION 3.5)

project(bgfx-qt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

set(BGFX_AMALGAMATED      OFF CACHE INTERNAL "" FORCE)
set(BGFX_BUILD_EXAMPLES   OFF CACHE INTERNAL "" FORCE)
set(BGFX_CONFIG_DEBUG     OFF CACHE INTERNAL "" FORCE)
set(BGFX_CUSTOM_TARGETS   OFF CACHE INTERNAL "" FORCE)
set(BGFX_INSTALL          OFF CACHE INTERNAL "" FORCE)
set(BGFX_INSTALL_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(BGFX_USE_DEBUG_SUFFIX OFF CACHE INTERNAL "" FORCE)
set(BGFX_USE_OVR          OFF CACHE INTERNAL "" FORCE)
set(BUILD_SHARED_LIBS     OFF CACHE INTERNAL "" FORCE)
set(BX_AMALGAMATED        OFF CACHE INTERNAL "" FORCE)

set(BGFX_BUILD_TOOLS      ON  CACHE INTERNAL "" FORCE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DBX_CONFIG_DEBUG=1)
else()
  add_definitions(-DBX_CONFIG_DEBUG=0)
endif()

# Synchronize bgfx rendering with Qt main thread
add_definitions(-DBGFX_CONFIG_MULTITHREADED=0)

add_subdirectory(bgfx.cmake)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${BGFX_DIR}/include
  ${BX_DIR}/include
)
if(WIN32)
  include_directories(${BX_DIR}/include/compat/msvc)
endif()

set(SHADER_OUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders)
bgfx_compile_shader_to_header(
  TYPE VERTEX
  SHADERS ${CMAKE_SOURCE_DIR}/shaders/vs_cubes.sc
  VARYING_DEF ${CMAKE_SOURCE_DIR}/shaders/varying.def.sc
  OUTPUT_DIR ${SHADER_OUT_DIR}
  INCLUDE_DIRS ${BX_DIR}/include ${BGFX_DIR}/src
)
bgfx_compile_shader_to_header(
  TYPE FRAGMENT
  SHADERS ${CMAKE_SOURCE_DIR}/shaders/fs_cubes.sc
  VARYING_DEF ${CMAKE_SOURCE_DIR}/shaders/varying.def.sc
  OUTPUT_DIR ${SHADER_OUT_DIR}
  INCLUDE_DIRS ${BX_DIR}/include ${BGFX_DIR}/src
)

qt_add_executable(${PROJECT_NAME}
  shaders/vs_cubes.sc shaders/fs_cubes.sc
  BGFXWidget.h BGFXWidget.cpp
  MainWindow.h MainWindow.cpp MainWindow.ui
  Main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SHADER_OUT_DIR})

if(WIN32)
  set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets bgfx)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
