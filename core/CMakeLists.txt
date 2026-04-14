add_library(core STATIC
    gamesaper.cpp
)

target_link_libraries(core PUBLIC Qt6::Core)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
