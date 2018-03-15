/*
* Copyright (C) 2015-2018 NXP Semiconductors
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.phdtaui.utils;

public class PhUtilities {

    /**  To Disable wave II and info checkbox make below WAVE_TWO_N_INFO flag to false */
    public static boolean WAVE_TWO_INFO_N_LOG=true;
    public static boolean isExitChecked = false;
    public static String exitValue = "exit not clicked";
    public static boolean NDK_IMPLEMENTATION = true;
    public static boolean DTA_DEBUG_MODE = true;
    public static String UI_TAG = "UIDTA>";
    public static String TCPCNT_TAG = "TCPCLIENT>";
    public static String TCPSRV_TAG = "TCPSERVER>";
    public static String PROTOCOL_TAG = "PROTOCOL>";

    /** Appear disappear and fading AUTO_MODE */
    public static boolean APPEAR_AUTO_MODE = true;
    public static boolean DISAPPER_AUTO_MODE = false;
    public static boolean FADING_AUTO_MODE = false;

    /** Appear disappear and fading SHOW_MESSAGE */
    public static boolean APPEAR_SHOW_MESSAGE = true;
    public static boolean DISAPPER_SHOW_MESSAGE = false;
    public static boolean FADING_SHOW_MESSAGE = false;

    /** Appear disappear and fading CUSTOM_MESSAGE */
    public static boolean APPEAR_CUSTOM_MESSAGE = true;
    public static boolean DISAPPER_CUSTOM_MESSAGE = false;
    public static boolean FADING_CUSTOM_MESSAGE = false;

    /** Appear disappear and fading TCP_IP_STATUS */
    public static boolean APPEAR_TCP_IP_STATUS = true;
    public static boolean DISAPPER_TCP_IP_STATUS = false;
    public static boolean FADING_TCP_IP_STATUS = false;

    /** Appear disappear and fading V_RF_POLL_TECHNOLOGY */
    public static boolean APPEAR_V_RF_POLL_TECHNOLOGY = false;
    public static boolean DISAPPER_V_RF_POLL_TECHNOLOGY = false;
    public static boolean FADING_V_RF_POLL_TECHNOLOGY = true;

    /** Appear disappear and fading V_RF_LISTEN_TECHNOLOGY */
    public static boolean APPEAR_V_RF_LISTEN_TECHNOLOGY = false;
    public static boolean DISAPPER_V_RF_LISTEN_TECHNOLOGY = false;
    public static boolean FADING_V_RF_LISTEN_TECHNOLOGY = true;

    /** Appear disappear and fading Card Emulation(UICC) */
    public static boolean APPEAR_UICC_CARD_EMULATION = true;
    public static boolean DISAPPER_UICC_CARD_EMULATION = false;
    public static boolean FADING_UICC_CARD_EMULATION = false;

    /** Appear disappear and fading Card Emulation(HCE) */
    public static boolean APPEAR_HCE_CARD_EMULATION = true;
    public static boolean DISAPPER_HCE_CARD_EMULATION = false;
    public static boolean FADING_HCE_CARD_EMULATION = false;

    /** Appear disappear and fading P2P(LLCP) */
    public static boolean APPEAR_LLCP_P2P = true;
    public static boolean DISAPPER_LLCP_P2P = false;
    public static boolean FADING_LLCP_P2P = false;

    /** Appear disappear and fading P2P(SNEP) */
    public static boolean APPEAR_SNEP_P2P = true;
    public static boolean DISAPPER_SNEP_P2P = false;
    public static boolean FADING_SNEP_P2P = false;

    /** Appear disappear and fading P2P(DEPI) */
    public static boolean APPEAR_DEPI_P2P = false;
    public static boolean DISAPPER_DEPI_P2P = false;
    public static boolean FADING_DEPI_P2P = true;

    /** Appear disappear and fading P2P(DEPT) */
    public static boolean APPEAR_DEPT_P2P = false;
    public static boolean DISAPPER_DEPT_P2P = false;
    public static boolean FADING_DEPT_P2P = true;

    /** Appear disappear Log Message */
    public static boolean APPEAR_LOG_MESSAGE = true;
    public static boolean DISAPPER_LOG_MESSAGE = false;
    public static boolean FADING_LOG_MESSAGE = false;

    /** Appear disappear Device */
    public static boolean APPEAR_DEVICE = true;
    public static boolean DISAPPER_DEVICE = false;
    public static boolean FADING_DEVICE = false;

    /** Appear disappear Device INFO */
    public static boolean APPEAR_DEVICE_INFO = true;
    public static boolean DISAPPER_DEVICE_INFO = false;
    public static boolean FADING_DEVICE_INFO = false;

    /** Appear disappear card emulation */
    public static boolean APPEAR_CARD_EMULATION = true;
    public static boolean DISAPPER_CARD_EMULATION = false;
    public static boolean FADING_CARD_EMULATION = false;

    /** Appear disappear P2P */
    public static boolean APPEAR_P2P = true;
    public static boolean DISAPPER_P2P = false;
    public static boolean FADING_CARD_P2P = false;

    /** Appear disappear Custom Edit Text */
    public static boolean APPEAR_CUSTOM = true;
    public static boolean DISAPPER_CUSTOM = false;
    public static boolean FADING_CUSTOM = false;

    /** To Check if Server or Client Radio Button is enabled */
    public static boolean CLIENT_SELECTED = false;
    public static boolean SERVER_SELECTED = false;
    public static boolean SIMULATED_SERVER_SELECTED = false;
    public static boolean SIMULATED_CLIENT_SELECTED = false;

    /** Check if DUT and LT Messages has to be logged */
    public static boolean LOG_DUT_MESSAGES = false;
    public static boolean LOG_LT_MESSAGES = false;
    public static String FILE_NAME_DUT = "";
    public static String FILE_NAME_LT = "";

    /** Decoding Message Buffers in Auto Mode */
    public static String ltType;
    public static String executeOrResultBuff;
    public static String testCaseIdBuff;
    public static String patternNUmberBuff = "";
    public static String cardEmulationModeBuff = "0";
    public static String rfu1Buff = "";
    public static String rfu2Buff = "";

    /** TCPIPCONNECTIONSTATUS */
    public static boolean TCPIPCONNECTIONSTATUS;
    public static volatile String TCPIPConnectionStateText = "Closed";

    /**
     * to check whether server is running when Client is selected in Auto Mode
     */
    public static boolean isServerRunning = true;

    /**
     * Initializing count of retries to server
     */
    public static int countOfRetry = 0;

    /**
     * Boolean variable to know if client is connected to server
     */
    public static boolean isClientConnected = false;

    /**
     * number of retries to Server to be made
     */
    public static int maxNumberOfRetries;
    /**
     * Boolean value to know status of CRC Check status
     */
    public static boolean isCRCCheckFailed = false;
    public static int STOP_BUTTON_HANDLE=0xFFFF;
}
