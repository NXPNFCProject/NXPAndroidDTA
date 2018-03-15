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

import java.net.UnknownHostException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.provider.Settings;
import android.text.Editable;
import android.text.SpannableStringBuilder;
import android.text.TextWatcher;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.Toast;

import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;
import com.phdtaui.utils.PhUtilities;

public class PhCustomIPDialog extends Dialog implements
        android.view.View.OnClickListener, OnCheckedChangeListener,
        android.widget.CompoundButton.OnCheckedChangeListener {

    private EditText ipAdressText, portNumberText, numberOfRetries;
    private Button doneButton;
    private RadioButton clientRadioButton, serverRadioButton,simualteCltRadioButton, simualteSerRadioButton;

    private EditText logClientMsg, logServerMsg;
    private CheckBox clientCheckBox, serverCheckBox;
    private Context mContext;
    private String fileName;
    private boolean serverToggle, clientToggle;
    private boolean isErrorSet = false;
    RelativeLayout numberOfRetriesLayout;

    /**
     * @param Constructor used for the Base Context or Parent Activity
     */
    public PhCustomIPDialog(Context context) {
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
        setContentView(R.layout.ph_custom_ip_dialog_layout);
        setCanceledOnTouchOutside(false);
        /**
         *Radio Group of Client,Server Radio Group id's declared
         */
        RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radio_group);
        /**
         *Radio Button of Client and Server Radio Button and Simulated Client and Simulated Server Radio Button id's declared
         */
        clientRadioButton = (RadioButton) findViewById(R.id.client_radio_btn);
        serverRadioButton = (RadioButton) findViewById(R.id.server_radio_btn);
        simualteCltRadioButton = (RadioButton) findViewById(R.id.simulate_client_rb_btn);
        simualteSerRadioButton = (RadioButton) findViewById(R.id.simulate_server_rb_btn);
        ipAdressText = (EditText) findViewById(R.id.ip_address_edit_txt);
        portNumberText = (EditText) findViewById(R.id.port_number_edittxt);
        numberOfRetries = (EditText) findViewById(R.id.number_of_retries_edit_text);
        /**
         * Edit Text id's for Client and Server  declared here
         */
        logClientMsg = (EditText) findViewById(R.id.client_cb_edit_txt);
        logServerMsg = (EditText) findViewById(R.id.server_cb_edit_txt);
        /**
         *Done Button id's declared here
         */
        doneButton = (Button) findViewById(R.id.ok_btn);
        /**
         *CheckBox id's declared for Client Server here
         */
        clientCheckBox = (CheckBox) findViewById(R.id.client_cb);
        serverCheckBox = (CheckBox) findViewById(R.id.server_cb);

        /**
         * Relative layout for number of retries
         */
        numberOfRetriesLayout = (RelativeLayout) findViewById(R.id.number_of_retry);
        /**
         *Registering the radio group  listener
         */
        radioGroup.setOnCheckedChangeListener(this);
        /**
         * Check Client is selected
         */
        if (clientRadioButton.isChecked()) {
            simualteSerRadioButton.setOnClickListener(this);
            simualteCltRadioButton.setEnabled(false);
        }
        /**
         * Registering the Done Button Listener
         */
        doneButton.setOnClickListener(this);

        /**
         * Handled the Device Keyboard
         */
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

             /**
         * Registering the On Click Listener for Client and Server Log
         */
        logClientMsg.setFocusable(false);
        logClientMsg.setOnClickListener(this);
        clientCheckBox.setEnabled(false);
        clientCheckBox.setOnClickListener(this);
        clientCheckBox.setOnCheckedChangeListener(this);
        logServerMsg.setFocusable(false);
        logServerMsg.setOnClickListener(this);
        serverCheckBox.setEnabled(false);
        serverCheckBox.setOnClickListener(this);
        serverCheckBox.setOnCheckedChangeListener(this);
        ipAdressText.setOnClickListener(this);
        portNumberText.setOnClickListener(this);

    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {
        /**
         * OK Button Listener
         */
        case R.id.ok_btn:
            if (!serverRadioButton.isChecked()) {
                isErrorSet = validateIpaddressTextBox(ipAdressText.getText().toString());
                isErrorSet = isErrorSet || validatePortNumberTextBox(portNumberText.getText().toString());
            }else if(serverRadioButton.isChecked()){
                isErrorSet = validatePortNumberTextBox(portNumberText.getText().toString());
            }
            if(!isErrorSet){
                try {
                   //((PhDTAUIMainActivity) this.getOwnerActivity())
                           ((PhDTAUIMainActivity) this.mContext).setIpAdressAndPortNumber(ipAdressText.getText().toString(), portNumberText.getText().toString());
                } catch (UnknownHostException e) {
                    e.printStackTrace();
                }
            }
            /**
             * Client  is Enabled
             */
            if (clientCheckBox.isChecked()) {

                fileName = logClientMsg.getText().toString();
                PhUtilities.LOG_DUT_MESSAGES = true;
                PhUtilities.FILE_NAME_DUT = fileName + ".txt";
            }
            /**
             * Server  is Enabled
             */
            if (serverCheckBox.isChecked()) {
                fileName = logServerMsg.getText().toString();
                PhUtilities.LOG_LT_MESSAGES = true;
                PhUtilities.FILE_NAME_LT = fileName + ".txt";
            }

                     if (clientRadioButton.isChecked()) {
                /*Log.d(Utilities.TCPCNT_TAG, "Starting TCP/IP Client. On Done button click.");
                new NioClientTask(null, 8887)
                        .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);*/
                PhUtilities.CLIENT_SELECTED = true;
                if (simualteSerRadioButton.isChecked()) {
                    /*Log.d(Utilities.TCPSRV_TAG, "Starting TCP/IP Server Simulator. On Done button click.");
                    new NioServerTask()
                            .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);*/
                    PhUtilities.SIMULATED_SERVER_SELECTED = true;
                }

            } else if (serverRadioButton.isChecked()) {
                /*Log.d(Utilities.TCPSRV_TAG, "Starting TCP/IP Server. On Done button click.");
                new NioServerTask()
                        .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);*/
                PhUtilities.SERVER_SELECTED = true;
                if (simualteCltRadioButton.isChecked()) {
                    /*Log.d(Utilities.TCPCNT_TAG, "Starting TCP/IP Client Simulator. On Done button click.");
                    new NioClientTask(null, 8887)
                            .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);*/
                    PhUtilities.SIMULATED_CLIENT_SELECTED = true;
                }
            }
            isErrorSet = isErrorSet || !isConnectedWiFi(getContext());
            if(!isConnectedWiFi(getContext())){
                Toast.makeText(getContext(), "Device is not Connected to WIFI", Toast.LENGTH_SHORT).show();
                Builder wifiSettinsDialog = new AlertDialog.Builder(this.mContext);
                wifiSettinsDialog.setTitle("WiFi Settings")
                .setMessage("Device is not connected to WiFi Please connect to WiFi before proceeding")
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        mContext.startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
                    }
                 })
                .setNegativeButton(android.R.string.no, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.cancel();
                    }
                 })
                 .show();
            }
            PhUtilities.maxNumberOfRetries = numberOfRetries.getText().toString().isEmpty() ? 0 : Integer.parseInt(numberOfRetries.getText().toString());
            Log.d(PhUtilities.TCPCNT_TAG, "Maximum number Of retries" + PhUtilities.maxNumberOfRetries);
            if(!isErrorSet){
                dismiss();
            }
            break;

            /**
             * Simulated Client action is here
             */
        case R.id.simulate_client_rb_btn:
            ipAdressText.setEnabled(true);
            ipAdressText.setHint("127.0.0.1");
            if (simualteCltRadioButton.isChecked() && !clientToggle) {
                simualteCltRadioButton.setChecked(true);
                clientToggle = true;
            } else if (serverToggle) {
                simualteCltRadioButton.setChecked(false);
                clientToggle = false;
            }

            if(simualteCltRadioButton.isChecked()){
                clientToggle = true;
                ipAdressText.setEnabled(false);
                ipAdressText.setHint("         ");
                ipAdressText.setText("");
            }
            break;
            /**
             * Simulated Server action is here
             */
        case R.id.simulate_server_rb_btn:
            if (simualteSerRadioButton.isChecked() && !serverToggle) {
                simualteSerRadioButton.setChecked(true);
                serverToggle = true;
            } else if (serverToggle) {
                simualteSerRadioButton.setChecked(false);
                serverToggle = false;
            }
            break;
            /**
             * Client Check Box for show Client Message
             */
        case R.id.client_cb_edit_txt:
            logClientMsg();
            break;
            /**
             * Server Check Box for show Server Message
             */
        case R.id.server_cb_edit_txt:
            logServerMsg();
            break;

        }
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {

        switch (group.getCheckedRadioButtonId()) {
             /**
         * Client Radio Button is Enabled it will execute
         */
        case R.id.client_radio_btn:
            ipAdressText.setEnabled(true);
            ipAdressText.setHint("127.0.0.1");
            numberOfRetriesLayout.setVisibility(View.VISIBLE);
            if (clientRadioButton.isChecked()) {
                simualteSerRadioButton.setEnabled(true);
                simualteSerRadioButton.setOnClickListener(this);
                simualteCltRadioButton.setEnabled(false);
                simualteCltRadioButton.setChecked(false);
            }

            break;
            /**
             * Server Radio Button is Enabled it will execute
             */
        case R.id.server_radio_btn:

            if (serverRadioButton.isChecked()) {
                simualteCltRadioButton.setEnabled(true);
                simualteSerRadioButton.setChecked(false);
                simualteCltRadioButton.setOnClickListener(this);
                simualteSerRadioButton.setEnabled(false);
                ipAdressText.setEnabled(false);
                ipAdressText.setHint("         ");
                ipAdressText.setText("");
                numberOfRetriesLayout.setVisibility(View.GONE);
                if(simualteCltRadioButton.isChecked()){
                    ipAdressText.setEnabled(true);
                    ipAdressText.setHint("127.0.0.1");
                }
            }

            break;
        }
    }

    /**
     * Set File Name for Client
     */
    public void logClientMsg() {
        logClientMsg.setFocusable(true);
        logClientMsg.setFocusableInTouchMode(true);
        logClientMsg.setEnabled(true);
        logClientMsg.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable editText) {
            }

            @Override
            public void beforeTextChanged(CharSequence arg0, int arg1,
                    int arg2, int arg3) {
            }

            @Override
            public void onTextChanged(CharSequence textInEditBox, int arg1,
                    int arg2, int count) {
                clientCheckBox = (CheckBox) findViewById(R.id.client_cb);
                logClientMsg = (EditText) findViewById(R.id.client_cb_edit_txt);
                if (textInEditBox.length() < 1) {
                    Toast toast = Toast.makeText(mContext,
                            "Enter File Name To Store Log", Toast.LENGTH_SHORT);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    clientCheckBox.setEnabled(false);
                    clientCheckBox.setFocusable(false);
                    clientCheckBox.setChecked(false);
                } else {
                    clientCheckBox.setEnabled(true);
                }
            }
        });
    }
    /**
     * Set File Name for Server
     */
    public void logServerMsg() {
        logServerMsg.setFocusable(true);
        logServerMsg.setFocusableInTouchMode(true);
        logServerMsg.setEnabled(true);
        logServerMsg.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable editText) {
                         }

            @Override
            public void beforeTextChanged(CharSequence arg0, int arg1,
                    int arg2, int arg3) {
            }

            @Override
            public void onTextChanged(CharSequence textInEditBox, int arg1,
                    int arg2, int count) {
                serverCheckBox = (CheckBox) findViewById(R.id.server_cb);
                logServerMsg = (EditText) findViewById(R.id.server_cb_edit_txt);
                if (textInEditBox.length() < 1) {
                    Toast toast = Toast.makeText(mContext,
                            "Enter File Name To Store Log", Toast.LENGTH_SHORT);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    serverCheckBox.setEnabled(false);
                    serverCheckBox.setFocusable(false);
                    serverCheckBox.setChecked(false);
                } else {
                    serverCheckBox.setEnabled(true);
                }
            }
        });
    }
     /**
     * Check Client is selected or Server Radio Button is selected
     */

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

        switch (buttonView.getId()) {
        /**
         * Client is selected
         */
        case R.id.client_cb:
            if (isChecked) {
                logClientMsg.setFocusable(true);
                logClientMsg.setFocusableInTouchMode(true);
            } else {
                logClientMsg.setFocusable(false);
                logClientMsg.setFocusableInTouchMode(false);
                logClientMsg.setText("");
            }
            break;
            /**
             * Server is selected
             */

        case R.id.server_cb:
            if (isChecked) {
                logServerMsg.setFocusable(true);
                logServerMsg.setFocusableInTouchMode(true);
            } else {
                logServerMsg.setFocusable(false);
                logServerMsg.setFocusableInTouchMode(false);
                logServerMsg.setText("");
            }
            break;
        }

    }

    /**
     * Validates IpAddress TextBox
     * @param ipAddress
     */
    protected boolean validateIpaddressTextBox(String ipAddress){
        if(validateIfTextBoxIsEmpty(ipAddress, ipAdressText)){
            return true;
        }else if(validateIpAddress(ipAddress)){
            return true;
        }
        return false;
    }

    /**
     * Validates PortNumber TextBox
     * @param portNumber
     */
    protected boolean validatePortNumberTextBox(String portNumber){
        if(validateIfTextBoxIsEmpty(portNumber, portNumberText)){
            return true;
        }else if(validatePortNumber(Integer.parseInt(portNumber))){
            return true;
        }
        return false;
    }

    /**
     * This method validates if text box is empty if it is empty it sets an error message to
     * text box
     * @param ipAddress
     */
    protected boolean validateIfTextBoxIsEmpty(String text, EditText textField) {
        if("".equals(text) || text == null){
            String errorString = "Field cannot be empty";
            ForegroundColorSpan errorStringColor = new ForegroundColorSpan(-65536);
            SpannableStringBuilder errorMessage = new SpannableStringBuilder(errorString);
            errorMessage.setSpan(errorStringColor, 0, errorString.length(), 0);
            textField.setError(errorMessage);
            return true;
        }else{
            return false;
        }
    }

    /**
     * This method validates port number captured from user interface and sets error message
     * if port number is not valid
     * @param portNumber
     */
    protected boolean validatePortNumber(int portNumber){
        String errorString = null;
        boolean isPortNumberCorrect = false;
        /**
         * Port numbers are valid from 0 - 65535 but they are reserved from 1 to 1024
         */
        if(portNumber < 0 || portNumber > 65535){
            errorString = "Port Number is out of Range";
            isPortNumberCorrect = true;
        }else if(portNumber >=1 && portNumber <= 1024){
            errorString = "Port Number given is reserved";
            isPortNumberCorrect = true;
        }
        if(isPortNumberCorrect){
            isErrorSet = true;
            ForegroundColorSpan errorStringColor = new ForegroundColorSpan(-65536);
            SpannableStringBuilder errorMessage = new SpannableStringBuilder(errorString);
            errorMessage.setSpan(errorStringColor, 0, errorString.length(), 0);
            portNumberText.setError(errorMessage);
            return true;
        }
        return false;
    }

    /**
     * This method validates ipAddress captures from UI and sets message if IpAddress is not valid
     * @param ipAddress
     */
    protected boolean validateIpAddress(String ipAddress){
        /**
         * Pattern to match IPV4 address
         */
        final String IPV4_MATCHING_PATTERN =
                "^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";

        Pattern pattern = Pattern.compile(IPV4_MATCHING_PATTERN);
        Matcher matcher = pattern.matcher(ipAddress);
        if(!matcher.matches()){
            String errorString = "Given IpAddress is not valid";
            ForegroundColorSpan errorStringColor = new ForegroundColorSpan(-65536);
            SpannableStringBuilder errorMessage = new SpannableStringBuilder(errorString);
            errorMessage.setSpan(errorStringColor, 0, errorString.length(), 0);
            ipAdressText.setError(errorMessage);
            return true;
        }
        return false;
    }

    /**
     * This method returns if the device is connected to wifi
     * @param context
     * @return
     */
    private static boolean isConnectedWiFi(Context context) {

        ConnectivityManager wifiManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo phWifi = wifiManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

            if (phWifi.isConnected()) {

                return true;

            } else {

                return false;
            }

    }
}
