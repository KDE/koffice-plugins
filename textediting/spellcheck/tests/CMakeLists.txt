set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/plugins/textediting/spellcheck)

########### next target ###############

set(TestSpellCheck_SRCS
    TestSpellCheck.cpp
    ../BgSpellCheck.cpp
)

kde4_add_unit_test(TestSpellCheck TESTNAME textedit-TestSpellCheck ${TestSpellCheck_SRCS})

target_link_libraries(TestSpellCheck ${KODFTEXT_LIBS} ${QT_QTTEST_LIBRARY})

########### last target ###############
