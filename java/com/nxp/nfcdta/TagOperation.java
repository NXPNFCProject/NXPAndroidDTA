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

import android.nfc.FormatException;
import android.nfc.NdefMessage;
import android.nfc.Tag;
import android.nfc.tech.IsoDep;
import android.nfc.tech.Ndef;
import android.nfc.tech.NfcF;
import android.nfc.tech.NfcV;
import android.util.Log;
import java.io.IOException;
import java.util.Arrays;

public class TagOperation {
  private final String TAG = "DTA_TagOp";
  private final byte[] ISO_DEP_START_LOOPBACK_FRAME = {
      0x00, (byte)0xA4, 0x04, 0x00, 0x0E, '2', 'N', 'F', 'C', '.',
      'S',  'Y',        'S',  '.',  'D',  'D', 'F', '0', '1', 0x00};
  private final byte[] ISO_DEP_END_LOOPBACK_FRAME = {(byte)0xFF, (byte)0xFF,
                                                     (byte)0xFF, 0x01, 0x01};
  private final byte[] T5TUID = {(byte)0xF1, 0x10,       0x32,      0x54, 0x76,
                                 (byte)0x99, (byte)0xAB, (byte)0xE0};
  private final int MAX_T3T_BUFFER_SIZE = 20;
  private enum TAGOPType { UNKNOWN, READ, WRITE, READONLY };
  private enum TAGTYPE { T1T, T2T, T3T, T4T, T5T };
  private String mPatternNo = "0000";
  private TAGOPType mOpType = TAGOPType.UNKNOWN;
  private TAGTYPE mTagType = TAGTYPE.T1T;
  private boolean mNdefStatus = false;
  NdefMessage mNdefMessage;

  public void setPatternNo(final String patternNo) {
    mPatternNo = patternNo;
    generateOperationType(patternNo);
  }

  private void generateOperationType(final String patternNo) {
    switch (patternNo) {
    case "0001":
    case "0011":
    case "0021":
    case "0005":
    case "0006":
    case "0007":
    case "0008":
    case "000A":
    case "000B":
      mOpType = TAGOPType.READ;
      break;
    case "0002":
    case "0012":
      mOpType = TAGOPType.WRITE;
      break;
    case "0003":
      mOpType = TAGOPType.READONLY;
      break;
    default:
      Log.i(TAG, "Invalid Pattern No.");
    }
  }

  public void processDetectedTag(final Tag tag) {
    String[] techList = tag.getTechList();
    Log.i(TAG, "Enter processDetectedTag tech size: " + techList.length);
    if (techList.length > 0) {
      for (String tech : techList) {
        Log.i(TAG, "Tech : " + tech);
        if (tech.equals("android.nfc.tech.Ndef")) {
          mNdefStatus = true;
        } else if (tech.equals("android.nfc.tech.MifareUltralight")) {
          mTagType = TAGTYPE.T2T;
        } else if (tech.equals("android.nfc.tech.NfcF")) {
          mTagType = TAGTYPE.T3T;
        } else if (tech.equals("android.nfc.tech.IsoDep")) {
          mTagType = TAGTYPE.T4T;
        } else if (tech.equals("android.nfc.tech.NfcV")) {
          mTagType = TAGTYPE.T5T;
        }
      }
    }

    if (mTagType == TAGTYPE.T5T) {
      mNdefStatus = false;
      if (mPatternNo.equals("0021") || mPatternNo.equals("0012") || mPatternNo.equals("0001") ||
          mPatternNo.equals("0002") || mPatternNo.equals("0003")) {
        mNdefStatus = true;
      }
    }
    if ((mTagType == TAGTYPE.T3T) && mPatternNo.equals("0000")) {
      // This is a workaround to fulfill TC_DP21_POL_NFCF_UND_BI_1_0_0
      // for CR12. This is ndef related TC and it will be mapped to
      // 0001 in future Certification Releases.
      NfcF nfcF = NfcF.get(tag);
      if (nfcF != null) {
        try {
          nfcF.connect();
          byte[] sendBuf = new byte[MAX_T3T_BUFFER_SIZE];
          int sendBuffLen = 0;
          sendBuf[sendBuffLen++] = (byte)0x00;
          sendBuf[sendBuffLen++] = (byte)0x00;
          sendBuf[sendBuffLen++] = (byte)0x12;
          sendBuf[sendBuffLen++] = (byte)0xFC;
          sendBuf[sendBuffLen++] = (byte)0x00;
          sendBuf[sendBuffLen++] = (byte)0x00;
          sendBuf[0] = (byte)sendBuffLen;
          byte[] finalSendBuf = Arrays.copyOfRange(sendBuf, 0, sendBuffLen);
          nfcF.transceive(finalSendBuf);
        }  catch (IOException e) {
          e.printStackTrace();
        }
      }
    } else if ((mTagType == TAGTYPE.T4T) && mPatternNo.equals("0000")) {
      IsoDep isodep = IsoDep.get(tag);
      if (isodep != null) {
        byte[] response;
        Log.i(TAG, "ISODEP: LOOPBACK STARTED");
        try {
          isodep.connect();
          isodep.setTimeout(60000);
          response = isodep.transceive(ISO_DEP_START_LOOPBACK_FRAME);
          do {
            response = Arrays.copyOfRange(response, 0, response.length - 2);
            if (Arrays.equals(ISO_DEP_END_LOOPBACK_FRAME, response)) {
              Log.i(TAG, "ISODEP: LOOPBACK ENDED");
              break;
            } else {
              response = isodep.transceive(response);
            }
          } while (response.length > 0);
          // isodep.close();
        } catch (IOException e) {
          e.printStackTrace();
        }
      }
    } else if (mNdefStatus) {
      Ndef ndef = Ndef.get(tag);
      if (ndef != null) {
        try {
          ndef.connect();
          if (mOpType == TAGOPType.READ) {
            Log.i(TAG, "Calling getNdefMessage()");
            mNdefMessage = ndef.getNdefMessage();
            Log.i(TAG, "Read data");
          } else if (mOpType == TAGOPType.WRITE) {
            Log.i(TAG, "Calling writeNdefMessage()");
            ndef.writeNdefMessage(mNdefMessage);
            Log.d(TAG, "Written data");
          } else if (mOpType == TAGOPType.READONLY) {
            Log.i(TAG, "Calling makeReadOnly()");
            ndef.makeReadOnly();
          }
          // ndef.close();
        } catch (IOException | FormatException e) {
          e.printStackTrace();
        }
      } else {
        Log.i(TAG, "No Ndef object");
      }
    }
    Log.i(TAG, "Exit processDetectedTag");
  }

  public void clearData() {
    Log.i(TAG, "clearData");
    mNdefStatus = false;
    mTagType = TAGTYPE.T1T;
  }
}
