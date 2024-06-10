cmake_minimum_required(VERSION 3.12)
project(GBEmu
    VERSION 0.1
    DESCRIPTION "A Gameboy Emulator"
    LANGUAGES CXX
)

#set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the directory containing the header files
include_directories(include)

# Add all the source files in the src directory to the executable
file(GLOB SOURCES "src/*.cpp")

# External libraries
add_subdirectory(dependencies)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ImGui-SFML::ImGui-SFML
    #nfd
)
