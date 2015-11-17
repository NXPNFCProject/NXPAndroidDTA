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

import android.util.Log;

import com.phdtaui.utils.PhUtilities;

class PhServerDataEvent {
    public PhNioServer server;
    public SocketChannel socket;
    public byte[] data;
     public PhServerDataEvent(PhNioServer server, SocketChannel socket, byte[] data) {
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside Server Data Event");
        this.server = server;
        this.socket = socket;
        this.data = data;
    }
}
