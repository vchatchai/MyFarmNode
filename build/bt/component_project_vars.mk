# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/bt/include $(IDF_PATH)/components/bt/include/esp32/include $(IDF_PATH)/components/bt/common/api/include/api $(IDF_PATH)/components/bt/common/btc/profile/esp/blufi/include $(IDF_PATH)/components/bt/common/btc/profile/esp/include $(IDF_PATH)/components/bt/common/osi/include $(IDF_PATH)/components/bt/host/bluedroid/api/include/api
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/bt -lbt -L $(IDF_PATH)/components/bt/controller/lib_esp32/esp32 -lbtdm_app
COMPONENT_LINKER_DEPS += $(IDF_PATH)/components/bt/controller/lib_esp32/esp32/libbtdm_app.a
COMPONENT_SUBMODULES += $(IDF_PATH)/components/bt/controller/lib_esp32
COMPONENT_LIBRARIES += bt
COMPONENT_LDFRAGMENTS += 
component-bt-build: 
