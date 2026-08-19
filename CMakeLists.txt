cmake_minimum_required(VERSION 3.14)
project(PhysicsEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Include project headers AND raylib headers from w64devkit
include_directories(include C:/raylib/w64devkit/include)

# 2. Tell CMake where to search for library files (.a)
link_directories(C:/raylib/w64devkit/lib)

# 3. Gather source files
file(GLOB SOURCES "src/*.cpp")

# 4. Define target executable
add_executable(${PROJECT_NAME} ${SOURCES})

# 5. Link raylib and required Windows system libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib 
    opengl32 
    gdi32 
    winmm
)