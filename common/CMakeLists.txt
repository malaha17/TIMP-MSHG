add_library(common STATIC
    protocol.cpp
    securityutils.cpp
)

target_link_libraries(common PUBLIC Qt6::Core)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
