project(spellcheck)
add_subdirectory( tests )

set(spellcheck_SRCS
    SpellCheckPlugin.cpp
    SpellCheck.cpp
    SpellCheckFactory.cpp
    BgSpellCheck.cpp
    SpellCheckMenu.cpp
)

kde4_add_plugin(spellcheck ${spellcheck_SRCS})

target_link_libraries(spellcheck ${KODFTEXT_LIBS})

install(TARGETS spellcheck DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES spellcheck.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
