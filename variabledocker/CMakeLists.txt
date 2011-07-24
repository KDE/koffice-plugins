cmake_minimum_required(VERSION 2.4)
project( variable_docker_plugin )

find_package(KDE4 REQUIRED)
find_package(KdepimLibs REQUIRED)

include(FindKOfficeLibs)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

include_directories( ${KOFFICEUI_INCLUDES} )

SET ( variable_docker_plugin_SRCS
   variable_docker_plugin.cpp
   variable_docker_factory.cpp
   variable_docker_widget.cpp
   # other cpp files
)

kde4_add_plugin(variable_docker_plugin ${variable_docker_plugin_SRCS})
target_link_libraries(variable_docker_plugin flake kotext koodf ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})
install(TARGETS variable_docker_plugin DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES variable_docker_plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
