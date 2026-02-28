cmake_minimum_required(VERSION 3.21)

project(escc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(ESCC_ENABLE_CLANG_TIDY "Enable clang-tidy analysis" ON)
option(ESCC_ENABLE_CPPCHECK "Enable cppcheck analysis" ON)

if(ESCC_ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)
  if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
  endif()
endif()

if(ESCC_ENABLE_CPPCHECK)
  find_program(CPPCHECK_EXE NAMES cppcheck)
  if(CPPCHECK_EXE)
    set(CMAKE_CXX_CPPCHECK
      "${CPPCHECK_EXE};--enable=warning,performance,portability;--inline-suppr;--suppress=unknownMacro;--suppress=preprocessorErrorDirective;--error-exitcode=2")
  endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test)
find_package(Qt6 QUIET COMPONENTS LinguistTools)
find_package(Qt6 QUIET COMPONENTS SerialPort)

qt_standard_project_setup()

set(ESCC_SOURCES
  main.cpp
  resources/resources.qrc
  src/ui/MainWindow.cpp
  src/ui/HomeWidget.cpp
  src/ui/ButtonBarWidget.cpp
  src/ui/FlashWidget.cpp
  src/ui/CommonSettingsWidget.cpp
  src/ui/PortPickerWidget.cpp
  src/serial/SerialPort.cpp
  src/serial/QueueProcessor.cpp
  src/serial/Serial.cpp
  src/core/Msp.cpp
  src/core/FourWay.cpp
  src/hardware/Mcu.cpp
  src/hardware/Hardware.cpp
  src/hardware/Silabs.cpp
  src/hardware/Arm.cpp
  src/network/HttpClient.cpp
  src/sources/Source.cpp
  src/sources/GithubSource.cpp
  src/sources/BluejaySource.cpp
  src/sources/AM32Source.cpp
  src/sources/BLHeliSource.cpp
  src/sources/BLHeliSSource.cpp
  src/sources/BLHeliAtmelSource.cpp
  src/sources/BLHeliSilabsSource.cpp
  src/sources/SourceDataLoader.cpp
  src/models/AppState.cpp
  src/models/EscListModel.cpp
  src/models/SettingsModel.cpp
  src/utils/Convert.cpp
  src/utils/Flash.cpp
  src/utils/Settings.cpp
  src/utils/FourWayHelper.cpp
  src/utils/General.cpp
)

set(ESCC_HEADERS
  src/ui/MainWindow.hpp
  src/ui/HomeWidget.hpp
  src/ui/ButtonBarWidget.hpp
  src/ui/FlashWidget.hpp
  src/ui/CommonSettingsWidget.hpp
  src/ui/PortPickerWidget.hpp
  src/serial/SerialPort.hpp
  src/serial/QueueProcessor.hpp
  src/serial/Serial.hpp
  src/core/Msp.hpp
  src/core/FourWay.hpp
  src/hardware/Mcu.hpp
  src/hardware/Hardware.hpp
  src/hardware/Silabs.hpp
  src/hardware/Arm.hpp
  src/network/HttpClient.hpp
  src/sources/Source.hpp
  src/sources/GithubSource.hpp
  src/sources/BluejaySource.hpp
  src/sources/AM32Source.hpp
  src/sources/BLHeliSource.hpp
  src/sources/BLHeliSSource.hpp
  src/sources/BLHeliAtmelSource.hpp
  src/sources/BLHeliSilabsSource.hpp
  src/sources/SourceDataLoader.hpp
  src/models/AppState.hpp
  src/models/EscListModel.hpp
  src/models/SettingsModel.hpp
  src/models/Types.hpp
  src/utils/Convert.hpp
  src/utils/Flash.hpp
  src/utils/Settings.hpp
  src/utils/FourWayHelper.hpp
  src/utils/General.hpp
)

qt_add_executable(escc
  MANUAL_FINALIZATION
  ${ESCC_SOURCES}
  ${ESCC_HEADERS}
)

target_include_directories(escc PRIVATE src)

target_link_libraries(escc PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::Network
)

if(TARGET Qt6::SerialPort)
  target_link_libraries(escc PRIVATE Qt6::SerialPort)
  target_compile_definitions(escc PRIVATE ESCC_HAS_QT_SERIALPORT=1)
else()
  message(WARNING "Qt6 SerialPort component not found. Building without hardware serial support.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(escc PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_subdirectory(tests)

qt_finalize_executable(escc)
