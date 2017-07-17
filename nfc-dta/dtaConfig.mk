#
#  Copyright (C) 2015 NXP Semiconductors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#########Build DTA Application and dependent modules#############################
$(info "NXP-NFC-DTA>Set DTA macros..")
FLAG_NXP_HAL_EXTNS := true
THREAD_PRIO_SUPPORT:= false
ANDROID_O          := true
AOSP_MASTER        := false
NCI_2_0            := true
PN81A              := false

D_CFLAGS := -DANDROID -DBUILDCFG=1

D_CFLAGS += -DNXP_EXTNS=TRUE
D_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE
D_CFLAGS += -DNFC_NXP_P2P_PERFORMANCE_TESTING=FALSE
D_CFLAGS += -DNFC_NXP_ESE=TRUE

#NXP PN547 Enable
#Chip selection
#variables for NFC_NXP_CHIP_TYPE
PN547C2 := 1
PN548AD := 2
PN551   := 3
PN553   := 4
PN557   := 5

ifeq ($(PN547C2),1)
D_CFLAGS += -DPN547C2=1
endif
ifeq ($(PN548AD),2)
D_CFLAGS += -DPN548AD=2
endif
ifeq ($(PN551),3)
D_CFLAGS += -DPN551=3
endif
ifeq ($(PN553),4)
D_CFLAGS += -DPN553=4
endif
ifeq ($(PN557),5)
D_CFLAGS += -DPN557=5
endif

#### Select the CHIP ####
NXP_CHIP_TYPE := $(PN557)

ifeq ($(NXP_CHIP_TYPE),$(PN547C2))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=$(PN547C2)
else ifeq ($(NXP_CHIP_TYPE),$(PN548AD))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=$(PN548AD)
else ifeq ($(NXP_CHIP_TYPE),$(PN551))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=$(PN551)
else ifeq ($(NXP_CHIP_TYPE),$(PN553))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=$(PN553)
else ifeq ($(NXP_CHIP_TYPE),$(PN557))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=$(PN557)
endif

ifeq ($(ANDROID_O), true)
    D_CFLAGS += -DANDROID_O=TRUE
endif
ifeq ($(AOSP_MASTER), true)
    D_CFLAGS += -DAOSP_MASTER_COMPILATION_SUPPORT=TRUE
endif
ifeq ($(NCI_2_0), true)
    D_CFLAGS += -DNCI_2_0=TRUE
endif
ifeq ($(THREAD_PRIO_SUPPORT), true)
    D_CFLAGS += -DTHREAD_PRIO_SUPPORT=TRUE
endif
ifeq ($(PN81A), true)
    D_CFLAGS += -DPN81A=TRUE
endif

