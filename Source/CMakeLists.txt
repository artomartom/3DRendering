cmake_minimum_required(VERSION 3.22)
project(Occlusion3D)
set(CMAKE_CXX_STANDARD 20)

file (GLOB SOURCES "./*.cpp" )
add_executable(${PROJECT_NAME} ${SOURCES} )
 
option(MAKEGUI "build  as a GUI rather then console app" ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ImGui)

if(MAKEGUI)
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  target_compile_definitions(${PROJECT_NAME} PUBLIC GUI)
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC CONSOLE)
endif(MAKEGUI)

unset(MAKEGUI CACHE) # <---- this is important

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE  
  #/options:strict # unrecognized compiler options are an error
  /nologo #suppress copyright message    
  #/GA #optimize for Windows Application
  #/O1 # maximum optimizations (favor space)
  #/Os #  favor code space
  #/favor:INTEL64 #  favor code space
  #/analyze #Enable native analysis      
  #/Wall #enable all warnings
  #/external:env:<var>      # location of external headers
  /external:anglebrackets # treat all headers included via <> as external
  /external:I "C://Program Files//Microsoft Visual Studio//2022//Community" #All recursive subdirectories of path are considered external
  /external:W0
  )
  endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  

  add_library(ImGui_s STATIC
  ${PROJECT_SOURCE_DIR}//ImGui//imgui_impl_win32.cpp
  ${PROJECT_SOURCE_DIR}//ImGui//imgui_impl_dx11.cpp  
  ${PROJECT_SOURCE_DIR}//ImGui//imgui_tables.cpp
  ${PROJECT_SOURCE_DIR}//ImGui//imgui.cpp
  ${PROJECT_SOURCE_DIR}//ImGui//imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}//ImGui//imgui_widgets.cpp
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE ImGui_s)
  target_link_libraries(${PROJECT_NAME} PRIVATE D3D11 DXGI dxguid d3dcompiler )
  
  target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp)