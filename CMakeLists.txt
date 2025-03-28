cmake_minimum_required(VERSION 3.10)
project(Programming_Assignment)

# Find GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Add executable with additional source files
add_executable(Programming_Assignment main.c)
add_executable(Programming_Assignment_Text algorithm.c main_text.c)
add_executable(Programming_Assignment_Tests algorithm.c unittest.c)
add_executable(Programming_Assignment_Gui algorithm.c gui.c)

# Link GTK4
target_include_directories(Programming_Assignment_Gui PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_directories(Programming_Assignment_Gui PRIVATE ${GTK4_LIBRARY_DIRS})
target_link_libraries(Programming_Assignment_Gui PRIVATE ${GTK4_LIBRARIES})

# Link SQLite

# Ensure runtime environment variables are set
set_property(TARGET Programming_Assignment_Gui APPEND PROPERTY ENVIRONMENT
        "LIBGL_ALWAYS_SOFTWARE=1"
        "GSK_RENDERER=cairo"
        )
