cmake_minimum_required(VERSION 3.13)
project(microcipher)
set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(microcipher
        main.cpp
        microcipher.cpp
        microcipher.h)

set_target_properties(microcipher
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#install(TARGETS microcipher RUNTIME DESTINATION bin)
find_package(Boost 1.4 COMPONENTS program_options REQUIRED)
target_link_libraries(microcipher LINK_PUBLIC ${Boost_LIBRARIES} )
