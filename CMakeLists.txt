cmake_minimum_required(VERSION 3.16)
project(SimplePointCloudViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/PointCloud.cpp
    src/PointCloudIO.cpp
    src/GLPointCloudWidget.cpp
)

set(HEADERS
    src/MainWindow.h
    src/PointCloud.h
    src/PointCloudIO.h
    src/GLPointCloudWidget.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# 着色器资源
set(SHADER_FILES
    resources/shaders/vertex.glsl
    resources/shaders/fragment.glsl
)

# 复制资源到构建目录
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
    DEPENDS ${SHADER_FILES}
)

add_dependencies(${PROJECT_NAME} copy_resources)
