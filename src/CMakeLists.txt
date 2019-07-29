add_executable(microcipher
        main.cpp
        microcipher.cpp
        microcipher.h)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

set_target_properties(microcipher
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Boost EXACT REQUIRED COMPONENTS program_options)
find_package(OpenSSL REQUIRED)
target_link_libraries(microcipher Boost::program_options OpenSSL::SSL)

