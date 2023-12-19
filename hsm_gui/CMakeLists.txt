set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC  ON)

add_executable(tricolor_light_gui
        main.cpp
        TricolorLight.hpp
        TricolorMainWindow.hpp
        ui/TricolorMainWindow.ui)

target_link_libraries(tricolor_light_gui PUBLIC simple-hsm::simple-hsm fmt::fmt qt::qt)
