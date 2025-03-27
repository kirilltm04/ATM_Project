cmake_minimum_required(VERSION 3.10)
project(Programming_Assignment)

# Find GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Find SQLite
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Add executable with additional source files
add_executable(Programming_Assignment main.c algorithm.c databases.c unittest.c)

# Link GTK4
target_include_directories(Programming_Assignment PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_directories(Programming_Assignment PRIVATE ${GTK4_LIBRARY_DIRS})
target_link_libraries(Programming_Assignment PRIVATE ${GTK4_LIBRARIES})

# Link SQLite
target_include_directories(Programming_Assignment PRIVATE ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(Programming_Assignment PRIVATE ${SQLITE3_LIBRARIES})
target_compile_options(Programming_Assignment PRIVATE ${SQLITE3_CFLAGS_OTHER})

# Ensure runtime environment variables are set
set_property(TARGET Programming_Assignment APPEND PROPERTY ENVIRONMENT
        "LIBGL_ALWAYS_SOFTWARE=1"
        "GSK_RENDERER=cairo"
        )
