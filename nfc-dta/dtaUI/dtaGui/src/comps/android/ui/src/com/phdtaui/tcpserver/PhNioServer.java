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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import android.os.AsyncTask;
import android.util.Log;

import com.phdtaui.asynctask.PhNioClientTask;
import com.phdtaui.helper.PhNXPHelperMainActivity;
import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.structure.PhDtaLibStructure;
import com.phdtaui.tcpclient.PhChangeRequest;
import com.phdtaui.helper.PhNXPJniHelper;
import com.phdtaui.tcpclient.PhRspHandler;
import com.phdtaui.utils.PhUtilities;

public class PhNioServer implements Runnable {
    // The host:port combination to listen on
    private InetAddress hostAddress;
    private int portNum;

    // The channel on which we'll accept connections
    private ServerSocketChannel serverChannel;

    // The selector we'll be monitoring
    private Selector selector;

    // The buffer into which we'll read data when it's available
    private ByteBuffer readBuffer = ByteBuffer.allocate(1024);

    private PhEchoWorker worker;
    private PhDTAUIMainActivity phDTAUIMainActivityObjRef;
    /**
     * to check whether Hello Message received from LT
     */
    private boolean isHelloMessageReceived = false;

    // A list of PendingChange instances
    private List pendingChanges = new LinkedList();

    // Maps a SocketChannel to a list of ByteBuffer instances
    private Map pendingData = new HashMap();

    public PhNioServer(PhDTAUIMainActivity phDTAUIMainActivityObj) {
        this.phDTAUIMainActivityObjRef = phDTAUIMainActivityObj;
    }

    public PhNioServer(PhDTAUIMainActivity phDTAUIMainActivityObj,InetAddress hostAddress, int port, PhEchoWorker worker)
            throws IOException {
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside NIO Server Constructor");
        this.hostAddress = hostAddress;
        this.portNum = port;
        this.selector = this.initSelector();
        this.worker = worker;
        this.phDTAUIMainActivityObjRef = phDTAUIMainActivityObj;
    }

    public void send(SocketChannel socket, byte[] data) {
        synchronized (this.pendingChanges) {
            // Indicate we want the interest ops set changed
            this.pendingChanges.add(new PhChangeRequest(socket,
                    PhChangeRequest.CHANGEOPS, SelectionKey.OP_WRITE));

            // And queue the data we want written
            synchronized (this.pendingData) {
                List queue = (List) this.pendingData.get(socket);
                if (queue == null) {
                    queue = new ArrayList();
                    this.pendingData.put(socket, queue);
                }
                queue.add(ByteBuffer.wrap(data));
            }
        }

        // Finally, wake up our selecting thread so it can make the required
        // changes
        this.selector.wakeup();
    }

    public void run() {
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside Run Mehtod");
        while (true) {
            try {
                // Process any pending changes
                synchronized (this.pendingChanges) {
                    Iterator changes = this.pendingChanges.iterator();
                    while (changes.hasNext()) {
                        PhChangeRequest change = (PhChangeRequest) changes.next();
                        switch (change.type) {
                        case PhChangeRequest.CHANGEOPS:
                            SelectionKey key = change.socket
                                    .keyFor(this.selector);
                            key.interestOps(change.ops);
                        }
                    }
                    this.pendingChanges.clear();
                }

                // Wait for an event one of the registered channels
                this.selector.select();

                // Iterate over the set of keys for which events are available
                Iterator selectedKeys = this.selector.selectedKeys().iterator();
                while (selectedKeys.hasNext()) {
                    SelectionKey key = (SelectionKey) selectedKeys.next();
                    selectedKeys.remove();

                    if (!key.isValid()) {
                        continue;
                    }

                    // Check what event is available and deal with it
                    if (key.isAcceptable()) {
                        Log.d(PhUtilities.TCPSRV_TAG,
                                "\n\rPresent inside is Acceptable");
                        this.accept(key);
                    } else if (key.isReadable()) {
                        Log.d(PhUtilities.TCPSRV_TAG,
                                "\n\rPresent inside is Readable");
                        this.read(key);
                    } else if (key.isWritable()) {
                        Log.d(PhUtilities.TCPSRV_TAG,
                                "\n\rPresent inside is Writable");
                        this.write(key);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void accept(SelectionKey key) throws IOException {
        // For an accept to be pending the channel must be a server socket
        // channel.
        ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key
                .channel();

        // Accept the connection and make it non-blocking
        Log.d(PhUtilities.TCPSRV_TAG,
                "\n\rAccept the connection and make it non-blocking");
        SocketChannel socketChannel = serverSocketChannel.accept();

        Socket socket = socketChannel.socket();

        socket.setKeepAlive(true);
        socket.setTcpNoDelay(true);

        /*try {
            if (socketChannel.isConnected() && PhDTAUIMainActivity.tcpIPTextView != null) {
                PhDTAUIMainActivity.tcpIPTextView.setText("TCP/IP Status: Open");
                Log.d(PhUtilities.TCPSRV_TAG, "TCP/IP Status: Open");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }*/

        Log.d(PhUtilities.TCPSRV_TAG,
                "\n\rConnection from tcp client socket channel: "
                        + socketChannel);
        Log.d(PhUtilities.TCPSRV_TAG,
                "\n\r***Ip Address: " + socket.getInetAddress()
                        + " ***PortNumber: " + socket.getPort());
        socketChannel.configureBlocking(false);

        // Register the new SocketChannel with our Selector, indicating
        // we'd like to be notified when there's data waiting to be read
        Log.d(PhUtilities.TCPSRV_TAG,
                "\n\rRegister the new SocketChannel with our Selector, indicating  we'd like to be notified when there's data waiting to be read");
        socketChannel.register(this.selector, SelectionKey.OP_READ);

        // socketChannel.register(this.selector, SelectionKey.OP_READ |
        // SelectionKey.OP_WRITE);
        Log.d(PhUtilities.TCPSRV_TAG, "Socket Channel Registered");
    }

    private void read(SelectionKey key) throws IOException {
        SocketChannel socketChannel = (SocketChannel) key.channel();
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside Read");
        ByteBuffer buf = ByteBuffer.allocate(0);
        // Clear out our read buffer so it's ready for new data
        this.readBuffer.clear();
        this.readBuffer.put(new byte[1024]); //this is used to clear all the data from buffer before getting ready
        this.readBuffer.clear();//to read other data clear sets buffer position to 0 again
        // Attempt to read off the channel
        int numRead;
        String logDataLT = "";
        try {
            numRead = socketChannel.read(this.readBuffer);
            logDataLT = new String(readBuffer.array());
            Log.d(PhUtilities.TCPSRV_TAG, "Read Data");

            Log.d(PhUtilities.TCPSRV_TAG, "\n\rServer Read : " + numRead);
            Log.d(PhUtilities.TCPSRV_TAG, "\n\rData Received from Client : "
                    + logDataLT.trim());
        } catch (IOException e) {
            // The remote forcibly closed the connection, cancel
            // the selection key and close the channel.
            Log.d(PhUtilities.TCPSRV_TAG,
                    "\n\rException Occured Remote Forcibly Closed Connection");
            key.cancel();
            socketChannel.close();

            phDTAUIMainActivityObjRef.setTxtVwAppStatus("TCP/IP Status: Close");
            Log.d(PhUtilities.TCPSRV_TAG, "TCP/IP Status: Close");

            return;
        }
        PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
        // splitting message received from LT in order to understand ltType
        String[] msgSplittedList = logDataLT.split(",");
        int numberOfFileds = msgSplittedList.length;
        Log.d(PhUtilities.TCPSRV_TAG, "CRC Calucated for String " + nxpHelperMainActivity.getSubString(logDataLT).toString());
        String crcValue = nxpHelperMainActivity.getCRC32CheckSumForGivenString(nxpHelperMainActivity.getSubString(logDataLT).toString());
        String crcValueReceivedFromLT = msgSplittedList[numberOfFileds - 1];
        //crcValueReceivedFromLT = crcValueReceivedFromLT.substring(0, crcValueReceivedFromLT.length() - 5);
        crcValueReceivedFromLT = crcValueReceivedFromLT.replace("*", "").replace("\\n", "").replace("\\r", "");
        Log.d(PhUtilities.TCPSRV_TAG, "Just Before CRC Comparision");
        int nb = 0;
        if(!(crcValue.trim().equalsIgnoreCase(crcValueReceivedFromLT.trim()))){
            PhUtilities.isCRCCheckFailed= true;
            Log.d(PhUtilities.TCPSRV_TAG, "CRC Check Failed");
            Log.d(PhUtilities.TCPSRV_TAG, "Value calculated at DUT"+crcValue);
            Log.d(PhUtilities.TCPSRV_TAG, "Value received from LT"+crcValueReceivedFromLT.trim());
            PhRspHandler handler = new PhRspHandler();
            String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#NACK,");
            send(socketChannel, ("@#NACK,"+CRC+"*\n\r").getBytes());
            Log.d(PhUtilities.TCPSRV_TAG, "CRC Check Failed");
            return;
        }
        if ("PRO".equals(PhUtilities.ltType)) {
            Log.d(PhUtilities.PROTOCOL_TAG, "Follows Propreitary Protocol");
            if("$%K".equals(msgSplittedList[0])){
                Log.d(PhUtilities.TCPCNT_TAG, "Received KEEPALIVE Message Protocol from LT");
                nb = nxpHelperMainActivity.getNumberOfBytes("@#K,NB,IAMALIVE,");
                String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#K,"+nb+",IAMALIVE,");
                send(socketChannel,("@#K,"+nb+",IAMALIVE,"+CRC+"*\n\r").getBytes());
            }
            if("$%E".equals(msgSplittedList[0])){
                Log.d(PhUtilities.TCPSRV_TAG, "Pattern number received along with Execute message "+ msgSplittedList[3]);
               if(PhUtilities.NDK_IMPLEMENTATION==true && !msgSplittedList[3].equals(PhUtilities.patternNUmberBuff)){
                   Log.d(PhUtilities.TCPSRV_TAG, "Configuring Device ...... with received pattern number");
                   PhDtaLibStructure phDtaLibStructure = phDTAUIMainActivityObjRef.getPhDtaLibStructureObj();
                   phDtaLibStructure.setPatternNum(Integer.parseInt(msgSplittedList[3], 16));
                   PhNXPJniHelper phNXPJniHelper =  PhNXPJniHelper.getInstance();
                   if(PhUtilities.patternNUmberBuff.equals("")){
                       PhUtilities.patternNUmberBuff = msgSplittedList[3];
                       phNXPJniHelper.enableDiscovery(phDtaLibStructure);
                   }else if(PhUtilities.NDK_IMPLEMENTATION==true){
                    /**
                     * Other fields to be updated later
                     */
                    phNXPJniHelper.phDtaLibDisableDiscoveryJNI();
                    PhUtilities.patternNUmberBuff = msgSplittedList[3];
                    phNXPJniHelper.enableDiscovery(phDtaLibStructure);
                   }
              }
               Log.d(PhUtilities.TCPCNT_TAG, "Received EXECUTE Message Protocol from LT");
               nb = nxpHelperMainActivity.getNumberOfBytes("@#E,NB,"+msgSplittedList[2]+",,,");
               String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#E,"+nb+","+msgSplittedList[2]+",,,,");
               send(socketChannel,("@#E,"+nb+","+msgSplittedList[2]+",,,,"+CRC+"*\n\r").getBytes());
            }
            if("$%R".equals(msgSplittedList[0])){
                Log.d(PhUtilities.TCPCNT_TAG, "Received RESULT Message Protocol from LT");
                nb = nxpHelperMainActivity.getNumberOfBytes("@#R,NB,"+msgSplittedList[2]+",,,,");
                String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#R,"+nb+","+msgSplittedList[2]+",,,,");
                send(socketChannel,("@#R,"+nb+","+msgSplittedList[2]+",,,,"+CRC+"*\n\r").getBytes());
            }
            if("$%STOP".equals(msgSplittedList[0])){
                Log.d(PhUtilities.TCPCNT_TAG, "Received STOP Message Protocol from LT");
                nb = nxpHelperMainActivity.getNumberOfBytes("@#STOP,NB,");
                String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#STOP,"+nb+",");
                send(socketChannel,("@#STOP,"+nb+","+CRC+"*\n\r").getBytes());
                isHelloMessageReceived = false;
               key.cancel();
               socketChannel.close();
               try {
                   if (!socketChannel.isConnected()) {
                       phDTAUIMainActivityObjRef.setTxtVwAppStatus("TCP/IP Status: Close");
                       Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Close");
                   }
               } catch (NullPointerException nullPointerException) {
                   nullPointerException.printStackTrace();
               }
               return;
            }
           /* Log.e(PhUtilities.TCPCNT_TAG, PhUtilities.executeOrResultBuff+  " "+msgSplittedList[0].substring(2));
            if (msgSplittedList[0].substring(2).contains("E")) {
                PhUtilities.executeOrResultBuff = "Execute";
                if (msgSplittedList[3].contains("0")) {
                    PhUtilities.cardEmulationModeBuff = "Disabled";
                } else if (msgSplittedList[3].contains("1")) {
                    PhUtilities.cardEmulationModeBuff = "Enabled";
                }
             }else if (msgSplittedList[0].substring(2).contains("R")) {
             PhUtilities.executeOrResultBuff = "Result";

             if (msgSplittedList[3].contains("0")) {
                    PhUtilities.cardEmulationModeBuff = "Fail";
             } else if (msgSplittedList[3].contains("1")) {
                    PhUtilities.cardEmulationModeBuff = "Pass";
             } else if (msgSplittedList[3].contains("2")) {
                    PhUtilities.cardEmulationModeBuff = "Inconclusive";
             } else if (msgSplittedList[3].contains("3")) {
                    PhUtilities.cardEmulationModeBuff = "Error";
             }
         }*/

           /* Log.e(PhUtilities.TCPCNT_TAG, PhUtilities.executeOrResultBuff+  " "+msgSplittedList[0].substring(2));
            PhUtilities.testCaseIdBuff = msgSplittedList[1];
            PhUtilities.patternNUmberBuff = msgSplittedList[2];

            //Utilities.cardEmulationModeBuff = msgSplittedList[3];

            PhUtilities.rfu1Buff = msgSplittedList[4];
            PhUtilities.rfu2Buff = msgSplittedList[5];

            Log.e(PhUtilities.PROTOCOL_TAG, "Execute or Result: "
                    + msgSplittedList[0].substring(2));
            Log.e(PhUtilities.PROTOCOL_TAG, "Test Case Id : "
                    + msgSplittedList[1]);
            Log.e(PhUtilities.PROTOCOL_TAG, "Pattern Number : "
                    + msgSplittedList[2]);
            Log.e(PhUtilities.PROTOCOL_TAG, "Status/Card Emulation Mode : "
                    + msgSplittedList[3]);
            Log.e(PhUtilities.PROTOCOL_TAG, "RFU1 : " + msgSplittedList[4]);
            Log.e(PhUtilities.PROTOCOL_TAG, "RFU2 : " + msgSplittedList[5]);*/

        }/* else if ("C2P".equals(PhUtilities.ltType)) {
            Log.d(PhUtilities.PROTOCOL_TAG, "Follows Clear 2 Pay Protocol");

        } else if ("AGT".equals(PhUtilities.ltType)) {
            Log.d(PhUtilities.PROTOCOL_TAG, "Follows Agilent Protocol");

        } else if ("MPS".equals(PhUtilities.ltType)) {
            Log.d(PhUtilities.PROTOCOL_TAG, "Follows MicrPros Protocol");

        } else if ("CMP".equals(PhUtilities.ltType)) {
            Log.d(PhUtilities.PROTOCOL_TAG, "Follows Comprion Protocol");

        }    */

        if("$%HELLODUT".equals(msgSplittedList[0])){
            Log.d(PhUtilities.TCPCNT_TAG, "Received HELLODUT Message Protocol from LT");
            nb = nxpHelperMainActivity.getNumberOfBytes("@#HELLOLT,4,PRO,");
            String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#HELLOLT,"+nb+",PRO,");
            send(socketChannel,("@#HELLOLT,"+nb+",PRO,"+CRC+"*\n\r").getBytes());
            Log.d(PhUtilities.TCPSRV_TAG, "Message sent : " + "@#HELLOLT,"+nb+",PRO,"+CRC+"*\n\r");
            isHelloMessageReceived = true;
            PhUtilities.ltType = msgSplittedList[2];
        }

        // Logging LT messages to File
        if (PhUtilities.SIMULATED_CLIENT_SELECTED) {
            if(PhUtilities.LOG_LT_MESSAGES){
                String timeStamp = "\n\rServerRead: TIME STAMP: "
                        + new Date().toString() + "\tMessage : " + logDataLT.trim();
                StringBuilder strLogTextToFile = new StringBuilder(timeStamp);
                File file = new File("/sdcard/nxpdtalog/" + PhUtilities.FILE_NAME_LT);
                if (!file.exists()) {
                    Log.e(PhUtilities.TCPSRV_TAG,
                            "File didnt exist trying to create file and write ....");
                    try {
                        if (!PhUtilities.FILE_NAME_LT.equals("")) {
                            Log.e(PhUtilities.UI_TAG, "Creating LT file");
                            nxpHelperMainActivity
                                    .createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_LT);
                            // nxpHelper.createFileWithGivenNameOnsdCard("LogLT" +
                            // ".txt");
                        }
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }
                if (file.exists()) {
                    Log.d(PhUtilities.TCPSRV_TAG,
                            "File Exists, writing data to file .... ");
                    FileWriter fileWritter = new FileWriter(file, true);
                    BufferedWriter bufferWritter = new BufferedWriter(fileWritter);
                    bufferWritter.write(strLogTextToFile.toString());
                    bufferWritter.close();
                }
            }
        }
        String[] msgSplittedListInRead = logDataLT.split(",");
        // Sending Data to Client
        /**
         * Commented code as this is already handled earlier
         */
        /*if (PhUtilities.SERVER_SELECTED
                && "$%HELLODUT".equals(msgSplittedListInRead[0])) {
            Log.d(PhUtilities.TCPSRV_TAG,
                    "Data to Client (DUT ---> LT): @#HELLOLT,XXX,NB,CRC*\n\r");
            send(socketChannel,
                    "@#HELLOLT,XXX,NB,CRC*\n\r".getBytes(Charset.forName("UTF-8")));
        } else */if (PhUtilities.SIMULATED_SERVER_SELECTED
                && "$%HELLODUT".equals(msgSplittedListInRead[0])) {
            Log.d(PhUtilities.TCPSRV_TAG,
                    "Data to Client (LT ---> DUT): $%HELLODUT,PRO,NB,CRC*\n\r");
            send(socketChannel,
                    "$%HELLODUT,PRO,NB,CRC*\n\r".getBytes(Charset.forName("UTF-8")));

            Log.d(PhUtilities.TCPSRV_TAG,
                    "Sending Test Case Protocol (LT ---> DUT): $%E,TC_POL_NFCA_UND_BV_1,0,0,,,NB,CRC*\n\r");
            send(socketChannel,
                    "$%E,TC_POL_NFCA_UND_BV_1,0,0,,,NB,CRC*\n\r".getBytes(Charset
                            .forName("UTF-8")));
        }

        if (numRead == -1) {
            // Remote entity shut the socket down cleanly. Do the
            // same from our end and cancel the channel.
            Log.d(PhUtilities.TCPSRV_TAG,
                    "\n\rnumRead == -1 Server key channel close and cancel");
            // key.channel().close();
            // key.cancel();
            return;
        }

        // Hand the data off to our worker thread
        this.worker.processData(this, socketChannel, this.readBuffer.array(),
                numRead);
    }

    private void write(SelectionKey key) throws IOException {
        SocketChannel socketChannel = (SocketChannel) key.channel();
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside write method");
        synchronized (this.pendingData) {
            List queue = (List) this.pendingData.get(socketChannel);
            String logDataDUT = "";
            // Write until there's not more data ...
            while (!queue.isEmpty()) {
                ByteBuffer buf = (ByteBuffer) queue.get(0);

                // CharBuffer charBuff = new CharBuffer("Writing from Buffer");
                Log.d(PhUtilities.TCPSRV_TAG,
                        "\n\rFrom buff: " + new String(buf.array()));
                try{
                    socketChannel.write(buf);
                }catch(SocketException e){
                    Log.d(PhUtilities.TCPSRV_TAG, "Socket Exception While Writing Data to Client");
                    queue.clear();
                    break;
                }
                if (buf.remaining() > 0) {
                    // ... or the socket's buffer fills up
                    break;
                }
                logDataDUT = logDataDUT + new String(buf.array()).trim();
                queue.remove(0);
            }

            /*
             * //splitting message received from LT in order to understand
             * ltType String[] msgSplittedList = logDataDUT.split(",");
             * if("$%HELLODUT".equals(msgSplittedList[0])){ Utilities.ltType =
             * msgSplittedList[1]; }else{ if("PRO".equals(Utilities.ltType)){
             * Log.d(Utilities.PROTOCOL_TAG, "Follows Propreitary Protocol");
             * Utilities.executeOrResultBuff = msgSplittedList[0].substring(2);
             * Utilities.testCaseIdBuff = msgSplittedList[1];
             * Utilities.patternNUmberBuff = msgSplittedList[2];
             * Utilities.cardEmulationModeBuff = msgSplittedList[3];
             * Utilities.rfu1Buff = msgSplittedList[4]; Utilities.rfu2Buff =
             * msgSplittedList[5]; Log.e(Utilities.PROTOCOL_TAG,
             * "Execute or Result: " + msgSplittedList[0].substring(2));
             * Log.e(Utilities.PROTOCOL_TAG, "Test Case Id : " +
             * msgSplittedList[1]); Log.e(Utilities.PROTOCOL_TAG,
             * "Pattern Number : " + msgSplittedList[2]);
             * Log.e(Utilities.PROTOCOL_TAG, "Card Emulation Mode : " +
             * msgSplittedList[3]); Log.e(Utilities.PROTOCOL_TAG, "RFU1 : " +
             * msgSplittedList[4]); Log.e(Utilities.PROTOCOL_TAG, "RFU2 : " +
             * msgSplittedList[5]);
             *
             * }else if("C2P".equals(Utilities.ltType)){
             * Log.d(Utilities.PROTOCOL_TAG, "Follows Clear 2 Pay Protocol");
             * }else if("AGT".equals(Utilities.ltType)){
             * Log.d(Utilities.PROTOCOL_TAG, "Follows Agilent Protocol"); }else
             * if("MPS".equals(Utilities.ltType)){ Log.d(Utilities.PROTOCOL_TAG,
             * "Follows MicrPros Protocol"); }else
             * if("CMP".equals(Utilities.ltType)){ Log.d(Utilities.PROTOCOL_TAG,
             * "Follows Comprion Protocol"); } }
             */

            if (PhUtilities.LOG_DUT_MESSAGES && PhUtilities.SERVER_SELECTED) {
                Log.e(PhUtilities.TCPSRV_TAG,
                        "Utilities.LOG_DUT_MESSAGES && Utilities.SERVER_SELECTED truue");
                String timeStamp = "\n\rServerWrite: TIME STAMP: "
                        + new Date().toString() + "\tMessage : " + logDataDUT;
                StringBuilder strLogTextToFile = new StringBuilder(timeStamp);
                PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
                File file = new File("/sdcard/nxpdtalog/"
                        + PhUtilities.FILE_NAME_DUT);
                if (!file.exists()) {
                    Log.e(PhUtilities.TCPSRV_TAG,
                            "File didnt exist trying to create file and write ....");
                    try {
                        if (!PhUtilities.FILE_NAME_DUT.equals("")) {
                            Log.e(PhUtilities.UI_TAG, "Creating DUT file");
                            nxpHelperMainActivity
                                    .createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_DUT);
                            // nxpHelper.createFileWithGivenNameOnsdCard("LogDUT"
                            // + ".txt");
                        }
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }
                if (file.exists()) {
                    FileWriter fileWritter = new FileWriter(file, true);
                    BufferedWriter bufferWritter = new BufferedWriter(
                            fileWritter);
                    bufferWritter.write(strLogTextToFile.toString());
                    bufferWritter.close();
                }
            }

            String[] msgSplittedList = logDataDUT.split(",");
            if ("@#STOP".equals(msgSplittedList[0])) {
                key.cancel();
                socketChannel.close();

                try {
                    /*if (PhDTAUIMainActivity.tcpIPTextView != null) {
                        PhDTAUIMainActivity.tcpIPTextView
                                .setText("TCP/IP Status: Close");
                        Log.d(PhUtilities.TCPSRV_TAG, "TCP/IP Status: Close");
                    }*/
                } catch (NullPointerException nullPointerException) {
                    nullPointerException.printStackTrace();
                }

                return;
            }

            if (queue.isEmpty()) {
                // We wrote away all data, so we're no longer interested
                // in writing on this socket. Switch back to waiting for
                // data.
                key.interestOps(SelectionKey.OP_READ);
            }
        }
    }

    private Selector initSelector() throws IOException {
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rInside initSelctor");
        // Create a new selector
        Selector socketSelector = SelectorProvider.provider().openSelector();

        // Create a new non-blocking server socket channel
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rOpening server channel");
        this.serverChannel = ServerSocketChannel.open();
        serverChannel.configureBlocking(false);

        // Bind the server socket to the specified address and port
        Log.d(PhUtilities.TCPSRV_TAG,
                "\n\rBind the server socket to the specified address and port");
        InetSocketAddress isa = new InetSocketAddress(this.hostAddress,
                this.portNum);
        serverChannel.socket().bind(isa);

        // Register the server socket channel, indicating an interest in
        // accepting new connections
        Log.d(PhUtilities.TCPSRV_TAG, "\n\rReady to accept new connections on port number "+this.portNum+" with IPAddress "+InetAddress.getLocalHost());
        serverChannel.register(socketSelector, SelectionKey.OP_ACCEPT);

        /*if (PhUtilities.CLIENT_SELECTED) {
            Log.d(PhUtilities.TCPCNT_TAG,
                    "Starting TCP/IP Client. On RUN button click. Server is ready to accept connections.");
            new PhNioClientTask(InetAddress.getByName("127.0.0.1"), port)
                    .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
            // Utilities.CLIENT_SELECTED = false;
        }*/

        if (PhUtilities.SIMULATED_CLIENT_SELECTED) {
            Log.d(PhUtilities.TCPCNT_TAG,
                    "Starting TCP/IP Client Simulator. On RUN button click. Server is ready to accept connections.");

            new PhNioClientTask(phDTAUIMainActivityObjRef,InetAddress.getByName("127.0.0.1"), portNum)
                    .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
            // Utilities.SIMULATED_CLIENT_SELECTED = false;
        }

        return socketSelector;
    }

    public void mainTCPServer(int portNumber) {
//          PhEchoWorker worker = new PhEchoWorker();
            this.worker  = new PhEchoWorker();
            this.portNum = portNumber;
            Log.d(PhUtilities.TCPSRV_TAG, "\n\rStarting Worker Thread");
            new Thread(worker).start();
            Log.d(PhUtilities.TCPSRV_TAG, "\n\rnew NioServer(null, "+portNumber +" , worker)");
//            new Thread(new PhNioServer(phDTAUIMainActivityObjRef,null, portNumber, worker)).start();
//            PhNioServer server = new PhNioServer(phDTAUIMainActivityObjRef);
//            new Thread(new NioServer(null, 8365, worker)).start();
            if(PhUtilities.SIMULATED_SERVER_SELECTED){
                PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
                int nb = nxpHelperMainActivity.getNumberOfBytes("$%HELLODUT,NB,PRO,");
                String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("$%HELLODUT,"+nb+",PRO,");
        }
    }
}
