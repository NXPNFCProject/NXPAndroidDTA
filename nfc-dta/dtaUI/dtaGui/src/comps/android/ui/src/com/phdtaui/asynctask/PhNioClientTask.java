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
