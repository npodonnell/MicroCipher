cmake_minimum_required(VERSION 2.6)
project(microcipher)

add_executable(microcipher main.cpp)

install(TARGETS microcipher RUNTIME DESTINATION bin)
find_package(Boost 1.4 COMPONENTS program_options REQUIRED)
target_link_libraries(microcipher LINK_PUBLIC ${Boost_LIBRARIES} )
