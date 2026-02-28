qt_add_executable(escc_test_msp
  test_msp.cpp
  ../src/core/Msp.cpp
)

target_include_directories(escc_test_msp PRIVATE ../src)

target_link_libraries(escc_test_msp PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_msp COMMAND escc_test_msp)

qt_add_executable(escc_test_fourway_constants
  test_fourway_constants.cpp
)

target_include_directories(escc_test_fourway_constants PRIVATE ../src)

target_link_libraries(escc_test_fourway_constants PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_fourway_constants COMMAND escc_test_fourway_constants)

qt_add_executable(escc_test_fourway_frame
  test_fourway_frame.cpp
  ../src/core/FourWay.cpp
  ../src/hardware/Mcu.cpp
  ../src/hardware/Hardware.cpp
  ../src/hardware/Silabs.cpp
  ../src/hardware/Arm.cpp
)

target_include_directories(escc_test_fourway_frame PRIVATE ../src)

target_link_libraries(escc_test_fourway_frame PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_fourway_frame COMMAND escc_test_fourway_frame)

qt_add_executable(escc_test_mcu
  test_mcu.cpp
  ../src/hardware/Mcu.cpp
)

target_include_directories(escc_test_mcu PRIVATE ../src)

target_link_libraries(escc_test_mcu PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_mcu COMMAND escc_test_mcu)

qt_add_executable(escc_test_hardware_ops
  test_hardware_ops.cpp
  ../src/core/FourWay.cpp
  ../src/hardware/Mcu.cpp
  ../src/hardware/Hardware.cpp
  ../src/hardware/Silabs.cpp
  ../src/hardware/Arm.cpp
)

target_include_directories(escc_test_hardware_ops PRIVATE ../src)

target_link_libraries(escc_test_hardware_ops PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_hardware_ops COMMAND escc_test_hardware_ops)

qt_add_executable(escc_test_convert
  test_convert.cpp
  ../src/utils/Convert.cpp
)

target_include_directories(escc_test_convert PRIVATE ../src)

target_link_libraries(escc_test_convert PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_convert COMMAND escc_test_convert)

qt_add_executable(escc_test_flash
  test_flash.cpp
  ../src/utils/Flash.cpp
)

target_include_directories(escc_test_flash PRIVATE ../src)

target_link_libraries(escc_test_flash PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_flash COMMAND escc_test_flash)

qt_add_executable(escc_test_settings
  test_settings.cpp
  ../src/utils/Settings.cpp
)

target_include_directories(escc_test_settings PRIVATE ../src)

target_link_libraries(escc_test_settings PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_settings COMMAND escc_test_settings)

qt_add_executable(escc_test_fourway_helper
  test_fourway_helper.cpp
  ../src/utils/FourWayHelper.cpp
)

target_include_directories(escc_test_fourway_helper PRIVATE ../src)

target_link_libraries(escc_test_fourway_helper PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_fourway_helper COMMAND escc_test_fourway_helper)

qt_add_executable(escc_test_general
  test_general.cpp
  ../src/utils/General.cpp
)

target_include_directories(escc_test_general PRIVATE ../src)

target_link_libraries(escc_test_general PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_general COMMAND escc_test_general)

qt_add_executable(escc_test_http_client
  test_http_client.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_http_client PRIVATE ../src)

target_link_libraries(escc_test_http_client PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_http_client COMMAND escc_test_http_client)

qt_add_executable(escc_test_source
  test_source.cpp
  ../src/sources/Source.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_source PRIVATE ../src)

target_link_libraries(escc_test_source PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_source COMMAND escc_test_source)

qt_add_executable(escc_test_github_source
  test_github_source.cpp
  ../src/sources/GithubSource.cpp
  ../src/sources/Source.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_github_source PRIVATE ../src)

target_link_libraries(escc_test_github_source PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_github_source COMMAND escc_test_github_source)

qt_add_executable(escc_test_bluejay_source
  test_bluejay_source.cpp
  ../src/sources/BluejaySource.cpp
  ../src/sources/GithubSource.cpp
  ../src/sources/Source.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_bluejay_source PRIVATE ../src)

target_link_libraries(escc_test_bluejay_source PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_bluejay_source COMMAND escc_test_bluejay_source)

qt_add_executable(escc_test_am32_source
  test_am32_source.cpp
  ../src/sources/AM32Source.cpp
  ../src/sources/GithubSource.cpp
  ../src/sources/Source.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_am32_source PRIVATE ../src)

target_link_libraries(escc_test_am32_source PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_am32_source COMMAND escc_test_am32_source)

qt_add_executable(escc_test_blheli_sources
  test_blheli_sources.cpp
  ../src/sources/BLHeliSource.cpp
  ../src/sources/BLHeliSSource.cpp
  ../src/sources/BLHeliAtmelSource.cpp
  ../src/sources/BLHeliSilabsSource.cpp
  ../src/sources/Source.cpp
  ../src/network/HttpClient.cpp
)

target_include_directories(escc_test_blheli_sources PRIVATE ../src)

target_link_libraries(escc_test_blheli_sources PRIVATE
  Qt6::Core
  Qt6::Network
  Qt6::Test
)

add_test(NAME escc_test_blheli_sources COMMAND escc_test_blheli_sources)

qt_add_executable(escc_test_source_data_loader
  test_source_data_loader.cpp
  ../src/sources/SourceDataLoader.cpp
)

target_include_directories(escc_test_source_data_loader PRIVATE ../src)

target_link_libraries(escc_test_source_data_loader PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_source_data_loader COMMAND escc_test_source_data_loader)

qt_add_executable(escc_test_models_types
  test_models_types.cpp
)

target_include_directories(escc_test_models_types PRIVATE ../src)

target_link_libraries(escc_test_models_types PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_models_types COMMAND escc_test_models_types)

qt_add_executable(escc_test_app_state
  test_app_state.cpp
  ../src/models/AppState.cpp
)

target_include_directories(escc_test_app_state PRIVATE ../src)

target_link_libraries(escc_test_app_state PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_app_state COMMAND escc_test_app_state)

qt_add_executable(escc_test_esc_list_model
  test_esc_list_model.cpp
  ../src/models/EscListModel.cpp
)

target_include_directories(escc_test_esc_list_model PRIVATE ../src)

target_link_libraries(escc_test_esc_list_model PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_esc_list_model COMMAND escc_test_esc_list_model)

qt_add_executable(escc_test_settings_model
  test_settings_model.cpp
  ../src/models/SettingsModel.cpp
)

target_include_directories(escc_test_settings_model PRIVATE ../src)

target_link_libraries(escc_test_settings_model PRIVATE
  Qt6::Core
  Qt6::Test
)

add_test(NAME escc_test_settings_model COMMAND escc_test_settings_model)
