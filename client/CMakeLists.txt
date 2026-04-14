add_executable(saper_client
    main.cpp
    client_singleton.cpp
    client_singleton.h
    func2client.cpp
    func2client.h
    auth_reg_form.cpp
    auth_reg_form.h
    game_form.cpp
    game_form.h
    top10_form.cpp
    top10_form.h
    manager_form.cpp
    manager_form.h
    cellbutton.h
)

target_link_libraries(saper_client PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    common
    core
)

target_include_directories(saper_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/core
)
