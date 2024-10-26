LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := MenuVSL
LOCAL_CPP_FEATURES += exceptions
LOCAL_SRC_FILES := main.cpp mod/logger.cpp mod/config.cpp Log.cpp MenuVSL.cpp Window.cpp Item.cpp Input.cpp Debug.cpp sdk/Image.cpp ModConfig.cpp Vehicles.cpp Popup.cpp Localization.cpp Messages.cpp
LOCAL_SRC_FILES += json_reader.cpp json_value.cpp json_writer.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_C_INCLUDES += ./include
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)