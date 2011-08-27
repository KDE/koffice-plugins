project( variable_docker_plugin )


include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES})

set(variable_docker_plugin_SRCS
   variable_docker_plugin.cpp
   variable_docker_factory.cpp
   variable_docker_widget.cpp
)



kde4_add_plugin(variable_docker_plugin ${variable_docker_plugin_SRCS})

target_link_libraries(variable_docker_plugin ${KODFTEXT_LIBS})

install(TARGETS variable_docker_plugin DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES variable_docker_plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
