COMPONENT_ADD_INCLUDEDIRS := include lib/include lib/esp32-camera/conversions/include
COMPONENT_PRIV_INCLUDEDIRS := lib/esp32-camera/driver/private_include lib/esp32-camera/conversions/private_include lib/esp32-camera/sensors/private_include
COMPONENT_SRCDIRS := . lib lib/esp32-camera/conversions lib/esp32-camera/driver lib/esp32-camera/sensors
CXXFLAGS += -fno-rtti
