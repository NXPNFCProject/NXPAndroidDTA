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
