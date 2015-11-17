/**
 *                    Copyright (c), NXP Semiconductors
 *
 *                       (C)NXP Semiconductors B.V.2014
 *         All rights are reserved. Reproduction in whole or in part is
 *        prohibited without the written consent of the copyright owner.
 *    NXP reserves the right to make changes without notice at any time.
 *   NXP makes no warranty, expressed, implied or statutory, including but
 *   not limited to any implied warranty of merchantability or fitness for any
 *  particular purpose, or that the use will not infringe any third party patent,
 *   copyright or trademark. NXP must not be liable for any loss or damage
 *                            arising from its use.
 *
 */

package com.phdtaui.messenger;

import com.phdtaui.customdialogs.PhCustomSNEPRTD;
import android.app.Service;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;
import android.widget.Toast;

public class PhMessengerService extends Service {
    public static final String MSG_UPDATE_GUI = "MSG_UPDATE_GUI";
    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Bundle data = msg.getData();
            String ndefMessage = data.getString("NDEF_MESSAGE");
            if(ndefMessage == null)
                Log.e("NDEF_MESSAGE", "NDEF IS NULL");
            else
            {
                Log.i("NDEF_MESSAGE", ndefMessage);
                    UpdateGUI(hexaToAscii(ndefMessage).toString());
            }

        }
    }

    final Messenger myMessenger = new Messenger(new IncomingHandler());

    @Override
    public IBinder onBind(Intent intent) {
        return myMessenger.getBinder();
    }

    public StringBuilder hexaToAscii(String ndefMessage) {
        String[] ndefMessageArray = ndefMessage.split("=");
        ndefMessage = ndefMessageArray[ndefMessageArray.length - 1].replace("]", "");
        StringBuilder output = new StringBuilder();
        for (int i = 0; i < ndefMessage.length(); i += 2) {
            String str = ndefMessage.substring(i, i + 2);
            try {
                int hexaValue = Integer.parseInt(str, 16);
                if (hexaValue >= 0x20 && hexaValue <= 0x7E) {
                    output.append((char) Integer.parseInt(str, 16));
                }
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        return output;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void UpdateGUI(String msgStr){
        Intent phCustomSnepRtdIntent = new Intent();
        phCustomSnepRtdIntent.setAction(MSG_UPDATE_GUI);
        phCustomSnepRtdIntent.putExtra("NDEF_MSG_FROM_NFC_SVC", msgStr);
        sendBroadcast(phCustomSnepRtdIntent);
    }
}
