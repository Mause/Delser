project("Delser CPP")

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_library(delser
    Delser.cpp
    utils.cpp
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(main
        main.cpp
    )

    target_link_libraries(main
        delser
    )
endif()
