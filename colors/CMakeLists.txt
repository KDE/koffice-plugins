add_subdirectory(pics)

include_directories(docker ${KOFFICELIBS_INCLUDE_DIR}/private)

set(kofficecolorsdocker_SRCS
    docker/ColorChooser.cpp
    docker/Docker.cpp
    docker/DockerFactory.cpp
    docker/GradientChooser.cpp
    docker/GradientEditor.cpp
    docker/GradientStopsEditor.cpp
    docker/LineStyleItemDelegate.cpp
    docker/LineStyleModel.cpp
    docker/LineStyleSelector.cpp
    docker/ColorPicker.cpp
    docker/XYColorSelector.cpp
    docker/LineStrokes.cpp
    docker/PatternChooser.cpp
    docker/SelectorWidget.cpp
    docker/StrokeFillSelector.cpp
    Plugin.cpp
)

kde4_add_ui_files(kofficecolorsdocker_SRCS
    docker/SelectorWidget.ui
    docker/GradientEditor.ui
    docker/PatternChooser.ui
    docker/LineStrokes.ui
)
kde4_add_plugin(kofficecolorsdocker ${kofficecolorsdocker_SRCS})

target_link_libraries(kofficecolorsdocker ${FLAKE_LIBS} ${KOWIDGETS_LIBS} ${PIGMENTCMS_LIBS})

install(TARGETS kofficecolorsdocker  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES kofficecolorsdocker.desktop DESTINATION ${SERVICES_INSTALL_DIR})
