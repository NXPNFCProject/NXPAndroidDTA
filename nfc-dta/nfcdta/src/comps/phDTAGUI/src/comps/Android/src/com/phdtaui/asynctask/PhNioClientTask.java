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

package com.phdtaui.asynctask;

import java.io.IOException;
import java.net.InetAddress;

import android.os.AsyncTask;

import com.phdtaui.tcpclient.PhNioClient;
import com.phdtaui.mainactivity.PhDTAUIMainActivity;

public class PhNioClientTask extends AsyncTask<Void, Void, Void> {

    private InetAddress hostAddress;
    private int port;
    private PhDTAUIMainActivity phDTAUIMainActivityObjRef;

    public PhNioClientTask(PhDTAUIMainActivity phDTAUIMainActivityObj,InetAddress hostAddress, int port) {
        this.hostAddress = hostAddress;
        this.port = port;
        this.phDTAUIMainActivityObjRef = phDTAUIMainActivityObj;
    }

    @Override
    protected Void doInBackground(Void... params) {
        try {
            PhNioClient nioClient = new PhNioClient(phDTAUIMainActivityObjRef,hostAddress, port);
            nioClient.mainTCPClient();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
