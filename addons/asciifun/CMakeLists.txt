add_library(asciiart STATIC asciiart.cpp asciifonts.qrc)
target_link_libraries(asciiart PUBLIC Qt::Gui)
set_target_properties(asciiart PROPERTIES POSITION_INDEPENDENT_CODE ON)

kate_add_plugin(asciifunplugin)
target_compile_definitions(asciifunplugin PRIVATE TRANSLATION_DOMAIN="asciifunplugin")
target_link_libraries(asciifunplugin PRIVATE asciiart KF6::I18n KF6::TextEditor)

target_sources(
  asciifunplugin
  PRIVATE
    asciifunplugin.cpp
    matrixrain.cpp
    plugin.qrc
)

if (BUILD_TESTING)
    include(ECMMarkAsTest)

    find_package(Qt6Test ${QT_MIN_VERSION} QUIET REQUIRED)
    add_executable(asciiarttest asciiarttest.cpp)
    add_test(NAME asciiarttest COMMAND asciiarttest)
    target_link_libraries(asciiarttest PRIVATE asciiart Qt::Test)
    ecm_mark_as_test(asciiarttest)
endif()
