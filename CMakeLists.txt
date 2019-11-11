project(bgfx_qt)
cmake_minimum_required(VERSION 2.8)

set(bgfx_qt_output_dir ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bgfx_qt_output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${bgfx_qt_output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${bgfx_qt_output_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${bgfx_qt_output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${bgfx_qt_output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${bgfx_qt_output_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${bgfx_qt_output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${bgfx_qt_output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${bgfx_qt_output_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${bgfx_qt_output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${bgfx_qt_output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${bgfx_qt_output_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${bgfx_qt_output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${bgfx_qt_output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${bgfx_qt_output_dir})

cmake_policy(SET CMP0020 NEW)
set(CMAKE_CXX_STANDARD 11)

set(QT_ROOT CACHE PATH QT_ROOT)
if(NOT QT_ROOT)
  message(FATAL_ERROR "Need to specify QT_ROOT")
endif()
set(CMAKE_PREFIX_PATH ${QT_ROOT})
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(MSVC_YEAR 2005)
if (MSVC_TOOLSET_VERSION EQUAL 90)
  set(MSVC_YEAR 2008)
elseif(MSVC_TOOLSET_VERSION EQUAL 100)
  set(MSVC_YEAR 2010)
elseif(MSVC_TOOLSET_VERSION EQUAL 110)
  set(MSVC_YEAR 2012)
elseif(MSVC_TOOLSET_VERSION EQUAL 120)
  set(MSVC_YEAR 2013)
elseif(MSVC_TOOLSET_VERSION EQUAL 140)
  set(MSVC_YEAR 2015)
elseif(MSVC_TOOLSET_VERSION EQUAL 141)
  set(MSVC_YEAR 2017)
elseif(MSVC_TOOLSET_VERSION EQUAL 142)
  set(MSVC_YEAR 2019)
endif()

set(BGFX_ROOT CACHE PATH BGFX_ROOT)
set(BIMG_ROOT CACHE PATH BIMG_ROOT)
set(BX_ROOT CACHE PATH BX_ROOT)

if(NOT BGFX_ROOT)
  message(FATAL_ERROR "Need to specify BGFX_ROOT")
endif()
if(NOT BIMG_ROOT)
  message(FATAL_ERROR "Need to specify BIMG_ROOT")
endif()
if(NOT BX_ROOT)
  message(FATAL_ERROR "Need to specify BX_ROOT")
endif()

set(BGFX_QT_UI_FILES
${bgfx_qt_SOURCE_DIR}/MainWindow.ui
)

set(BGFX_MOC_FILES
${bgfx_qt_SOURCE_DIR}/BGFXWidget.h
${bgfx_qt_SOURCE_DIR}/MainWindow.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(
${Qt5Widgets_INCLUDE_DIRS}
${bgfx_qt_SOURCE_DIR}
)

set(SHADER_BIN_FILES)
set(SHADER_SOURCES
${bgfx_qt_SOURCE_DIR}/shaders/varying.def.sc
)

macro(compile_shader_for_single_platform shader_file_name target_folder)
  list(APPEND SHADER_BIN_FILES ${bgfx_qt_output_dir}/shaders/${target_folder}/${shader_file_name}.bin)
  list(APPEND SHADER_SOURCES ${bgfx_qt_SOURCE_DIR}/shaders/${shader_file_name}.sc)
  set(shader_flags ${ARGN})
  add_custom_command(
  OUTPUT ${bgfx_qt_output_dir}/shaders/${target_folder}/${shader_file_name}.bin
  COMMAND
    ${BGFX_ROOT}/.build/win64_vs${MSVC_YEAR}/bin/shadercRelease
    -f ${bgfx_qt_SOURCE_DIR}/shaders/${shader_file_name}.sc
    -o ${bgfx_qt_output_dir}/shaders/${target_folder}/${shader_file_name}.bin
    -i ${bgfx_qt_SOURCE_DIR}/shaders
    -i ${BGFX_ROOT}/src
    --varyingdef ${bgfx_qt_SOURCE_DIR}/shaders/varying.def.sc
    ${shader_flags}
  MAIN_DEPENDENCY
    ${bgfx_qt_SOURCE_DIR}/shaders/${shader_file_name}.sc
  DEPENDS
    ${bgfx_qt_SOURCE_DIR}/shaders/varying.def.sc
  WORKING_DIRECTORY
    ${bgfx_qt_SOURCE_DIR}/shaders/
  )
endmacro()

macro(compile_vs_shader shader_file_name)
  if(WIN32)
    compile_shader_for_single_platform(${shader_file_name} dx9 --type vertex --platform windows -p vs_3_0 -O 3)
    compile_shader_for_single_platform(${shader_file_name} dx11 --type vertex --platform windows -p vs_5_0 -O 3)
  endif()
  compile_shader_for_single_platform(${shader_file_name} metal --type vertex --platform osx -p metal)
  compile_shader_for_single_platform(${shader_file_name} glsl --type vertex --platform linux -p 120)
  compile_shader_for_single_platform(${shader_file_name} essl --type vertex --platform android)
  compile_shader_for_single_platform(${shader_file_name} spirv --type vertex --platform linux -p spirv)
endmacro()

macro(compile_fs_shader shader_file_name)
  if(WIN32)
    compile_shader_for_single_platform(${shader_file_name} dx9 --type fragment --platform windows -p ps_3_0 -O 3)
    compile_shader_for_single_platform(${shader_file_name} dx11 --type fragment --platform windows -p ps_5_0 -O 3)
  endif()
  compile_shader_for_single_platform(${shader_file_name} metal --type fragment --platform osx -p metal)
  compile_shader_for_single_platform(${shader_file_name} glsl --type fragment --platform linux -p 120)
  compile_shader_for_single_platform(${shader_file_name} essl --type fragment --platform android)
  compile_shader_for_single_platform(${shader_file_name} spirv --type fragment --platform linux -p spirv)
endmacro()

compile_vs_shader(vs_cubes)
compile_fs_shader(fs_cubes)

include_directories(
${BGFX_ROOT}/include
${BX_ROOT}/include
${BX_ROOT}/include/compat/msvc
)

link_directories(
${BGFX_ROOT}/.build/win64_vs${MSVC_YEAR}/bin
)

add_executable(bgfx_qt WIN32
${BGFX_QT_UI_FILES}
${BGFX_MOC_FILES}
${SHADER_SOURCES}
${SHADER_BIN_FILES}
${bgfx_qt_SOURCE_DIR}/BGFXWidget.h
${bgfx_qt_SOURCE_DIR}/BGFXWidget.cpp
${bgfx_qt_SOURCE_DIR}/Main.cpp
${bgfx_qt_SOURCE_DIR}/MainWindow.h
${bgfx_qt_SOURCE_DIR}/MainWindow.cpp
)

if(WIN32)
  set_target_properties(bgfx_qt PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${bgfx_qt_output_dir})
endif()

source_group(ShaderBinaries FILES ${SHADER_BIN_FILES})
source_group(ShadersSources FILES ${SHADER_SOURCES})
set_source_files_properties(${SHADER_BIN_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(${SHADER_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(bgfx_qt Qt5::Core Qt5::Widgets)
target_link_libraries(bgfx_qt optimized bgfxRelease optimized bimgRelease optimized bimg_decodeRelease optimized bxRelease debug bgfxDebug debug bimgDebug debug bimg_decodeDebug debug bxDebug)
