cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")  # Disable optimizations
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION $<IF:$<CONFIG:Debug>,OFF,ON>)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PRODUCTION_BUILD "Make this a production build" OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(mygame)

add_compile_definitions(SHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

FetchContent_Declare(
  fastgltf
  GIT_REPOSITORY https://github.com/spnda/fastgltf.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(fastgltf)


set(SDL_STATIC ON)

add_subdirectory(thirdparty/SDL-release-2.30.5) #window opener
add_subdirectory(thirdparty/glad) #opengl loader
add_subdirectory(thirdparty/stb_image) #loading images
add_subdirectory(thirdparty/stb_truetype) #loading ttf files
#add_subdirectory(thirdparty/enet-1.3.17)		#networking
add_subdirectory(thirdparty/glm) #math\
#add_subdirectory(thirdparty/SDL_image)     #for clay sdl renderer
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")

add_executable("${CMAKE_PROJECT_NAME}"
  src/Shader.cpp
  src/main.cpp
  src/openglErrorReporting.cpp
  src/filereader.h
  src/camera.cpp
  src/loadTexture.h
  src/camera.h "thirdparty/imgui/imconfig.h" "thirdparty/imgui/imgui.cpp" "thirdparty/imgui/imgui.h" "thirdparty/imgui/imgui_demo.cpp" "thirdparty/imgui/imgui_draw.cpp" "thirdparty/imgui/imgui_internal.h" "thirdparty/imgui/imgui_tables.cpp" "thirdparty/imgui/imgui_widgets.cpp" "thirdparty/imgui/imstb_rectpack.h" "thirdparty/imgui/imstb_textedit.h" "thirdparty/imgui/imstb_truetype.h" "thirdparty/imgui/imgui_impl_sdl2.cpp" "thirdparty/imgui/imgui_impl_sdl2.h" "thirdparty/imgui/imgui_impl_sdlrenderer2.cpp" "thirdparty/imgui/imgui_impl_sdlrenderer2.h")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)


target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC GLFW_INCLUDE_NONE=1)

if(PRODUCTION_BUILD)
  # setup the ASSETS_PATH macro to be in the root folder of your exe
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/")

  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1)

else()
  # This is useful to get an ASSETS_PATH in your IDE during development
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0)

endif()

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm
 glad stb_image stb_truetype SDL2-static)

