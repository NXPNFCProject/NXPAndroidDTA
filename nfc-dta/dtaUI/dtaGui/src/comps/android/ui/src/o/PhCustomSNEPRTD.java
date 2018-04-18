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

package com.phdtaui.customdialogs;

import java.nio.charset.Charset;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.NfcAdapter.CreateNdefMessageCallback;
import android.nfc.NfcAdapter.OnNdefPushCompleteCallback;
import android.nfc.NfcEvent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;
import com.phdtaui.helper.PhNXPJniHelper;
import com.phdtaui.utils.PhUtilities;
import com.phdtaui.messenger.PhMessengerService;
import android.nfc.dta.NfcDta;

public class PhCustomSNEPRTD extends Dialog implements
        android.view.View.OnClickListener, CreateNdefMessageCallback,
        OnNdefPushCompleteCallback, OnItemSelectedListener {

    /*Variables for accessing GUI view elements*/
    private Button runServerBtn, stopServerBtn, runClientBtn, stopClientBtn,
                   backBtn,clientClearMsgBtn,serverClearMsgBtn;
    private Spinner spinnerClientTestCaseIds, spinnerServerTestCaseIds;
    private ProgressDialog loadingProgress;
    private CheckBox chkBoxShortRecordLayout;
    private TextView txtVwClientMsg,txtVwServerMsg;

    /* constants*/
    private final String TAG = "SnepExtendedDTAServer";
    private final int MESSAGE = 1 , MESSAGE_RECEIVE = 2 , MESSAGE_LLCP_ACTIVATED = 7 ,
                                MESSAGE_LLCP_DEACTIVATED = 8;

    /*Other variables of object context*/
    private Context mContext;
    private NfcAdapter mNfcAdapter;
    private NfcDta mNfcDta;
    private StringBuffer strBuffer;
    private boolean isNfcInitInDtaMode,runClientBtnClicked, runServerBtnClicked;
    private int testCaseID;
    private boolean isServerRunning;
    private boolean isClientRunning;
    SnepBroadcastReciever snepDataReciever;

    /**
     * AutomaTest Related SNEP Code for working with Intents on different
     * NFC Forum Test Tools
     */
    public boolean AUTOMATEST_FLAG;

    public PhCustomSNEPRTD(Context context) {
        super(context);
        this.mContext = context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /**
         * No Title for the PopUp
         */
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        /**
         * Setting the layout for Custom Exit Dialog
         */
        setContentView(R.layout.ph_custom_snep_rtd);
        isNfcInitInDtaMode = true;
        if (isNfcInitInDtaMode) {
            isNfcInitInDtaMode=false;
            new DeInitDTA().execute();
        }
        setCanceledOnTouchOutside(false);
        runServerBtn = (Button) findViewById(R.id.run_server);
        runServerBtn.setOnClickListener(this);

        stopServerBtn = (Button) findViewById(R.id.stop_server);
        stopServerBtn.setOnClickListener(this);

        runClientBtn = (Button) findViewById(R.id.run_client);
        runClientBtn.setOnClickListener(this);
        runClientBtn.setEnabled(false);

        stopClientBtn = (Button) findViewById(R.id.stop_client);
        stopClientBtn.setOnClickListener(this);

        backBtn = (Button) findViewById(R.id.back_btn);
        backBtn.setOnClickListener(this);

        spinnerClientTestCaseIds = (Spinner) findViewById(R.id.client_test_case_id);
        spinnerServerTestCaseIds = (Spinner) findViewById(R.id.server_test_case_id);
        spinnerClientTestCaseIds.setOnItemSelectedListener(this);
        spinnerServerTestCaseIds.setOnItemSelectedListener(this);
        txtVwClientMsg=(TextView)findViewById(R.id.client_ndef_message);
        txtVwServerMsg=(TextView)findViewById(R.id.server_ndef_message);

        chkBoxShortRecordLayout=(CheckBox)findViewById(R.id.short_record_layout);
        chkBoxShortRecordLayout.setChecked(false);

        clientClearMsgBtn=(Button)findViewById(R.id.client_clear_msg_button);
        clientClearMsgBtn.setOnClickListener(this);

        serverClearMsgBtn=(Button)findViewById(R.id.server_clear_msg_button);
        serverClearMsgBtn.setOnClickListener(this);

        strBuffer = new StringBuffer();
        mNfcAdapter   = NfcAdapter.getDefaultAdapter(this.mContext);
        mNfcDta = NfcDta.getInstance(mNfcAdapter);
    }

    @Override
    public void onClick(View v) {
        boolean nfcEnable=mNfcAdapter.isEnabled();
        switch (v.getId()) {
        case R.id.run_server:
            handleSnepRunServer();
            break;

        case R.id.stop_server:
            handleSnepStopServer();
            break;

        case R.id.run_client:
            handleSnepRunClient();
            break;

        case R.id.stop_client:
            handleSnepStopClient();
            break;

        case R.id.back_btn:
            snepDtaCmd("disableDta", null, 0, 0, 0, 0);
            exitDialog();
            break;
        case R.id.server_clear_msg_button:
            txtVwServerMsg.setText("");
            break;
        case R.id.client_clear_msg_button:
            txtVwClientMsg.setText("");
            break;
        }
    }

    //** To handle SNEP Client RUN */
    public void handleSnepRunClient() {
        Log.d(PhUtilities.UI_TAG, "In handleSnepRunClient");
        boolean nfcEnable = mNfcAdapter.isEnabled();
        if(!isServerRunning){
            isClientRunning=true;
        if(chkBoxShortRecordLayout.isChecked()){
            testCaseID=testCaseID+20;
        }
        txtVwClientMsg.setText("");
        txtVwServerMsg.setText("");
        chkBoxShortRecordLayout.setEnabled(false);
        runClientBtnClicked=true;
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this.mContext);
        Log.e("mNfcAdapter.isEnabled()", ""+mNfcAdapter.isEnabled());
        runClientBtn.setVisibility(View.INVISIBLE);
        stopClientBtn.setVisibility(View.VISIBLE);
        if(!nfcEnable){
            loadingProgress = ProgressDialog.show(mContext, "",
                                                   "Enabling Please Wait...");
            new Thread() {
                public void run() {
                    try {
                        Log.d(PhUtilities.UI_TAG,"calling snepDtaCmd...");
                        snepDtaCmd("enabledta", null, 0, 0, 0, 0);
                        mNfcAdapter.enable();
                        threadSleep(5000);
                        Log.d("testCasesID", ""+testCaseID);
                        if(testCaseID>0 && testCaseID <=6){
                            snepDtaCmd("enableclient", "urn:nfc:sn:snep",
                                       13, 128, 1, testCaseID);
                        }else {
                            snepDtaCmd("enableclient", "urn:nfc:sn:sneptest",
                                       13, 128, 1, testCaseID);
                        }
                    } catch (NoSuchMethodError e) {
                        e.printStackTrace();
                    }catch (NoClassDefFoundError e) {
                        e.printStackTrace();
                    }
                    loadingProgress.dismiss();
                }
            }.start();
            }else if(!spinnerClientTestCaseIds.getSelectedItem().toString().equals(
                    R.string.select_client_test_case)){
            Log.d("testCasesID", ""+testCaseID);
            threadSleep(500);
            if(testCaseID>0 && testCaseID <=6){
                snepDtaCmd("enableclient", "urn:nfc:sn:snep", 13,
                           128, 1, testCaseID);
            }else {
                snepDtaCmd("enableclient", "urn:nfc:sn:sneptest", 13,
                           128, 1, testCaseID);
            }
        }
        }else{
            Toast.makeText(mContext, "Please Stop the Snep Server",
                            Toast.LENGTH_SHORT).show();
        }
    }

    //** To handle SNEP Client STOP */
    public void handleSnepStopClient() {
        Log.d(PhUtilities.UI_TAG, "In handleSnepStopClient");
        isClientRunning=false;
        testCaseID=spinnerClientTestCaseIds.getSelectedItemPosition();
        chkBoxShortRecordLayout.setEnabled(true);
        runClientBtnClicked=false;
        txtVwClientMsg.setText("");
        txtVwServerMsg.setText("");
        runClientBtn.setVisibility(View.VISIBLE);
        stopClientBtn.setVisibility(View.INVISIBLE);
        snepDtaCmd("disableclient", null, 0, 0, 0, 0);
        stopServer(mNfcAdapter.isEnabled());
    }

    //** To handle SNEP Server RUN */
    public void handleSnepRunServer() {
        Log.d(PhUtilities.UI_TAG, "In handleSnepRunServer");
        boolean nfcEnable = mNfcAdapter.isEnabled();
        if (!isClientRunning) {
            runServerBtnClicked = true;
            isServerRunning = true;
            mNfcAdapter = NfcAdapter.getDefaultAdapter(this.mContext);
            stopServerBtn.setVisibility(View.VISIBLE);
            runServerBtn.setVisibility(View.INVISIBLE);
            txtVwClientMsg.setText("");
            txtVwServerMsg.setText("");
            if (!nfcEnable) {
                loadingProgress = ProgressDialog.show(mContext, "",
                                            "Enabling Please Wait...");
                new Thread() {
                    public void run() {
                        try {
                            Log.d(PhUtilities.UI_TAG, "calling snepDtaCmd...");
                            snepDtaCmd("enabledta", null, 0, 0, 0, 0);
                            mNfcAdapter.enable();
                            threadSleep(5000);
                            snepDtaCmd("enableserver", "urn:nfc:sn:sneptest",
                                       13, 128, 1, testCaseID);
                        } catch (Exception e) {
                        }
                        loadingProgress.dismiss();
                    }
                }.start();
            } else {
                runServerBtnClicked = true;
                threadSleep(500);
                snepDtaCmd("enableserver", "urn:nfc:sn:sneptest", 13,
                           128, 1, 0);
            }
        } else {
            Toast.makeText(mContext, "Please Stop Snep Client",
                            Toast.LENGTH_SHORT).show();
        }
    }

    //** To handle SNEP Server STOP */
    public void handleSnepStopServer() {
        Log.d(PhUtilities.UI_TAG, "In handleSnepStopServer");
        runServerBtnClicked=false;
        isServerRunning=false;
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this.mContext);
        stopServerBtn.setVisibility(View.INVISIBLE);
        runServerBtn.setVisibility(View.VISIBLE);
        txtVwServerMsg.setText("");
        txtVwClientMsg.setText("");
        stopServer(mNfcAdapter.isEnabled());
    }

    //** To handle SNEP Back Button Press to exit to MainActivity */
    public void handleBackBtnSnep() {
        snepDtaCmd("disableDta",null,0,0,0,0);
        //exitDialog();
        dismissTheDialog();
    }

    private void stopServer(boolean nfcEnable) {
         if(nfcEnable){
             loadingProgress = ProgressDialog.show(mContext, "",
                                                "Disabling Please Wait...");
             new Thread() {
                 public void run() {
                     try {
                         mNfcAdapter.disable();
                         threadSleep(2000);
                         snepDtaCmd("disableserver", null, 0, 0, 0, 0);
                         threadSleep(500);
                     } catch (NoSuchMethodError e) {
                         e.printStackTrace();
                     }catch (NoClassDefFoundError e) {
                         e.printStackTrace();
                     }
                     loadingProgress.dismiss();
                 }
             }.start();
         }
    }

    AlertDialog.Builder alertBuilder;

    public void exitDialog(){
        alertBuilder=new AlertDialog.Builder(mContext);
        alertBuilder.setMessage("Do you want to exit from SNEP testing and \n go back to Main Menu?");
        alertBuilder.setPositiveButton("Yes", new OnClickListener() {
        @Override
        public void onClick(DialogInterface arg0, int arg1) {
                dismissTheDialog();
            }
        });
        alertBuilder.setNegativeButton("No", new OnClickListener() {
        @Override
        public void onClick(DialogInterface arg0, int arg1) {
        }
        });
        alertBuilder.create();
        alertBuilder.show();
    }

    public void dismissTheDialog(){
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this.mContext);
        Log.d("mNfcAdapter.isEnabled()", ""+mNfcAdapter.isEnabled());
        if(mNfcAdapter.isEnabled()){
            try {
                mNfcAdapter.disable();
            } catch (NoSuchMethodError e) {
                e.printStackTrace();
            }catch (NoClassDefFoundError e) {
                e.printStackTrace();
            }
        }
        if (!isNfcInitInDtaMode) {
            loadingProgress = ProgressDialog.show(mContext, "",
                                                    "Please Wait...");
            new Thread() {
                public void run() {
                    try {
                        dismiss();
                    } catch (Exception e) {
                        Log.e("tag", e.getMessage());
                    }
                    loadingProgress.dismiss();
                }
            }.start();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(event.KEYCODE_BACK==keyCode){
            exitDialog();
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onNdefPushComplete(NfcEvent event) {
        mHandler.obtainMessage(MESSAGE).sendToTarget();
        Log.w(TAG, "sending message "
                + mHandler.obtainMessage(MESSAGE).toString());
    }

    @Override
    public NdefMessage createNdefMessage(NfcEvent arg0) {
        String text = ("Beam me up!\n\n" + "Beam Time: ");
        NdefMessage msg = new NdefMessage(new NdefRecord[] { createMimeRecord(
                "application/com.example.android.beam", text.getBytes()) });
        return msg;
    }

    public NdefRecord createMimeRecord(String mimeType, byte[] payload) {
        byte[] mimeBytes = mimeType.getBytes(Charset.forName("US-ASCII"));
        NdefRecord mimeRecord = new NdefRecord(NdefRecord.TNF_MIME_MEDIA,
                mimeBytes, new byte[0], payload);
        return mimeRecord;
    }

    /** This handler receives a message from onNdefPushComplete */
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE: {
                Toast.makeText(mContext, "Message sent!", Toast.LENGTH_SHORT)
                        .show();
                break;
            }
            case MESSAGE_RECEIVE: {
                Toast.makeText(mContext, "Message Received!",
                        Toast.LENGTH_SHORT).show();
                // only one message sent during the beam
                NdefMessage ndefMsg = (NdefMessage) msg.obj;
                // record 0 contains the MIME type, record 1 is the AAR, if
                // present
                byte[] b_msg = ndefMsg.getRecords()[0].getPayload();
                strBuffer = new StringBuffer();
                strBuffer.append("Receive: ");
                strBuffer.append("legnth = " + b_msg.length);
                strBuffer.append('\n');
                strBuffer.append(new String(b_msg));
                strBuffer.append('\n');
                strBuffer.append("[" + toHexString(b_msg, 0, b_msg.length)
                        + "]");
                strBuffer.append('\n');
                break;
            }
            case MESSAGE_LLCP_ACTIVATED: {
                Toast.makeText(mContext, "LLCP activated!", Toast.LENGTH_SHORT)
                        .show();
                break;
            }
            case MESSAGE_LLCP_DEACTIVATED: {
                Toast.makeText(mContext, "LLCP deactivated!",
                        Toast.LENGTH_SHORT).show();
                break;
            }
            }
        }
    };
    private String toHexString(byte[] buffer, int offset, int length) {
        final char[] HEX_CHARS = "0123456789abcdef".toCharArray();
        char[] chars = new char[3 * length];
        for (int j = offset; j < offset + length; ++j) {
            chars[3 * j] = HEX_CHARS[(buffer[j] & 0xF0) >>> 4];
            chars[3 * j + 1] = HEX_CHARS[buffer[j] & 0x0F];
            chars[3 * j + 2] = ' ';
        }
        return new String(chars);
    }

    /** Getter & Setter for Test Case ID for both SNEP Clinet as well as SNEP server*/
    public int getTestCaseID() {
        return testCaseID;
    }

    public void setTestCaseID(int testCaseID) {
        /** The test case ID received form AutomaTest commands in mainactivity will
            be set in phCustomSNEPRTD.java */
        this.testCaseID = testCaseID;
        /** This is only to avoid manual selection from UI invocation */
        AUTOMATEST_FLAG = true;
    }

    public class DeInitDTA extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... params) {
            return null;
        }
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            try {
                if(PhUtilities.NDK_IMPLEMENTATION==true){
                    PhNXPJniHelper phNXPJniHelper = PhNXPJniHelper.getInstance();
                    phNXPJniHelper.phDtaLibDeInit();
                }
            } catch (UnsatisfiedLinkError e) {
                e.printStackTrace();
            } catch (NoClassDefFoundError e) {
                e.printStackTrace();
            } catch (NoSuchMethodError e) {
                e.printStackTrace();
            }

        }
        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
        }
    }
    @Override
    public void onItemSelected(AdapterView<?> arg0, View view, int arg2,
            long arg3) {
        switch (arg0.getId()) {
        case R.id.client_test_case_id:
            clientTestCaseIdSelection();
            break;

        case R.id.server_test_case_id:
            serverTestCaseIdSelection();
            break;
        }
    }

    public void snepDtaCmd(String snepCmdType, String snepServiceName, int serviceSap,
            int snepMiu, int snepRwSize, int snepTestCaseId) {
        boolean result = false;
        try{
            if(snepCmdType.equals("enabledta")) {
                result = mNfcDta.registerMessageService("com.phdtaui.messageservice.ACTION_BIND");
                result = mNfcDta.enableDta();
            }else if(snepCmdType.equals("disableDta")) {
                result = mNfcDta.disableDta();
            }else if(snepCmdType.equals("enableserver")) {
                result = mNfcDta.enableServer(snepServiceName, serviceSap, snepMiu, snepRwSize, snepTestCaseId );
            }else if(snepCmdType.equals("disableserver")) {
                result = mNfcDta.disableServer();
            }else if(snepCmdType.equals("enableclient")) {
                result = mNfcDta.enableClient(snepServiceName, snepMiu, snepRwSize, snepTestCaseId );
            }else if(snepCmdType.equals("disableclient")) {
                result = mNfcDta.disableClient();
            }
        }catch (NoSuchMethodError e) {
            e.printStackTrace();
        }catch (NoClassDefFoundError e) {
            e.printStackTrace();
        }
    }
    @Override
    public void onNothingSelected(AdapterView<?> arg0) {

    }
    public void threadSleep(long sleep) {
        try {
            Thread.sleep(sleep);
        } catch (InterruptedException e) {
        }
    }

    public void serverTestCaseIdSelection(){
        if(!AUTOMATEST_FLAG) {
            testCaseID = spinnerServerTestCaseIds.getSelectedItemPosition();
        }else{
            Log.d("In serverTestCaseIdSelection", (String.valueOf(testCaseID)));
            this.spinnerServerTestCaseIds.setSelection(testCaseID);
        }
        if(spinnerServerTestCaseIds.getSelectedItem().equals(getContext().getResources().getString(
                R.string.select_server_test_case))){
            Log.d(PhUtilities.UI_TAG,spinnerServerTestCaseIds.getSelectedItem().toString());
            runServerBtn.setEnabled(false);
        }else if(spinnerServerTestCaseIds.getSelectedItem().equals(getContext().getResources()
                .getString(R.string.other_test_case))){
            Log.d(PhUtilities.UI_TAG,spinnerServerTestCaseIds.getSelectedItem().toString());
            testCaseID = 0;
            if(runServerBtnClicked){
                runServerBtn.setEnabled(true);
                snepDtaCmd("disableserver", null, 0, 0, 0, 0);
                runServerBtnClicked=false;
            }
            runServerBtn.setEnabled(true);
            runServerBtn.setVisibility(View.VISIBLE);
            stopServerBtn.setVisibility(View.INVISIBLE);
        }else if(spinnerServerTestCaseIds.getSelectedItem().equals(getContext().getResources()
                .getString(R.string.tc_s_ret_bi_01))){
            Log.d(PhUtilities.UI_TAG,spinnerServerTestCaseIds.getSelectedItem().toString());
            testCaseID = 1;
            if(runServerBtnClicked){
                snepDtaCmd("disableserver", null, 0, 0, 0, 0);
                runServerBtnClicked=false;
            }
            runServerBtn.setEnabled(true);
            Log.d(PhUtilities.UI_TAG,spinnerServerTestCaseIds.getSelectedItem().toString());
            runServerBtn.setVisibility(View.VISIBLE);
            stopServerBtn.setVisibility(View.INVISIBLE);
        }
    }

    public void clientTestCaseIdSelection(){

        if(!AUTOMATEST_FLAG) {
            testCaseID = spinnerClientTestCaseIds.getSelectedItemPosition();
        }else{
            Log.d("In clientTestCaseIdSelection", (String.valueOf(testCaseID)));
            this.spinnerClientTestCaseIds.setSelection(testCaseID);
        }
        Log.d(PhUtilities.UI_TAG,spinnerClientTestCaseIds.getSelectedItem().toString());
        if(spinnerClientTestCaseIds.getSelectedItem().equals(getContext().getResources()
                .getString(R.string.select_client_test_case))){
            runClientBtn.setEnabled(false);
        }
        else if (testCaseID > 0 && testCaseID < 7) {
            if(runClientBtnClicked){
                runClientBtnClicked=false;
                snepDtaCmd("disableclient", null, 0, 0, 0, 0);
                stopServer(mNfcAdapter.isEnabled());
            }
            spinnerClientTestCaseIds.getSelectedItemPosition();
            stopClientBtn.setVisibility(View.INVISIBLE);
            runClientBtn.setVisibility(View.VISIBLE);
            runClientBtn.setEnabled(true);
            Log.d(PhUtilities.UI_TAG, "<7" + testCaseID);
        } else if (testCaseID > 6 && testCaseID <= 9) {
            if(runClientBtnClicked){
                runClientBtnClicked=false;
                snepDtaCmd("disableclient", null, 0, 0, 0, 0);
                stopServer(mNfcAdapter.isEnabled());
            }
            Log.d(PhUtilities.UI_TAG, ">7" + testCaseID);
            runClientBtn.setEnabled(true);
            stopClientBtn.setVisibility(View.INVISIBLE);
            runClientBtn.setVisibility(View.VISIBLE);
        }
    }
@Override
protected void onStart() {
    if(!mNfcAdapter.isEnabled()){
        /*Handle scenrio when Open App->Snep->Run Server->Home Btn->Settings->NfcSvc Off
         * ->BacktoApp*/
        Toast.makeText(mContext, "SNEP Started", Toast.LENGTH_SHORT).show();
        isServerRunning=false;
        isClientRunning=false;
        runServerBtn.setVisibility(View.VISIBLE);
        stopServerBtn.setVisibility(View.INVISIBLE);
        runClientBtn.setVisibility(View.VISIBLE);
        stopClientBtn.setVisibility(View.INVISIBLE);
        spinnerClientTestCaseIds.setSelection(0);
        spinnerServerTestCaseIds.setSelection(0);
        runClientBtn.setEnabled(false);
    }
    //Register BroadcastReceiver
    //to receive event from our service
    snepDataReciever = new SnepBroadcastReciever();
    IntentFilter intentFilter = new IntentFilter();
    intentFilter.addAction(PhMessengerService.MSG_UPDATE_GUI);
    mContext.registerReceiver(snepDataReciever, intentFilter);

    super.onStart();
}
@Override
protected void onStop() {
    super.onStop();
    Toast.makeText(mContext, "SNEP Stopped", Toast.LENGTH_SHORT).show();
}
private class SnepBroadcastReciever extends BroadcastReceiver{

    @Override
    public void onReceive(Context arg0, Intent arg1) {
        String ndefMessage = arg1.getStringExtra("NDEF_MSG_FROM_NFC_SVC");
        Log.e(PhUtilities.UI_TAG, ndefMessage);
        /*Set Text on the GUI Message window(Client/Server)*/
        if(txtVwClientMsg != null){
            if(isClientRunning)
                txtVwClientMsg.setText(ndefMessage);
            if(isServerRunning)
                txtVwServerMsg.setText(ndefMessage);
        }
    }
   }
}
