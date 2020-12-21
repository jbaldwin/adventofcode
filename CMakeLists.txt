cmake_minimum_required(VERSION 3.10)

# Ignore tests and do not install nlohmann/json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(vendor/nlohmann/json)
add_subdirectory(vendor/jbaldwin/libchain)

set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)

project(adventofcode)
add_library(${PROJECT_NAME}
    lib/algorithms.hpp
    lib/AStar.hpp
    lib/containers.hpp
    lib/Crypto.h
    lib/file_util.hpp lib/file_util.cpp

    2019/intcode/Interpreter.hpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

function(project_setup year day problem)
    set(name ${year}_day${day}_p${problem})
    project(${name})
    set(project_bin_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

    add_executable(${name} ${year}/day${day}/p${problem}/main.cpp)
    set_target_properties(
        ${name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${project_bin_dir}
    )
    target_compile_features(${name} PRIVATE cxx_std_20)
    target_link_libraries(${name} PRIVATE
        adventofcode
        pthread
        nlohmann_json::nlohmann_json
        chain
    )

    if (EXISTS "${year}/day${day}/p${problem}/input.txt")
        file(COPY
            "${year}/day${day}/p${problem}/input.txt"
            DESTINATION
            ${project_bin_dir}
        )
    endif()
endfunction()

project_setup(2015 01 1)
project_setup(2015 01 2)
project_setup(2015 02 1)
project_setup(2015 02 2)
project_setup(2015 03 1)
project_setup(2015 03 2)
project_setup(2015 04 1)
project_setup(2015 04 2)
project_setup(2015 05 1)
project_setup(2015 05 2)
project_setup(2015 06 1)
project_setup(2015 06 2)
project_setup(2015 07 1)
project_setup(2015 07 2)
project_setup(2015 08 1)
project_setup(2015 08 2)
project_setup(2015 09 1)
project_setup(2015 09 2)
project_setup(2015 10 1)
# project_setup(2015 10 2) # No new code for 10.2
project_setup(2015 11 1)
# project_setup(2015 11 2) # No new code for 11.2
project_setup(2015 12 1)
project_setup(2015 12 2)
project_setup(2015 13 1)
project_setup(2015 13 2)
project_setup(2015 14 1)
project_setup(2015 14 2)
project_setup(2015 15 1)
project_setup(2015 15 2)
project_setup(2015 16 1)
project_setup(2015 16 2)
project_setup(2015 17 1)
project_setup(2015 17 2)
project_setup(2015 18 1)
project_setup(2015 18 2)
project_setup(2015 19 1)
project_setup(2015 19 2)
project_setup(2015 20 1)
project_setup(2015 20 2)
project_setup(2015 21 1)
# project_setup(2015 21 2) # No new code for 21.2
project_setup(2015 22 1)
# project_setup(2015 22 2) # No new code for 22.2
project_setup(2015 23 1)
# project_setup(2015 23 2) # No new code for 23.2
project_setup(2015 24 1)
# project_setup(2015 24 2) # No new code for 24.2
project_setup(2015 25 1)

project_setup(2016 01 1)
project_setup(2016 01 2)
project_setup(2016 02 1)
project_setup(2016 02 2)
project_setup(2016 03 1)
project_setup(2016 03 2)
project_setup(2016 04 1)
project_setup(2016 04 2)
project_setup(2016 05 1)
project_setup(2016 05 2)
project_setup(2016 06 1)
project_setup(2016 06 2)
project_setup(2016 07 1)
project_setup(2016 07 2)
project_setup(2016 08 1)
# project_setup(2016 08 2) # No new code for 08.2
project_setup(2016 09 1)
project_setup(2016 09 2)
project_setup(2016 10 1)
# project_setup(2016 10 2) # No new code for 10.2
project_setup(2016 11 1)
# project_setup(2020 11 1) # No new code for 11.2

project_setup(2019 01 1)
project_setup(2019 01 2)
project_setup(2019 02 1)
project_setup(2019 02 2)
project_setup(2019 03 1)
project_setup(2019 03 2)
project_setup(2019 04 1)
project_setup(2019 04 2)
project_setup(2019 05 1)
project_setup(2019 05 2)
project_setup(2019 06 1)
project_setup(2019 06 2)
project_setup(2019 07 1)
project_setup(2019 07 2)
project_setup(2019 08 1)
project_setup(2019 08 2)
project_setup(2019 09 1)
project_setup(2019 09 2)
project_setup(2019 10 1)
project_setup(2019 10 2)
project_setup(2019 11 1)
project_setup(2019 11 2)
project_setup(2019 12 1)
project_setup(2019 12 2)
project_setup(2019 13 1)
project_setup(2019 13 2)
project_setup(2019 14 1)
project_setup(2019 14 2)
project_setup(2019 15 1)
project_setup(2019 15 2)
project_setup(2019 16 1)
project_setup(2019 16 2)
project_setup(2019 17 1)
project_setup(2019 17 2)
project_setup(2019 18 1)
project_setup(2019 19 1)
project_setup(2019 19 2)
project_setup(2019 20 1)
project_setup(2019 20 2)
project_setup(2019 21 1)
project_setup(2019 21 2)
project_setup(2019 22 1)
project_setup(2019 22 2)
project_setup(2019 23 1)
project_setup(2019 23 2)
project_setup(2019 24 1)
project_setup(2019 24 2)
project_setup(2019 25 1)

project_setup(2020 01 1)
project_setup(2020 01 2)
project_setup(2020 02 1)
project_setup(2020 02 2)
project_setup(2020 03 1)
project_setup(2020 03 2)
project_setup(2020 04 1)
project_setup(2020 04 2)
project_setup(2020 05 1)
project_setup(2020 05 2)
project_setup(2020 06 1)
project_setup(2020 06 2)
project_setup(2020 07 1)
project_setup(2020 07 2)
project_setup(2020 08 1)
project_setup(2020 08 2)
project_setup(2020 09 1)
project_setup(2020 09 2)
project_setup(2020 10 1)
project_setup(2020 10 2)
project_setup(2020 11 1)
project_setup(2020 11 2)
project_setup(2020 12 1)
project_setup(2020 12 2)
project_setup(2020 13 1)
project_setup(2020 13 2)
project_setup(2020 14 1)
project_setup(2020 14 2)
project_setup(2020 15 1)
# project_setup(2020 15 2) # No new code for 15.2
project_setup(2020 16 1)
project_setup(2020 16 2)
project_setup(2020 17 1)
project_setup(2020 17 2)
project_setup(2020 18 1)
project_setup(2020 18 2)
project_setup(2020 19 1)
project_setup(2020 19 2)
project_setup(2020 20 1)
