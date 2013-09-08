# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)
include buildinfo.inc

include $(CLEAR_VARS)

APP_ABI := armeabi-v7a armeabi x86 mips
LOCAL_MODULE    := libParticles
LOCAL_CFLAGS    := -Iinc  -DBUILD_NB=$(BUILD) -O3 -DANDROID
LOCAL_SRC_FILES := src/jniCalls.cpp src/utils.cpp src/ParticleEngine.cpp src/Bitmap.cpp src/ShaderProgram.cpp src/GlApp.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 
LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_LDFLAGS += -fuse-ld=bfd




include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)


