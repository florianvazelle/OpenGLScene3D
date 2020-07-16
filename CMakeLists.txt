project("OpenGLScene3D")
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 -Wall")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libs)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Scene
set(SCENE_SOURCES  ${${PROJECT_NAME}_SOURCES} example/Scene.cpp)
add_executable(scene ${SCENE_SOURCES} ${${PROJECT_NAME}_HEADERS})
target_link_libraries(scene glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

# Sky
set(SKY_SOURCES  ${${PROJECT_NAME}_SOURCES} example/Sky.cpp)
add_executable(sky ${SKY_SOURCES} ${${PROJECT_NAME}_HEADERS})
target_link_libraries(sky glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

# Shadow
set(SHADOW_SOURCES  ${${PROJECT_NAME}_SOURCES} example/Shadow.cpp)
add_executable(shadow ${SHADOW_SOURCES} ${${PROJECT_NAME}_HEADERS})
target_link_libraries(shadow glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})


file(COPY assets/ DESTINATION assets)
