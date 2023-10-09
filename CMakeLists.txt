cmake_minimum_required(VERSION 3.1)
project(learn_openGL)

set (CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

# ---
include_directories( 
     ${OPENGL_INCLUDE_DIRS}
     3party/glad/include
     3party
     .
)

add_subdirectory(3party/glfw)

# ---
set(glad_files 
     3party/glad/include/glad/glad.h
     3party/glad/src/glad.c
)

# ---
set(base_utils
     utils/shader.cpp
     utils/shader.hpp
     utils/camera.hpp
)
# END OF PREPARATION

# --- FIRST LESSON (Colors) ----------------------------------

set(out_bin "01_colors")

add_executable(${out_bin}
     ${base_utils}
     ${out_bin}/main.cpp
     ${glad_files}
)

file(COPY 
          ${out_bin}/shaders 
          ${out_bin}/textures 
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${out_bin}
     ${OPENGL_LIBRARIES}
     glfw
)

# --- SECOND LESSON (Basics) ----------------------------------

set(out_bin "02_basics")

add_executable(${out_bin}
     ${base_utils}
     ${out_bin}/main.cpp
     ${glad_files}
)

file(COPY 
          ${out_bin}/shaders 
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${out_bin}
     ${OPENGL_LIBRARIES}
     glfw
)

# --- THIRD LESSON (Materials) ----------------------------------

set(out_bin "03_materials")

add_executable(${out_bin}
     ${base_utils}
     ${out_bin}/main.cpp
     ${glad_files}
)

file(GLOB my_shaders "${out_bin}/shaders/*.*s")
file(COPY 
          ${my_shaders}
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}/shaders/${out_bin}
)

target_link_libraries(${out_bin}
     ${OPENGL_LIBRARIES}
     glfw
)

# --- FOURTH LESSON (Lighting maps) ----------------------------------

set(out_bin "04_lighting-maps")

add_executable(${out_bin}
     ${base_utils}
     ${out_bin}/main.cpp
     ${glad_files}
)

file(GLOB my_shaders "${out_bin}/shaders/*.*s")
file(COPY 
          ${my_shaders}
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}/shaders/${out_bin}
)

file(GLOB my_textures "${out_bin}/textures/*.*")
file(COPY 
          ${my_textures}
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}/textures/${out_bin}
)

target_link_libraries(${out_bin}
     ${OPENGL_LIBRARIES}
     glfw
)

# --- FIFTH LESSON (Multiple lights)

set(out_bin "05_multiple-lights")

add_executable(${out_bin}
     ${base_utils}
     ${out_bin}/main.cpp
     ${glad_files}
)

file(GLOB my_shaders "${out_bin}/shaders/*.*s")
file(COPY 
          ${my_shaders}
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}/shaders/${out_bin}
)

file(GLOB my_textures "${out_bin}/textures/*.*")
file(COPY 
          ${my_textures}
     DESTINATION 
          ${CMAKE_CURRENT_BINARY_DIR}/textures/${out_bin}
)

target_compile_definitions(${out_bin} PRIVATE LESSON_NAME="${out_bin}")

target_link_libraries(${out_bin}
     ${OPENGL_LIBRARIES}
     glfw
)

