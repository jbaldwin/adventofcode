cmake_minimum_required(VERSION 3.10)

set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)

project(adventofcode)
add_library(${PROJECT_NAME} 
    lib/Algorithms.hpp
    lib/Crypto.h
    lib/FileUtil.h lib/FileUtil.cpp
    lib/StringUtil.h lib/StringUtil.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

function(project_setup year day problem)
    set(name ${year}_day${day}_p${problem})
    project(${name})
    set(project_bin_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}/)

    add_executable(${name} ${year}/day${day}/p${problem}/main.cpp)
    set_target_properties(
        ${name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${project_bin_dir}
    )
    target_compile_features(${name} PRIVATE cxx_std_17)
    target_link_libraries(${name} PRIVATE adventofcode)

    if (EXISTS "${year}/day${day}/p${problem}/input.txt")
        file(COPY 
            "${year}/day${day}/p${problem}/input.txt"
            DESTINATION
            ${project_bin_dir}
        )
    endif()
endfunction()

project_setup(2015 1 1)
project_setup(2015 1 2)
project_setup(2015 2 1)
project_setup(2015 2 2)
project_setup(2015 3 1)
project_setup(2015 3 2)
project_setup(2015 4 1)
project_setup(2015 4 2)
project_setup(2015 5 1)
project_setup(2015 5 2)
project_setup(2015 6 1)
project_setup(2015 6 2)
project_setup(2015 7 1)
project_setup(2015 7 2)
project_setup(2015 8 1)
project_setup(2015 8 2)
project_setup(2015 9 1)
project_setup(2015 9 2)

project_setup(2019 1 1)
project_setup(2019 1 2)
project_setup(2019 2 1)
project_setup(2019 2 2)
project_setup(2019 3 1)
project_setup(2019 3 2)
project_setup(2019 4 1)
project_setup(2019 4 2)
project_setup(2019 5 1)
project_setup(2019 5 2)
project_setup(2019 6 1)
project_setup(2019 6 2)
project_setup(2019 7 1)
project_setup(2019 7 2)
project_setup(2019 8 1)
project_setup(2019 8 2)
project_setup(2019 9 1)
project_setup(2019 9 2)