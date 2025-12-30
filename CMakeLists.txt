cmake_minimum_required(VERSION 3.10)

project(MotionTrajectory VERSION 1.0.0 LANGUAGES CXX)

# =====================
# C++ standard
# =====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =====================
# Default build type
# =====================
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified. Defaulting to Release.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# =====================
# Find OpenCV
# =====================
find_package(OpenCV REQUIRED)

# =====================
# Source files
# =====================
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cc"
)

# =====================
# Executable
# =====================
add_executable(${PROJECT_NAME} ${SOURCES})

# =====================
# Compiler options (TARGET-based)
# =====================
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Debug symbols only in Debug
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-g>
)

# =====================
# Link OpenCV
# =====================
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
)

# =====================
# MinGW specific
# =====================
if (MINGW)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN64 _AMD64_ IS_64BIT)
endif()

# =====================
# Install
# =====================
install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# =====================
# Info
# =====================
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Arch: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
