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
find_package(Qt6 QUIET COMPONENTS Multimedia)

set(ESCC_TRANSLATION_TS_FILES
  src/i18n/escc_cs.ts
  src/i18n/escc_de.ts
  src/i18n/escc_en.ts
  src/i18n/escc_es.ts
  src/i18n/escc_fr.ts
  src/i18n/escc_it.ts
  src/i18n/escc_pl.ts
  src/i18n/escc_ru.ts
  src/i18n/escc_tr.ts
  src/i18n/escc_uk.ts
  src/i18n/escc_zh_cn.ts
  src/i18n/escc_zh_tw.ts
)

qt_standard_project_setup()

set(ESCC_SOURCES
  main.cpp
  resources/resources.qrc
  src/ui/MainWindow.cpp
  src/ui/HomeWidget.cpp
  src/ui/ButtonBarWidget.cpp
  src/ui/AppSettingsDialog.cpp
  src/ui/ChangelogDialog.cpp
  src/ui/FlashWidget.cpp
  src/ui/CommonSettingsWidget.cpp
  src/ui/EscWidget.cpp
  src/ui/FirmwareSelectorDialog.cpp
  src/ui/LogWidget.cpp
  src/ui/MelodyEditorWidget.cpp
  src/ui/MotorControlWidget.cpp
  src/ui/OverlayWidget.cpp
  src/ui/PortPickerWidget.cpp
  src/ui/StatusBarWidget.cpp
  src/ui/Input/LabeledSelect.cpp
  src/ui/Input/Checkbox.cpp
  src/ui/Input/Number.cpp
  src/ui/Input/Slider.cpp
  src/ui/Input/Info.cpp
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
  src/i18n/LanguageManager.cpp
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
  src/ui/AppSettingsDialog.hpp
  src/ui/ChangelogDialog.hpp
  src/ui/FlashWidget.hpp
  src/ui/CommonSettingsWidget.hpp
  src/ui/EscWidget.hpp
  src/ui/FirmwareSelectorDialog.hpp
  src/ui/LogWidget.hpp
  src/ui/MelodyEditorWidget.hpp
  src/ui/MotorControlWidget.hpp
  src/ui/OverlayWidget.hpp
  src/ui/PortPickerWidget.hpp
  src/ui/StatusBarWidget.hpp
  src/ui/Input/LabeledSelect.hpp
  src/ui/Input/Checkbox.hpp
  src/ui/Input/Number.hpp
  src/ui/Input/Slider.hpp
  src/ui/Input/Info.hpp
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
  src/i18n/LanguageManager.hpp
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

if(TARGET Qt6::Multimedia)
  target_link_libraries(escc PRIVATE Qt6::Multimedia)
  target_compile_definitions(escc PRIVATE ESCC_HAS_QT_MULTIMEDIA=1)
else()
  message(WARNING "Qt6 Multimedia component not found. Melody preview will be disabled.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(escc PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(COMMAND qt_add_translations)
  qt_add_translations(escc TS_FILES ${ESCC_TRANSLATION_TS_FILES})
elseif(TARGET Qt6::LinguistTools)
  message(WARNING "Qt LinguistTools found but qt_add_translations() is unavailable; skipping .qm generation.")
else()
  message(WARNING "Qt6 LinguistTools component not found. Translation .qm files will not be generated.")
endif()

enable_testing()
add_subdirectory(tests)

qt_finalize_executable(escc)
