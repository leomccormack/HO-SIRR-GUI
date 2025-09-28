
message(STATUS "Configuring hosirrlib...")

# Set-up project 
project(hosirrlib LANGUAGES C)
add_library(${PROJECT_NAME} STATIC)

# Needed for C11 Atomics on GCC
if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif()

# Link with SAF
target_link_libraries(${PROJECT_NAME} PRIVATE saf)

# Source files
target_sources(${PROJECT_NAME} 
PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/hosirrlib/src/hosirr_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hosirrlib/src/hosirr_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hosirrlib/src/hosirrlib.c 
)

# Include directory
target_include_directories(${PROJECT_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hosirrlib/include/>   
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
) 


