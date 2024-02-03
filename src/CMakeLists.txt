cmake_minimum_required(VERSION 3.6)

project(learn-opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW    REQUIRED IMPORTED_TARGET glew)
pkg_check_modules(GLFW    REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(GLM    REQUIRED IMPORTED_TARGET glm)

set(SRCS main.cpp Renderer.cpp VertexBuffer.cpp IndexBuffer.cpp VertexArray.cpp VertexBuffer.cpp)

if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework Foundation -framework OpenGL -framework CoreVideo")
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    add_compile_options(-Wno-unused-command-line-argument)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME}  ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GLEW PkgConfig::GLFW PkgConfig::GLM)