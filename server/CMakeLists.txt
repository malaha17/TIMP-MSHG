add_executable(saper_server
    main.cpp
    echoserver.cpp
    echoserver.h
    func2serv.cpp
    func2serv.h
    dbsingleton.cpp
    dbsingleton.h
)

target_link_libraries(saper_server PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Sql
    common
)

target_include_directories(saper_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/common
)
