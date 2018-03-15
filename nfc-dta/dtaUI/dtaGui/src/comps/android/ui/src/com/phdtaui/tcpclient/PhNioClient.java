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

package com.phdtaui.tcpclient;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ConnectException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import android.os.AsyncTask;
import android.util.Log;

import com.phdtaui.asynctask.PhNioServerTask;
import com.phdtaui.helper.PhNXPHelperMainActivity;
import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.structure.PhDtaLibStructure;
import com.phdtaui.helper.PhNXPJniHelper;
import com.phdtaui.utils.PhUtilities;

public class PhNioClient extends AsyncTask<Void, Void, Void> implements Runnable {
    // The host:port combination to connect to
    private InetAddress hostAddress;
    private int port;
    private PhDTAUIMainActivity phMainActivityObjRef;

    // The selector we'll be monitoring
    private Selector selector;

    // The buffer into which we'll read data when it's available
    private ByteBuffer readBuffer = ByteBuffer.allocate(8192);

    // A list of PendingChange instances
    private List pendingChanges = new LinkedList();

    // Maps a SocketChannel to a list of ByteBuffer instances
    private Map pendingData = new HashMap();
     // Maps a SocketChannel to a RspHandler
    private Map rspHandlers = Collections.synchronizedMap(new HashMap());
     public PhNioClient(PhDTAUIMainActivity phMainActivityObj,InetAddress hostAddress, int port) throws IOException {
        this.hostAddress = hostAddress;
        this.port = port;
        this.selector = this.initSelector();
        this.phMainActivityObjRef = phMainActivityObj;
    }

    /**
     * to check whether Hello Message received from LT
     */
    private boolean isHelloMessageReceived = false;

    /**
     *
     * @param data
     * @param handler
     * @throws IOException
     */
    public void send(byte[] data, PhRspHandler handler) throws IOException {
        // Start a new connection
        SocketChannel socket = this.initiateConnection();
             // Register the response handler
        this.rspHandlers.put(socket, handler);
             // And queue the data we want written
        synchronized (this.pendingData) {
            List queue = (List) this.pendingData.get(socket);
            if (queue == null) {
                queue = new ArrayList();
                this.pendingData.put(socket, queue);
            }
            Log.d(PhUtilities.TCPCNT_TAG, new String(ByteBuffer.wrap(data).array()));
            queue.add(ByteBuffer.wrap(data));
        }

        // Finally, wake up our selecting thread so it can make the required changes
        this.selector.wakeup();
    }

    public void run() {
        while (true) {
            try {
                // Process any pending changes
                synchronized (this.pendingChanges) {
                    Iterator changes = this.pendingChanges.iterator();
                    while (changes.hasNext()) {
                        PhChangeRequest change = (PhChangeRequest) changes.next();
                        switch (change.type) {
                        case PhChangeRequest.CHANGEOPS:
                            SelectionKey key = change.socket.keyFor(this.selector);
                            key.interestOps(change.ops);
                            break;
                        case PhChangeRequest.REGISTER:
                            change.socket.register(this.selector, change.ops);
                            break;
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
                    if (key.isConnectable()) {
                        boolean hasExceptionOccured = false;
                        try{
                            this.finishConnection(key);
                        }catch (ConnectException e) {
                            hasExceptionOccured = true;
                            // TODO Auto-generated catch block
                            PhUtilities.isServerRunning = false;
                            try{
                                /**
                                 * Forcing thread to sleep to give some time before trying to reconnect to server
                                 */
                                Thread.sleep(1500);
                            } catch(Exception ex){

                            }
                            /**
                             * Checking number of retries. It tries for given number of times from input.
                             */
                            if(PhUtilities.countOfRetry <= PhUtilities.maxNumberOfRetries){
                                /**
                                 * Sends hello message which in turn tries to connect to server
                                 */
                                send("@#HELLOLT,NB,PRO,CRC*\r\n".getBytes(), new PhRspHandler());
                                PhUtilities.countOfRetry++;
                            }else{
                                PhUtilities.isServerRunning = false;
                            }
                        } catch (IOException e) {
                            // Cancel the channel's registration with our selector
                            hasExceptionOccured = true;
                            PhUtilities.isServerRunning = false;
                            System.out.println(e);
                            key.cancel();
                            return;
                        }
                        if(!hasExceptionOccured){
                            /**
                             * Making number of retries to zero once client is connected to server
                             */
                            PhUtilities.countOfRetry = 0;
                        }
                        PhUtilities.isServerRunning = false;
                    } else if (key.isReadable()) {
                        this.read(key);
                    } else if (key.isWritable()) {
                        this.write(key);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void read(SelectionKey key) throws IOException {
        SocketChannel socketChannel = (SocketChannel) key.channel();
        PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
        // Clear out our read buffer so it's ready for new data
        this.readBuffer.clear();

        // Attempt to read off the channel
        int numRead = 0;
        String logDataLT = "";
        try {
                numRead = socketChannel.read(this.readBuffer);
                logDataLT = new String(readBuffer.array());
                Log.d(PhUtilities.TCPCNT_TAG, "\n\rClient Read : " + numRead);
                Log.d(PhUtilities.TCPCNT_TAG, "\n\rData Received from Server : " + logDataLT.trim());
            } catch(ConnectException ce){
                try {
                    if (!socketChannel.isConnected()) {
                        PhUtilities.TCPIPConnectionStateText = "Close";
                        Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Close");
                        PhUtilities.isServerRunning = false;
                    }
                } catch (NullPointerException nullPointerException) {
                    nullPointerException.printStackTrace();
                }
            } catch (IOException e) {
                // The remote forcibly closed the connection, cancel
                // the selection key and close the channel.
                key.cancel();
                socketChannel.close();
                try {
                    if (!socketChannel.isConnected()) {
                        PhUtilities.TCPIPConnectionStateText = "Close";
                                         Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Close");
                    }
                } catch (NullPointerException nullPointerException) {
                    nullPointerException.printStackTrace();
                }
            return;
        }
             /*//splitting message received from LT in order to understand ltType
        String[] msgSplittedList = logDataLT.split(",");
        if("$%HELLODUT".equals(msgSplittedList[0])){
            Utilities.ltType = msgSplittedList[1];
        }else{
            if("PRO".equals(Utilities.ltType)){
                Log.d(Utilities.PROTOCOL_TAG, "Follows Propreitary Protocol");
                Utilities.executeOrResultBuff = msgSplittedList[0].substring(2);
                Utilities.testCaseIdBuff = msgSplittedList[1];
                Utilities.patternNUmberBuff = msgSplittedList[2];
                Utilities.cardEmulationModeBuff = msgSplittedList[3];
                Utilities.rfu1Buff = msgSplittedList[4];
                Utilities.rfu2Buff = msgSplittedList[5];
                Log.e(Utilities.PROTOCOL_TAG, "Execute or Result: " + msgSplittedList[0].substring(2));
                Log.e(Utilities.PROTOCOL_TAG, "Test Case Id : " + msgSplittedList[1]);
                Log.e(Utilities.PROTOCOL_TAG, "Pattern Number : " + msgSplittedList[2]);
                Log.e(Utilities.PROTOCOL_TAG, "Card Emulation Mode : " + msgSplittedList[3]);
                Log.e(Utilities.PROTOCOL_TAG, "RFU1 : " + msgSplittedList[4]);
                Log.e(Utilities.PROTOCOL_TAG, "RFU2 : " + msgSplittedList[5]);
                         }else if("C2P".equals(Utilities.ltType)){
                Log.d(Utilities.PROTOCOL_TAG, "Follows Clear 2 Pay Protocol");
            }else if("AGT".equals(Utilities.ltType)){
                Log.d(Utilities.PROTOCOL_TAG, "Follows Agilent Protocol");
            }else if("MPS".equals(Utilities.ltType)){
                Log.d(Utilities.PROTOCOL_TAG, "Follows MicrPros Protocol");
            }else if("CMP".equals(Utilities.ltType)){
                Log.d(Utilities.PROTOCOL_TAG, "Follows Comprion Protocol");
            }
        }*/
            if(PhUtilities.SIMULATED_SERVER_SELECTED){
                PhNioServerTask phNioServerTask = new PhNioServerTask(this.phMainActivityObjRef,1111);
                if(PhUtilities.LOG_LT_MESSAGES){
                    Log.e(PhUtilities.TCPCNT_TAG, "Utilities.LOG_LT_MESSAGES && Utilities.SIMULATED_CLIENT_SELECTED coming as true");
                    String timeStamp = "\n\rClientRead: TIME STAMP: "
                            + new Date().toString()
                            + "\tMessage : " + logDataLT.trim();
                    StringBuilder strLogTextToFile = new StringBuilder(timeStamp);
                        File file = new File("/sdcard/nxpdtalog/" + PhUtilities.FILE_NAME_LT);
                        if (!file.exists()) {
                            Log.e(PhUtilities.TCPSRV_TAG, "File didnt exist trying to create file and write ....");
                            try {
                                if(!PhUtilities.FILE_NAME_LT.equals("")){
                                    Log.e(PhUtilities.UI_TAG, "Crating LT file");
                                    nxpHelperMainActivity.createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_LT);
                                    //nxpHelper.createFileWithGivenNameOnsdCard("LogLT" + ".txt");
                                }
                            } catch (IOException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
                        }
                        if (file.exists()) {
                            FileWriter fileWritter = new FileWriter(file, true);
                            BufferedWriter bufferWritter = new BufferedWriter(fileWritter);
                            bufferWritter.write(strLogTextToFile.toString());
                            bufferWritter.close();
                        }
                }
            }
        String[] msgSplittedList = logDataLT.split(",");
        int numberOfFileds = msgSplittedList.length;
        String crcValueReceivedFromLT = msgSplittedList[numberOfFileds - 1];
        String crcValue = nxpHelperMainActivity.getCRC32CheckSumForGivenString(nxpHelperMainActivity.getSubString(logDataLT).toString());
        crcValueReceivedFromLT = crcValueReceivedFromLT.replace("*", "").replace("\\n", "").replace("\\r", "");
        int nb = 0;
        Log.d(PhUtilities.TCPCNT_TAG, "Before CRC Validation");
        if(!(crcValue.trim().equalsIgnoreCase(crcValueReceivedFromLT.trim()))){
            PhUtilities.isCRCCheckFailed= true;
            Log.d(PhUtilities.TCPCNT_TAG, "CRC Check Failed");
            Log.d(PhUtilities.TCPCNT_TAG, "Value calculated at DUT"+crcValue);
            Log.d(PhUtilities.TCPCNT_TAG, "Value received from LT"+crcValueReceivedFromLT.trim());
            PhRspHandler handler = new PhRspHandler();
            String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#NACK,");
            send(("@#NACK,"+CRC+"*\r\n").getBytes(), handler);
            Log.d(PhUtilities.TCPCNT_TAG, "CRC Check Failed");
            return;
        }
            if ("PRO".equals(PhUtilities.ltType)) {
                Log.d(PhUtilities.PROTOCOL_TAG, "Follows Propreitary Protocol");
                if("$%K".equals(msgSplittedList[0])){
                    Log.d(PhUtilities.TCPCNT_TAG, "Received KEEPALIVE Message Protocol from LT");
                    PhRspHandler handler = new PhRspHandler();
                    nb = nxpHelperMainActivity.getNumberOfBytes("@#K,NB,IAMALIVE,");
                    String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#K,"+nb+",IAMALIVE,");
                    send(("@#K,"+nb+",IAMALIVE,"+CRC+"*\r\n").getBytes(), handler);
                }else if("$%E".equals(msgSplittedList[0])){
                    Log.d(PhUtilities.TCPCNT_TAG, "Received EXECUTE Message Protocol from LT");
                    PhRspHandler handler = new PhRspHandler();
                    nb = nxpHelperMainActivity.getNumberOfBytes("@#E,NB,"+msgSplittedList[2]+",,,,");
                    String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#E,"+nb+","+msgSplittedList[2]+",,,,");
                    if(!msgSplittedList[3].equals(PhUtilities.patternNUmberBuff)){
                        PhDtaLibStructure phDtaLibStructure = phMainActivityObjRef.getPhDtaLibStructureObj();
                        phDtaLibStructure.setPatternNum(Integer.parseInt(msgSplittedList[3], 16));
                        PhNXPJniHelper phNXPJniHelper =  PhNXPJniHelper.getInstance();
                        if(PhUtilities.NDK_IMPLEMENTATION==true && PhUtilities.patternNUmberBuff == null){
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
                        send(("@#E,"+nb+","+msgSplittedList[2]+",,,,"+CRC+"*\r\n").getBytes(), handler);
                    }
                }else if("$%R".equals(msgSplittedList[0])){
                    Log.d(PhUtilities.TCPCNT_TAG, "Received RESULT Message Protocol from LT");
                    PhRspHandler handler = new PhRspHandler();
                    nb = nxpHelperMainActivity.getNumberOfBytes("@#R,NB,"+msgSplittedList[2]+",,,,");
                    String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#R,"+nb+","+msgSplittedList[2]+",,,,");
                    send(("@#R,"+nb+","+msgSplittedList[2]+",,,,"+CRC+"*\r\n").getBytes(), handler);
                }else if("$%STOP".equals(msgSplittedList[0])){
                    Log.d(PhUtilities.TCPCNT_TAG, "Received STOP Message Protocol from LT");
                    PhRspHandler handler = new PhRspHandler();
                    nb = nxpHelperMainActivity.getNumberOfBytes("@#STOP,NB,");
                    String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#STOP,"+nb+",");
                    send(("@#STOP,"+nb+","+CRC+"*\r\n").getBytes(), handler);
                   key.cancel();
                   socketChannel.close();
                   isHelloMessageReceived = false;
                   try {
                       /*if (PhDTAUIMainActivity.tcpIPTextView != null) {
                           PhDTAUIMainActivity.tcpIPTextView.setText("TCP/IP Status: Close");
                           Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Close");
                       }*/
                   } catch (NullPointerException nullPointerException) {
                       nullPointerException.printStackTrace();
                   }
                   return;
               }
            }/*else if (msgSplittedList[0].substring(2).contains("R")) {
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
                Log.e(PhUtilities.PROTOCOL_TAG, "RFU2 : " + msgSplittedList[5]);
                }*/

            if("$%HELLODUT".equals(msgSplittedList[0])){
                    Log.d(PhUtilities.TCPCNT_TAG, "Received HELLODUT Message Protocol from LT");
                    isHelloMessageReceived = true;
                    PhRspHandler handler = new PhRspHandler();
                    PhUtilities.ltType = msgSplittedList[2];
                    nb = nxpHelperMainActivity.getNumberOfBytes("@#HELLOLT,NB,PRO,");
                    String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#HELLOLT,"+nb+",PRO,");
                    Log.d(PhUtilities.TCPCNT_TAG, "Message sent " + "@#HELLOLT,"+nb+",PRO,"+CRC+"*\n\r");
                    send(("@#HELLOLT,"+nb+",PRO,"+CRC+"*\n\r").getBytes(), handler);
            }
            if(PhUtilities.SIMULATED_CLIENT_SELECTED){
                 PhNXPHelperMainActivity phNxpHelperActivity = new PhNXPHelperMainActivity();
                if("@#HELLOLT".equals(msgSplittedList[0])){
                    System.err.println(PhUtilities.TCPCNT_TAG+" Received HELLODUT Message Protocol from LT");
                    isHelloMessageReceived = true;
                    PhRspHandler handler = new PhRspHandler();
                    PhUtilities.ltType = msgSplittedList[2];
                    //send("@#HELLOLT,NB,PRO,CRC*\r\n".getBytes(), handler);
                    System.out.println("Hand Shake Completed");

                    String CRC = phNxpHelperActivity.getCRC32CheckSumForGivenString("$%K,NB,HEARTBEAT,");
                     send(("$%K,NB,HEARTBEAT,"+CRC+"*\r\n").getBytes(), handler);

                 } else if("@#K".equals(msgSplittedList[0])){
                     System.err.println(PhUtilities.TCPCNT_TAG+" Received KEEPALIVE Message Protocol from LT");
                     PhRspHandler handler = new PhRspHandler();
                     String CRC = phNxpHelperActivity.getCRC32CheckSumForGivenString("$%E,NB,"+"TID"+","+PhUtilities.patternNUmberBuff+",CE,RFU1,RFU2,");
                     send(("$%E,NB,"+"TID"+","+PhUtilities.patternNUmberBuff+",CE,RFU1,RFU2,"+CRC+"*\r\n").getBytes(), handler);
                 }
            }
            if (numRead == -1) {
            // Remote entity shut the socket down cleanly. Do the
            // same from our end and cancel the channel.
            Log.d(PhUtilities.TCPCNT_TAG, "numRead == -1 so key channel close and cancel");
            //key.channel().close();
            //key.cancel();
            return;
        }

        // Handle the response
        this.handleResponse(socketChannel, this.readBuffer.array(), numRead);
    }

    private void handleResponse(SocketChannel socketChannel, byte[] data, int numRead) throws IOException {
        // Make a correctly sized copy of the data before handing it
        // to the client
        byte[] rspData = new byte[numRead];
        System.arraycopy(data, 0, rspData, 0, numRead);
             // Look up the handler for this channel
        PhRspHandler handler = (PhRspHandler) this.rspHandlers.get(socketChannel);
             // And pass the response to it
        /*if (handler.handleResponse(rspData)) {
            // The handler has seen enough, close the connection
            socketChannel.close();
            socketChannel.keyFor(this.selector).cancel();
        }*/
    }

    private void write(SelectionKey key) throws IOException {
        SocketChannel socketChannel = (SocketChannel) key.channel();

        synchronized (this.pendingData) {
            List queue = (List) this.pendingData.get(socketChannel);
            String logDataDUT = "";
            // Write until there's not more data ...
            while (!queue.isEmpty()) {
                ByteBuffer buf = (ByteBuffer) queue.get(0);
                             //Log.e(Utilities.TCPCNT_TAG, "In While when queue is not empty");
                             socketChannel.write(buf);
                if (buf.remaining() > 0) {
                    // ... or the socket's buffer fills up
                    break;
                }
                logDataDUT = logDataDUT + new String(buf.array()).trim();
                queue.remove(0);
            }
                     if(PhUtilities.LOG_DUT_MESSAGES && PhUtilities.CLIENT_SELECTED){
                String timeStamp = "\n\rClientWrite: TIME STAMP: "
                        + new Date().toString()
                        + "\tMessage : " + logDataDUT;
                StringBuilder strLogTextToFile = new StringBuilder(timeStamp);
                PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
                    File file = new File("/sdcard/nxpdtalog/" + PhUtilities.FILE_NAME_DUT);
                    if(!file.exists()){
                        Log.e(PhUtilities.TCPSRV_TAG, "File didnt exist trying to create file and write ....");
                        try {
                            if(!PhUtilities.FILE_NAME_DUT.equals("")){
                                Log.e(PhUtilities.UI_TAG, "Creating DUT file");
                                nxpHelperMainActivity.createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_DUT);
                                //nxpHelper.createFileWithGivenNameOnsdCard("LogDUT" + ".txt");
                            }
                        } catch (IOException e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    }
                    if (file.exists()) {
                        FileWriter fileWritter = new FileWriter(file, true);
                        BufferedWriter bufferWritter = new BufferedWriter(fileWritter);
                        bufferWritter.write(strLogTextToFile.toString());
                        bufferWritter.close();
                    }
            }
                     if (queue.isEmpty()) {
                // We wrote away all data, so we're no longer interested
                // in writing on this socket. Switch back to waiting for
                // data.
                key.interestOps(SelectionKey.OP_READ);
            }
        }
    }

    private void finishConnection(SelectionKey key) throws IOException, ConnectException {
            SocketChannel socketChannel = (SocketChannel) key.channel();
             // Finish the connection. If the connection operation failed
            // this will raise an IOException
            boolean isConnectionSuccessful = socketChannel.finishConnect();
            if (socketChannel.isConnected()) {
                PhUtilities.TCPIPConnectionStateText = "Open";
                PhUtilities.isServerRunning = true;
                PhUtilities.isClientConnected = true;
            }
             Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Open");

         // Register an interest in writing on this channel
        key.interestOps(SelectionKey.OP_WRITE);
    }

    private SocketChannel initiateConnection() throws IOException {
        // Create a non-blocking socket channel
        SocketChannel socketChannel = SocketChannel.open();
        socketChannel.configureBlocking(false);
         // Kick off connection establishment
        socketChannel.connect(new InetSocketAddress(this.hostAddress, this.port));
         // Queue a channel registration since the caller is not the
        // selecting thread. As part of the registration we'll register
        // an interest in connection events. These are raised when a channel
        // is ready to complete connection establishment.
        synchronized(this.pendingChanges) {
            this.pendingChanges.add(new PhChangeRequest(socketChannel, PhChangeRequest.REGISTER, SelectionKey.OP_CONNECT));
        }
        Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status");
        if (socketChannel.isConnected()) {
            PhUtilities.TCPIPConnectionStateText = "Open";
            Log.d(PhUtilities.TCPCNT_TAG, "TCP/IP Status: Open");
            PhUtilities.isServerRunning = false;

        }
             return socketChannel;
    }

    private Selector initSelector() throws IOException {
        // Create a new selector
        return SelectorProvider.provider().openSelector();
    }

    public void mainTCPClient() {
        try {
            //NioClient client = new NioClient(InetAddress.getByAddress("192.168.1.105".getBytes()), 8887);
            Log.d(PhUtilities.TCPCNT_TAG, "In Nio Client TCP");
//            PhNioClient client = new PhNioClient(PhDTAUIMainActivity.inetAddress, PhDTAUIMainActivity.portNumber);
            //PhNioClient client = new PhNioClient(InetAddress.getByName("192.168.1.104"), 4020);
            //PhNioClient client = new PhNioClient(InetAddress.getLocalHost(), 8887);
            //NioClient client = new NioClient(InetAddress.getLocalHost(), 8365);
            Thread t = new Thread(this);
            Log.d(PhUtilities.TCPCNT_TAG, "Created thread");
            t.setDaemon(true);
            t.start();
            PhRspHandler handler = new PhRspHandler();
            //Log.d(PhUtilities.TCPCNT_TAG, "Connected to Server. Sending Data.");
            if(PhUtilities.CLIENT_SELECTED){
                Log.d(PhUtilities.TCPCNT_TAG, "DUT ---> LT");
                send("@#HELLOLT,NB,PRO,CRC*\r\n".getBytes(), handler);
                /*PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
                long CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("@#HELLOLT,NB,PRO,");
                client.send((("$%HELLODUT,NB,PRO,"+Long.toHexString(CRC).toUpperCase()+"*\n\r").trim()).getBytes(), handler);*/
            }else if(PhUtilities.SIMULATED_CLIENT_SELECTED){
                Log.d(PhUtilities.TCPCNT_TAG, "LT ---> DUT");
                PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
                int nb = nxpHelperMainActivity.getNumberOfBytes("$%HELLODUT,NB,PRO,");
                String CRC = nxpHelperMainActivity.getCRC32CheckSumForGivenString("$%HELLODUT,"+nb+",PRO,");
                send(("$%HELLODUT,"+nb+",PRO,"+CRC+"*\n\r").getBytes(), handler);
              //  client.send("$%E,NB,TC_POL_NFCA_UND_BV_1,0,0,,,CRC*\n\r".getBytes(), handler);
              //  client.send("$%STOP,NB,CRC*\n\r".getBytes(), handler);
                //client.send("$%E,TC_POL_NFCA_UND_BV_2,0,0,,,NB,CRC*\n\r".getBytes(), handler);
            }
            handler.waitForResponse();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected Void doInBackground(Void... params) {
        return null;
    }

}
