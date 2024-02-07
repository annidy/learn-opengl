cmake_minimum_required(VERSION 3.6)

project(learn-opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW    REQUIRED IMPORTED_TARGET glew)
pkg_check_modules(GLFW    REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(GLM    REQUIRED IMPORTED_TARGET glm)

set(SRCS main.cpp Renderer.cpp VertexBuffer.cpp IndexBuffer.cpp VertexArray.cpp VertexBuffer.cpp Shader.cpp Texture.cpp)
list(APPEND SRCS vendor/stb_image/stb_image.cpp)
list(APPEND SRCS vendor/imgui/imgui_draw.cpp vendor/imgui/imgui_impl_glfw.cpp vendor/imgui/imgui_tables.cpp vendor/imgui/imgui_widgets.cpp vendor/imgui/imgui.cpp vendor/imgui/imgui_impl_opengl3.cpp)

add_subdirectory(tests)

message(STATUS "SRCS: ${SRCS}")

if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework Foundation -framework OpenGL -framework CoreVideo")
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    add_compile_options(-Wno-unused-command-line-argument)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/include vendor/imgui ${CMAKE_CURRENT_LIST_DIR})
add_executable(${PROJECT_NAME}  ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GLEW PkgConfig::GLFW PkgConfig::GLM)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
