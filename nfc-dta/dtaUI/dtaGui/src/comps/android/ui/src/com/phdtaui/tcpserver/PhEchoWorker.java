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

package com.phdtaui.tcpserver;

import java.nio.channels.SocketChannel;
import java.util.LinkedList;
import java.util.List;

import android.util.Log;

import com.phdtaui.utils.PhUtilities;

public class PhEchoWorker implements Runnable {
    private List queue = new LinkedList();
     public void processData(PhNioServer server, SocketChannel socket, byte[] data, int count) {
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside Process Data event");
        byte[] dataCopy = new byte[count];
        System.arraycopy(data, 0, dataCopy, 0, count);
        synchronized(queue) {
            queue.add(new PhServerDataEvent(server, socket, dataCopy));
            queue.notify();
        }
    }
     public void run() {
        PhServerDataEvent dataEvent;
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside Run");
        while(true) {
            // Wait for data to become available
            synchronized(queue) {
                while(queue.isEmpty()) {
                    try {
                        queue.wait();
                    } catch (InterruptedException e) {
                    }
                }
                dataEvent = (PhServerDataEvent) queue.remove(0);
            }
                     // Return to sender
        //    dataEvent.server.send(dataEvent.socket, dataEvent.data); -- Bhanu
        }
    }
}
