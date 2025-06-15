cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(CppOrchestrator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_PREFIX_PATH /home/kami/libs/cpp/libtorch/)
# Find LibTorch package (cxx11-abi version is required)
find_package(Torch REQUIRED)

# ======================================================================
# STAGE 1: PYTHON ENVIRONMENT SETUP
# ======================================================================

set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpp_venv")
set(PYTHON_EXECUTABLE "${VENV_DIR}/bin/python3")

# --- THIS IS THE FIX ---
# We now pass the full path to requirements.txt as an argument to the script.
add_custom_target(
        setup_python_env ALL
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/setup.sh" "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Setting up Python virtual environment and dependencies..."
        VERBATIM
)

# ======================================================================
# STAGE 2: C++ APPLICATION BUILD
# ======================================================================

add_executable(orchestrator orchestrator.cpp)
add_dependencies(orchestrator setup_python_env)
target_link_libraries(orchestrator PRIVATE "${TORCH_LIBRARIES}")

add_custom_command(
        TARGET orchestrator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/convert_model.py
        $<TARGET_FILE_DIR:orchestrator>/convert_model.py
        COMMENT "Copying Python conversion script to build directory"
)