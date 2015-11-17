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
