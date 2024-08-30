/*
* Copyright 2024 NXP
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

package com.nxp.nfcdta;

import android.nfc.cardemulation.HostNfcFService;
import android.os.Bundle;
import android.util.Log;
import java.util.Arrays;

public class NfcFCardService extends HostNfcFService {
  private static final String TAG = "DTA_CEF";
  private static final int BUFFER_SIZE = 400;
  private static final int T3T_NFCID2_SIZE = 8;
  private static final int BLOCK_NUM1 = 14;
  private static final int BLOCK_NUM2 = 15;
  private static final int BLOCK_NUM3 = 16;
  private static final int CHECK_CMD = 0X06;
  private static final int UPDATE_CMD = 0X08;
  private static byte[] readUpdateCmdBuf = new byte[BUFFER_SIZE];
  private static byte checkCmdMask = (byte)0x80;
  private static byte blockNum = 0x00;

  @Override
  public void onDeactivated(int reason) {
    Log.i(TAG, "onDeactivated with reason: " + Integer.toString(reason));
  }
  @Override
  public byte[] processNfcFPacket(byte[] packet, Bundle bundle) {
    byte[] sendBuff = new byte[BUFFER_SIZE];

    if (!MainActivity.getTestStatus() || packet.length <= 0) {
      return sendBuff;
    }

    int sendBuffLen = 0;
    int count = 0;
    if (packet[1] == UPDATE_CMD) {
      Log.i(TAG,"Update command received");
      readUpdateCmdBuf = packet.clone();
      blockNum = packet[BLOCK_NUM1];
      sendBuff[sendBuffLen++] = 0x00;
      sendBuff[sendBuffLen++] = 0x09;
      for (int i = 0; i < T3T_NFCID2_SIZE; i++) {
        sendBuff[sendBuffLen] = packet[sendBuffLen];
        sendBuffLen++;
      }
      sendBuff[sendBuffLen++] = 0x00;
      sendBuff[sendBuffLen++] = 0x00;
      sendBuff[0] = (byte)sendBuffLen;
    } else if (packet[1] == CHECK_CMD) { // checking the CHECK cmd
      if (((blockNum & checkCmdMask) > 0) && (packet.length >= 16) &&
          (readUpdateCmdBuf[14] == packet[BLOCK_NUM1]) &&
          (readUpdateCmdBuf[15] == packet[BLOCK_NUM2])) {
        Log.i(TAG, "Condition #1 preparing buffer to send check response with block list is 2 bytes");
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x07;
        for (int i = 0; i < T3T_NFCID2_SIZE; i++) {
          sendBuff[sendBuffLen] = packet[sendBuffLen];
          sendBuffLen++;
        }
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x01;
        for (int i = 0; i < 16; i++) {
          sendBuff[sendBuffLen++] = readUpdateCmdBuf[16 + i];
        }
        sendBuff[0] = (byte)sendBuffLen;
      } else if (((blockNum & checkCmdMask) == 0) &&
                 (packet.length >= 17) &&
                 (readUpdateCmdBuf[14] == packet[BLOCK_NUM1]) &&
                 (readUpdateCmdBuf[15] == packet[BLOCK_NUM2]) &&
                 (readUpdateCmdBuf[16] == packet[BLOCK_NUM3])) {
        Log.i(TAG, "Condition #2 preparing buffer to send check response with block list is 3 bytes");
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x07;
        for (int i = 0; i < T3T_NFCID2_SIZE; i++) {
          sendBuff[sendBuffLen] = packet[sendBuffLen];
          sendBuffLen++;
        }
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x01;
        for (int i = 0; i < 16; i++) {
          sendBuff[sendBuffLen++] = readUpdateCmdBuf[17 + i];
        }
        sendBuff[0] = (byte)sendBuffLen;
      } else {
        Log.i(TAG, "Condition #3 Error in block size or check command called before update command!!");
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x07;
        for (int i = 0; i < T3T_NFCID2_SIZE; i++) {
          sendBuff[sendBuffLen] = packet[sendBuffLen];
          sendBuffLen++;
        }
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x00;
        sendBuff[sendBuffLen++] = 0x01;
        for (int i = 0; i < 16; i++) {
          sendBuff[sendBuffLen++] = (byte)0xFF;
        }
        sendBuff[0] = (byte)sendBuffLen;
      }
    }


    byte[] finalResponse = Arrays.copyOfRange(sendBuff, 0, sendBuffLen);
    MainActivity.printPacket(TAG, packet, finalResponse);

    return finalResponse;

  }
}
