add_executable(microcipher
        main.cpp
        microcipher.cpp
        microcipher.h)


set(CMAKE_CXX_FLAGS "-O5 -Wall -Wextra")

set_target_properties(microcipher
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Boost 1.4 COMPONENTS program_options REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(microcipher LINK_PUBLIC ${Boost_LIBRARIES} OpenSSL::SSL )