cmake_minimum_required(VERSION 3.10)
project(SimpleMixer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the include directories for SDL2 and SDL2_mixer
# Update these paths based on your system
include_directories(/opt/homebrew/include/SDL2) # SDL2 headers
include_directories(/opt/homebrew/include) # SDL2_mixer headers

# Find SDL2 and SDL2_mixer libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Add source files
add_executable(SimpleMixer main.cpp)

# Link SDL2 and SDL2_mixer libraries
target_link_libraries(SimpleMixer
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    /opt/homebrew/lib/libSDL2_mixer.dylib # Explicitly link SDL2_mixer if needed
    /opt/homebrew/lib/libSDL2.dylib # Explicitly link SDL2
)

# For macOS, you might also need to link against CoreFoundation and Cocoa
if(APPLE)
    target_link_libraries(SimpleMixer
        "-framework CoreFoundation"
        "-framework Cocoa"
    )
endif()
