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


import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.tcpserver.PhEchoWorker;
import com.phdtaui.tcpserver.PhNioServer;
import com.phdtaui.utils.PhUtilities;

import android.os.AsyncTask;
import android.util.Log;

public class PhNioServerTask extends AsyncTask<Void, Void, Void> {

    private int portNumber;
    private PhDTAUIMainActivity phDTAUIMainActivityObjRef;

    public PhNioServerTask(PhDTAUIMainActivity phDTAUIMainActivityObj,int portNumber) {
        this.portNumber = portNumber;
        this.phDTAUIMainActivityObjRef = phDTAUIMainActivityObj;
    }

    @Override
    protected Void doInBackground(Void... params) {
        PhNioServer nioServer = new PhNioServer(phDTAUIMainActivityObjRef);
        nioServer.mainTCPServer(portNumber);
        return null;
    }
}
