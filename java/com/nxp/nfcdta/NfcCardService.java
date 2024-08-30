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

import android.nfc.cardemulation.HostApduService;
import android.os.Bundle;
import android.util.Log;
import java.util.Arrays;

public class NfcCardService extends HostApduService {
  private static final String TAG = "DTA_CE";
  private static final int MAX_LOOPBACK_DATABUF_SIZE = 4112;
  // Select AID
  private static final byte[] DATA_EVENT1 = {(byte)0x00, (byte)0xa4, (byte)0x04,
                                     (byte)0x00};
  // CAPDU PACKET DATA_EVENT2
  private static final byte[] DATA_EVENT2 = {(byte)0x80, (byte)0xa8, (byte)0x00,
                                     (byte)0x00};
  // CAPDU PACKET  DATA_EVENT3
  private static final byte[] DATA_EVENT3 = {(byte)0x00, (byte)0xb2, (byte)0x01,
                                     (byte)0x0c};
  // RAPDU PACKET  DATA_EVENT4
  private static final byte[] DATA_EVENT4 = {(byte)0x00, (byte)0xb2, (byte)0x02,
                                     (byte)0x0c};
  // LOOP BACK DATA CHECK
  private static final byte[] DATA_EVENT5 = {(byte)0x80, (byte)0xee, (byte)0x00,
                                     (byte)0x00};
  private static final byte[] P_AidData = {(byte)0x01, (byte)0x00, (byte)0x90,
                                     (byte)0x00};
  private static final byte[] CApduRespData = {(byte)0x80, (byte)0xA8, (byte)0x00,
                                     (byte)0x00,(byte)0x01,(byte)0x02};
  private static final byte[] CmdRespXchg1 = {(byte)0x00, (byte)0xB2, (byte)0x01,
                                     (byte)0x0C,(byte)0x00};
  private static final byte[] CmdRespXchg2 = {(byte)0x00, (byte)0xB2, (byte)0x02,
                                     (byte)0x0C,(byte)0x00};
  private static final byte[] StatusWordData = {(byte)0x90, (byte)0x00};

  @Override
  public void onDeactivated(int reason) {
    Log.i(TAG, "onDeactivated with reason: " + Integer.toString(reason));
  }

  @Override
  public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
    byte[] responseApdu = new byte[MAX_LOOPBACK_DATABUF_SIZE];

    if (!MainActivity.getTestStatus() || commandApdu.length<=0) {
      return responseApdu;
    }

    int responseApduLen = 0;

    byte[] dataHeader = Arrays.copyOfRange(commandApdu, 0, 4);
    if (Arrays.equals(DATA_EVENT1, dataHeader)) {
      responseApdu = Arrays.copyOf(P_AidData, P_AidData.length);
      responseApduLen = P_AidData.length;
    } else if (Arrays.equals(DATA_EVENT2, dataHeader)) {
      responseApdu = Arrays.copyOf(CApduRespData, CApduRespData.length);
      responseApduLen = CApduRespData.length;
    } else if (Arrays.equals(DATA_EVENT3, dataHeader)) {
      responseApdu = Arrays.copyOf(CmdRespXchg1, CmdRespXchg1.length);
      responseApduLen = CmdRespXchg1.length;
    } else if (Arrays.equals(DATA_EVENT4, dataHeader)) {
      responseApdu = Arrays.copyOf(CmdRespXchg2, CmdRespXchg2.length);
      responseApduLen = CmdRespXchg2.length;
    } else if (Arrays.equals(DATA_EVENT5, dataHeader)) {
      for (int i = 0; i < commandApdu.length; i++) {
        responseApdu[i] = commandApdu[i];
      }
      responseApduLen = commandApdu.length;
      responseApdu[responseApduLen++] = (byte)StatusWordData[0];
      responseApdu[responseApduLen++] = (byte)StatusWordData[1];
    }

    byte[] finalResponse =
        Arrays.copyOfRange(responseApdu, 0, responseApduLen);
    MainActivity.printPacket(TAG, commandApdu, finalResponse);

    return finalResponse;
  }
}
