project(koffice-plugins)

find_package(KDE4 REQUIRED)
find_package(KOfficeLibs REQUIRED)

include (KDE4Defaults)

include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} ${KOFFICELIBS_INCLUDE_DIR})

add_subdirectory(divineProportion)
add_subdirectory(paragraphtool)
add_subdirectory(pluginshape)
add_subdirectory(simpletextedit)
add_subdirectory(textediting)
add_subdirectory(colors)

