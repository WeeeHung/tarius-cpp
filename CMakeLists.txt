cmake_minimum_required(VERSION 3.10)
project(TariusAI VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration options
option(TARIUS_DISABLE_DEBUG_LOGS "Disable debug and info logs" OFF)
option(TARIUS_DISABLE_LLAMA_LOGS "Disable llama.cpp logs" OFF)

# Enable OpenMP if available (useful for llama.cpp)
find_package(OpenMP QUIET)
if(OpenMP_CXX_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Find required packages
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  add_subdirectory(external/nlohmann_json)
endif()
# Try to find system spdlog, otherwise use the submodule
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
  add_subdirectory(external/spdlog)
endif()
# For local LLM integration (optional for MVP)
# find_package(llama_cpp REQUIRED)

# If spdlog is not found through find_package, you might need to specify its location:
# set(SPDLOG_INCLUDE_DIR "/path/to/spdlog/include")
# include_directories(${SPDLOG_INCLUDE_DIR})

# Set up llama.cpp
set(LLAMA_STANDALONE OFF CACHE BOOL "Build llama.cpp as standalone library")
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "Build llama.cpp examples")
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "Build llama.cpp tests")
add_subdirectory(external/llama.cpp)

# Source files
set(SOURCES
    src/main.cpp
    src/app/cli_interface.cpp
    src/app/app_controller.cpp
    src/models/memory_manager.cpp
    src/models/llama_model.cpp
    src/ai_twin/ai_twin.cpp
    src/ai_secretary/ai_secretary.cpp
    src/ai_secretary/calendar.cpp
    src/ai_secretary/task_list.cpp
    src/utils/logger.cpp
    src/utils/config.cpp
    src/utils/json_handler.cpp
)

# Create regular executable with logs
add_executable(tarius_ai ${SOURCES})

# Create release executable with minimal logs
add_executable(tarius_ai_release ${SOURCES})
target_compile_definitions(tarius_ai_release PRIVATE 
    TARIUS_DISABLE_DEBUG_LOGS
    TARIUS_DISABLE_LLAMA_LOGS
)

# Include directories
target_include_directories(tarius_ai PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # If using submodule
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/llama.cpp
    # Add any other include directories here
)
target_include_directories(tarius_ai_release PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # If using submodule
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/llama.cpp
    # Add any other include directories here
)

# Link libraries
target_link_libraries(tarius_ai PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    # llama_cpp (if using local LLM)
    llama
)
target_link_libraries(tarius_ai_release PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    # llama_cpp (if using local LLM)
    llama
)

# Add C++17 filesystem library if needed (for std::filesystem)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(tarius_ai PRIVATE stdc++fs)
    target_link_libraries(tarius_ai_release PRIVATE stdc++fs)
endif()

# Create data directories during build
add_custom_command(TARGET tarius_ai POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data/conversations
    ${CMAKE_CURRENT_SOURCE_DIR}/data/summaries
    ${CMAKE_CURRENT_SOURCE_DIR}/data/calendar
    ${CMAKE_CURRENT_SOURCE_DIR}/data/tasks
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)
add_custom_command(TARGET tarius_ai_release POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data/conversations
    ${CMAKE_CURRENT_SOURCE_DIR}/data/summaries
    ${CMAKE_CURRENT_SOURCE_DIR}/data/calendar
    ${CMAKE_CURRENT_SOURCE_DIR}/data/tasks
    ${CMAKE_CURRENT_SOURCE_DIR}/models
) 