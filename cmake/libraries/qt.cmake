find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools Multimedia Concurrent Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Multimedia Concurrent Charts)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

list(APPEND PROJECT_LIBRARIES_LIST Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Charts)
list(APPEND PROJECT_DIRECTORIES_LIST ${Qt6_INCLUDE_DIRS})
