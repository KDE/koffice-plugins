project(changecase)

set(changecase_SRCS
    ChangecasePlugin.cpp
    Changecase.cpp
    ChangecaseFactory.cpp
)

kde4_add_plugin(changecase ${changecase_SRCS})

target_link_libraries(changecase ${KODFTEXT_LIBS})

install(TARGETS changecase DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES changecase.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
