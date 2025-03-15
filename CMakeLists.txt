cmake_minimum_required(VERSION 3.10)
project(flatpack-portable-builder VERSION 1.0 LANGUAGES CXX)

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.80.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED
    Core
    Widgets
)

# Find KDE Frameworks
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# Find KDE Frameworks we need
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    XmlGui
    Crash
)

# Sources
set(flatpack_portable_builder_SRCS
    main.cpp
    mainwindow.cpp
    portableappinfo.h
    wineconfigwidget.cpp
    flatpakmanifest.cpp
)

# Add executable
add_executable(flatpack-portable-builder ${flatpack_portable_builder_SRCS})

# Link libraries
target_link_libraries(flatpack-portable-builder
    Qt5::Core
    Qt5::Widgets
    KF5::I18n
    KF5::XmlGui
    KF5::Crash
)

# Install
install(TARGETS flatpack-portable-builder ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.flatpack-portable-builder.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.flatpack-portable-builder.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# Add UI file
ki18n_wrap_ui(flatpack-portable-builder flatpack-portable-builder.ui)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)