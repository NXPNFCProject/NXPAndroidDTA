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
