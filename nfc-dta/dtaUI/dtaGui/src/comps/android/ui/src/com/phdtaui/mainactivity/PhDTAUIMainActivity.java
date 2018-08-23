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

package com.phdtaui.mainactivity;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.concurrent.TimeUnit;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Color;
import android.nfc.NfcAdapter;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.text.Editable;
import android.text.Html;
import android.text.Selection;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import com.phdtaui.asynctask.PhNioClientTask;
import com.phdtaui.asynctask.PhNioServerTask;
import com.phdtaui.customdialogs.PhCustomExitDialog;
import com.phdtaui.customdialogs.PhCustomIPDialog;
import com.phdtaui.customdialogs.PhCustomLibDialog;
import com.phdtaui.customdialogs.PhCustomMSGDialog;
import com.phdtaui.customdialogs.PhCustomPopValidation;
import com.phdtaui.customdialogs.PhCustomSNEPRTD;
import com.phdtaui.customdialogs.PhShowMSGDialog;
import com.phdtaui.helper.PhNXPHelperMainActivity;
import com.phdtaui.helper.PhNXPJniHelper;
import com.phdtaui.structure.PhDtaLibStructure;
import com.phdtaui.structure.PhDtaLibStructure.PhRfTechnology;
import com.phdtaui.utils.PhUtilities;
import android.view.WindowManager;
import com.phdtaui.helper.PhDTAJniEvent;
import java.io.File;


public class PhDTAUIMainActivity extends Activity implements
        android.view.View.OnClickListener,
        android.widget.RadioGroup.OnCheckedChangeListener,
        android.widget.CompoundButton.OnCheckedChangeListener,
        android.widget.AdapterView.OnItemSelectedListener {

    /*Pattern number related view elements*/
    private Spinner spinnerPatterNum, certificationVersion, timeSlotNumberF, connectionDeviceLimit;
    private RelativeLayout tsnRelativeLyt, connDevRelativeLyt;
    private EditText editTextCustomPatternNum;

    /*Test execution view elements*/
    private Button runBtn, stopBtn, exitBtn;

    /*RF Technolgy group view elements*/
    private CheckBox chkBoxPollP2p, chkBoxPollRdWt,
                     chkBoxListenP2p, chkBoxListenUicc, chkBoxListenHce, chkBoxListenEse,
                     chkBoxPollP2PA, chkBoxPollP2PB, chkBoxPollP2PF,
                     chkBoxPollRDWTA, chkBoxPollRDWTB, chkBoxPollRDWTF ,
                     chkBoxListenP2PA, chkBoxListenP2PB, chkBoxListenP2PF,
                     chkBoxListenUICCA, chkBoxListenUICCB, chkBoxListenUICCF,
                     chkBoxListenHCEA, chkBoxListenHCEB, chkBoxListenHCEF,
                     chkBoxListenESEA, chkBoxListenESEB, chkBoxListenESEF;

    /*Auto test execution mode view elements*/
    private CheckBox customMessage,showMessage;
    private RadioButton radioBtnAutoMode, radioBtnManualMode;
    private RadioGroup autoManualRadioGroup;

    /*Utilities view elements*/
    /*Logging*/
    private EditText editTxtLogFileName;
    private CheckBox chkBoxLogCat,chkBoxLogToFile;
    private TextView txtVwLogMsg;

    /*Device SW/MW/FW Version numbers*/
    private CheckBox chkBoxInfo;
    private TextView txtVwVerNum,txtVwCopyRights,txtVwFwVer;

    /*status of application: Running/Stopped*/
    private  TextView txtVwAppStatus;
    /*For LLCP or SNEP test execution*/
    private CheckBox chkBoxLlcp, chkBoxLlcpConnectPduPrms, chkBoxSnep;

    private boolean onClickColoringRunning = false,
                    checkNfcServiceDialog = false ,
                    isRunClickedAtlLeastOnce = false;

    private String currentStatusStopped = "Current Status: <font color='red'> Stopped</font>" ,
            currentStatusRunning = "Current Status: <font color='#006600'> Running</font>" ,
            currentStatusInitializing = "Current Status: <font color='#006600'> Initializing</font>";
    private String logFileName;
    private String sPatternNumber , versionName;
    ArrayAdapter<CharSequence> adapterPatternNumberDefault,
                               adapterPatternNumberLlcp, adapterCleanPatternNumber;

    private PhNXPHelperMainActivity nxpHelperMainActivity = new PhNXPHelperMainActivity();
    private PhDtaLibStructure       phDtaLibStructureObj  = new PhDtaLibStructure();
    private PhCustomSNEPRTD         phCustomSNEPRTD;
    private NfcAdapter mNfcAdapter;
    private PhCustomPopValidation customPopValidation = null;
    private static boolean errorPopUP;
    private static String errorMsg;
    /**
     * Inet Address and port number for Wireless connection between DUT and LT
     */
    public static InetAddress inetAddress;
    public static int portNumber;
    public final static String DTA_GUI_VERSION = "10.01";

    /**
     * TO know whether Run button or Stop button is clicked
     */
    PhNXPJniHelper phNXPJniHelper = PhNXPJniHelper.getInstance();
    ProgressDialog clientConnectionProgress;

    /**
     * AutomaTest Related Code for working with Intents on different NFC Forum Test Tools
     */
    public final String BUNDLED_PARAMS_CERT_REL_KEY = "cert_rel";
    public final int CERT_REL_MIN_VALUE = 8;

    public final String BUNDLED_PARAMS_TSNF_KEY = "tsnf";
    public final String BUNDLED_PARAMS_TSNF_03_VALUE = "03";
    public final String BUNDLED_PARAMS_TSNF_F3_VALUE = "F3";

    public final String BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY = "conn_dev_limit";

    /** Digital Protocol Pattern Numbers Key */
    public final String BUNDLED_PARAMS_PATTERN_KEY = "pattern";

    /** Analog Pattern Number Key */
    public final String BUNDLED_PARAMS_ANALOG_PATTERN_KEY = "analog_pattern";

    public final String BUNDLED_PARAMS_LIS_P2P_MODE_KEY = "listen_p2p";
    public final String BUNDLED_PARAMS_LIS_P2P_ENABLE_VALUE = "enable";
    public final String BUNDLED_PARAMS_LIS_P2P_DISABLE_VALUE = "disable";
    public final String BUNDLED_PARAMS_HCE_MODE_KEY = "hce_mode";
    public final String BUNDLED_PARAMS_HCE_MODE_NONE_VALUE = "none";
    public final String BUNDLED_PARAMS_HCE_MODE_AB_VALUE = "ab";
    public final String BUNDLED_PARAMS_HCE_MODE_F_VALUE = "f";

    public final String BUNDLED_PARAMS_UICC_MODE_KEY = "uicc_mode";
    public final String BUNDLED_PARAMS_UICC_ENABLE_VALUE = "enable";
    public final String BUNDLED_PARAMS_UICC_DISABLE_VALUE = "disable";

    public final String BUNDLED_PARAMS_ESE_MODE_KEY = "ese_mode";
    public final String BUNDLED_PARAMS_ESE_ENABLE_VALUE = "enable";
    public final String BUNDLED_PARAMS_ESE_DISABLE_VALUE = "disable";

    public final String BUNDLED_PARAMS_LLCP_KEY = "llcp";
    public final String BUNDLED_PARAMS_LLCP_ENABLE_VALUE = "enable";
    public final String BUNDLED_PARAMS_LLCP_DISABLE_VALUE = "disable";
    public final String BUNDLED_PARAMS_LLCP_CONN_PDU_PARAMS_KEY = "llcp_conn_pdu_params";
    public final String BUNDLED_PARAMS_LLCP_ENABLE_CONN_PDU_PARAMS_VALUE = "enable";
    public final String BUNDLED_PARAMS_LLCP_DISABLE_CONN_PDU_PARAMS_VALUE = "disable";

    /**  LLCP Pattern Number Key */
    public final String BUNDLED_PARAMS_LLCP_PATTERN_KEY = "llcp_pattern";
    public final String BUNDLED_PARAMS_LLCP_PATTERN_1200 = "1200";
    public final String BUNDLED_PARAMS_LLCP_PATTERN_1240 = "1240";
    public final String BUNDLED_PARAMS_LLCP_PATTERN_1280 = "1280";

/** SNEP Related Parameter for AutomaTest */
    public final String BUNDLED_PARAMS_SNEP_KEY             = "snep";   // SNEP Key
    public final String BUNDLED_PARAMS_SNEP_ENABLE_VALUE    = "enable";
    public final String BUNDLED_PARAMS_SNEP_DISABLE_VALUE   = "disable";

    public final String BUNDLED_PARAMS_SNEP_CLIENT_KEY      = "snep_client"; // SNEP Client Related
    public final String BUNDLED_PARAMS_SNEP_CLIENT_RUN      = "client_run";
    public final String BUNDLED_PARAMS_SNEP_CLIENT_STOP     = "client_stop";
    public final String BUNDLED_PARAM_SNEP_TC_C_BIT_BV_01   = "TC_C_BIT_BV_01";
    public final String BUNDLED_PARAM_SNEP_TC_C_BIT_BI_01_0 = "TC_C_BIT_BI_01_0";
    public final String BUNDLED_PARAM_SNEP_TC_C_BIT_BI_01_1 = "TC_C_BIT_BI_01_1";
    public final String BUNDLED_PARAM_SNEP_TC_C_PUT_BV_01   = "TC_C_PUT_BV_01";
    public final String BUNDLED_PARAM_SNEP_TC_C_PUT_BV_02   = "TC_C_PUT_BV_02";
    public final String BUNDLED_PARAM_SNEP_TC_C_PUT_BI_01   = "TC_C_PUT_BI_01";
    public final String BUNDLED_PARAM_SNEP_TC_C_GET_BV_01   = "TC_C_GET_BV_01";
    public final String BUNDLED_PARAM_SNEP_TC_C_GET_BV_02   = "TC_C_GET_BV_02";
    public final String BUNDLED_PARAM_SNEP_TC_C_GET_BV_03   = "TC_C_GET_BV_03";

    public final String BUNDLED_PARAMS_SNEP_SERVER_KEY      = "snep_server"; // SNEP Server Related
    public final String BUNDLED_PARAMS_SNEP_SERVER_RUN      = "server_run";
    public final String BUNDLED_PARAMS_SNEP_SERVER_STOP     = "server_stop";
    public final String BUNDLED_PARAM_SNEP_OTHER_TEST_CASES = "Other_Test_Cases";
    public final String BUNDLED_PARAM_SNEP_TC_S_RET_BI_01   = "TC_S_RET_BI_01";

    public final String BUNDLED_PARAMS_START_KEY = "start";
    public final String BUNDLED_PARAMS_STOP_KEY = "stop";

    public final String DTA_INTENT_RESPONSE = "nxp_dta_response";
    public final String DTA_INTENT_DONE_RESPONSE = "done";

    /** Varaible to check and control Automa Test application is running or not
     */
    public boolean bIsInAutomationMode = false;

    /**
     * Load Library
     */

    static {
        try {
            if (PhUtilities.NDK_IMPLEMENTATION) {
                System.loadLibrary("dta_jni");

                Log.d(PhUtilities.UI_TAG, "Initializing System Load Library");
            } else {
                Log.d(PhUtilities.UI_TAG, "NDK is Disabled");
            }
        } catch (UnsatisfiedLinkError ule) {
            /**
             * UnstaisfiedLinkErrorhandled here
             */
            errorPopUP = true;
            errorMsg = "NO_LIBRARY_FILE";
            PhUtilities.NDK_IMPLEMENTATION = false;
            Log.d(PhUtilities.UI_TAG,
                    " Could not load native library: " + ule.getMessage());
        } catch (NoClassDefFoundError error) {
            /**
             * NoClassDefFoundError Path mismatch error
             */

            errorMsg = "PATH_MISMATCH";
            errorPopUP = true;
            PhUtilities.NDK_IMPLEMENTATION = false;
            Log.d(PhUtilities.UI_TAG, error.getMessage());
        } catch (NoSuchMethodError e) {
            /**
             * NoSuchMethodError Library not loaded correctly
             */
            Log.d(PhUtilities.UI_TAG, e.getMessage());
        }
    }

    /**
     * Loading the .so file from library
     */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /**
         * Loading the layout for displaying the user interface
         */
        setContentView(R.layout.ph_phdtaui_main_activity);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setFindViewByID();
        /*Disable Technologies not supported*/
        chkBoxListenP2PB.setEnabled(false);
        chkBoxPollP2PB.setEnabled(false);
        if(PhUtilities.DTA_DEBUG_MODE){
            nxpHelperMainActivity.setdtaDebugFlag(true);
            tsnRelativeLyt.setVisibility(View.VISIBLE);
            connDevRelativeLyt.setVisibility(View.VISIBLE);
        }else{
            tsnRelativeLyt.setVisibility(View.GONE);
            connDevRelativeLyt.setVisibility(View.GONE);
            nxpHelperMainActivity.setdtaDebugFlag(false);
            /*When view is invisible, initializing the variables with 00
             * to avoid Number format exception*/
            nxpHelperMainActivity.setsTimeSlotNumberF("00");
            nxpHelperMainActivity.setsConnectionDeviceLimit("00");
        }
        /**
         * Error Pop up start /
         */
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
        if (errorPopUP) {
            PhCustomLibDialog customLibDialog = new PhCustomLibDialog(
                    PhDTAUIMainActivity.this, errorMsg);
            customLibDialog.show();
        }

        PhUtilities.exitValue = "exit not clicked";
        /** START - get version number from manifest file */
        try {
            versionName = getPackageManager().getPackageInfo(getPackageName(),
                    0).versionName;
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        }
        // versionNumber.setText("UI Version: " + "05.05");
        // END - get version number from manifest file

        // START - enabling text box on check of file to log
        nfccheck();
        if (PhUtilities.NDK_IMPLEMENTATION && !(mNfcAdapter.isEnabled())) {
            clientConnectionProgress = ProgressDialog.show(PhDTAUIMainActivity.this, "",
                                                            "Loading Please Wait..", true, true);
            clientConnectionProgress.setCancelable(false);
            new Thread(){
                public void run() {
                    super.run();
                    try{
                       if(phNXPJniHelper.startPhDtaLibInit()==0){
                       Log.e(PhUtilities.UI_TAG, "DTA is Initializing");
                    }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                        clientConnectionProgress.dismiss();
                };
            }.start();
        }
        txtVwAppStatus.setText(Html.fromHtml(currentStatusStopped),
                TextView.BufferType.SPANNABLE);

        /**
         * Auto Manual Radio Group Check Change Listener
         */
        autoManualRadioGroup.setOnCheckedChangeListener(this);

        txtVwFwVer.setText(getResources().getString(
                R.string.firmare_version));

        /**
         * Enabling and Setting all the show message and custom message checked
         * is false in manual mode
         */
        selectionMode(false);
        customMessage.setChecked(false);
        showMessage.setChecked(false);
        editTxtLogFileName.setEnabled(true);

        /**
         * On Click listener for all the widget
         */
        onClickEventForAllTheViews();
        /**
         * check weather logToFileCheckBox is check or not checked
         */

        chkBoxLogToFile.setOnCheckedChangeListener(this);
        /** END - enabling text box on check of file to log */

        /** START - Display Copy Rights symbol */
        String copyRightsSymbol = "Copyright NXP Semiconductors";
        txtVwCopyRights.setText(copyRightsSymbol);
        /** END - Display Copy Rights symbol */

        chkBoxLogCat.setChecked(false);
        /** START - change intent to show Log Cat or Console */

        /** check weather logcatCheckBox is check or not checked */

        chkBoxLogCat.setOnCheckedChangeListener(this);

        /** END - change intent to show Log Cat or Console */

        editTextCustomPatternNum.setEnabled(true);
        spinnerPatterNum.setEnabled(true);
        certificationVersion.setEnabled(true);
        timeSlotNumberF.setEnabled(true);
        connectionDeviceLimit.setEnabled(true);
        spinnerPatterNum.setOnItemSelectedListener(PhDTAUIMainActivity.this);
        certificationVersion.setOnItemSelectedListener(PhDTAUIMainActivity.this);
        timeSlotNumberF.setOnItemSelectedListener(PhDTAUIMainActivity.this);
        connectionDeviceLimit.setOnItemSelectedListener(PhDTAUIMainActivity.this);
        adapterPatternNumberDefault = ArrayAdapter
                .createFromResource(PhDTAUIMainActivity.this,
                     R.array.multi_text_array,
                       android.R.layout.simple_spinner_item);
        adapterPatternNumberLlcp = ArrayAdapter
                .createFromResource(PhDTAUIMainActivity.this,
                     R.array.pattern_number_llcp,
                       android.R.layout.simple_spinner_item);
        adapterCleanPatternNumber = ArrayAdapter
                .createFromResource(PhDTAUIMainActivity.this, R.array.clean_pattern_numbers, android.R.layout.simple_spinner_item);
       adapterPatternNumberDefault.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
       adapterPatternNumberLlcp.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
       adapterCleanPatternNumber.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        /**
         * Set the adapter for the device spinner
         */

        runBtn.setOnClickListener(this);
        stopBtn.setOnClickListener(this);
        exitBtn.setOnClickListener(this);

        /**
         * Get pattern number from user
         */
        editTextCustomPatternNum.addTextChangedListener(new MaxLengthWatcher(4));
        /**
         *
         * APPEAR DISAPPEAR AND FADDING
         */
        /**
         * Poll and listen mode selection defaultly
         */

        /** Appear disappear for Custom */
        appearDissappearOFViews();
        handleAllTheModesEvent();

        /** Parse Intent on Create */
        this.onNewIntent(this.getIntent());
    }

    /**
     * Configuring DTA based on the inputs from AutomaTest
     */
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Bundle bundledParams = intent.getExtras();

        if (bundledParams != null) {

            //** Handle DP & Tag Operations commands received from AutomaTest */
            handleAutomaTestDPCommands(bundledParams);

            //** Handle LLCP commands received from AutomaTest */
            handleAutomaTestLLCPCommands(bundledParams);

            //** Handle LLCP commands received from AutomaTest */
            handleAutomaTestSNEPCommands(bundledParams);

            //** START & STOP button Implementaion for Automation */
            if (bundledParams.containsKey(BUNDLED_PARAMS_START_KEY)) {
              bIsInAutomationMode = true;
              Log.d(PhUtilities.UI_TAG, "Start DTA in ATM Mode");
              this.handleStartButtonEvent();
            }
            if (bundledParams.containsKey(BUNDLED_PARAMS_STOP_KEY)) {
              Log.d(PhUtilities.UI_TAG, "Stop DTA ATM Mode");
              this.handleStopButtonEvent();
            }

            Log.d(DTA_INTENT_RESPONSE, DTA_INTENT_DONE_RESPONSE);
        }
    }

    //** DP & Tag Operations Implementaion for Automation */
    private void handleAutomaTestDPCommands(Bundle bundledParams) {
        Log.d(PhUtilities.UI_TAG, "In handleAutomaTestDPCommands");

        if ( bundledParams.containsKey(BUNDLED_PARAMS_PATTERN_KEY) ) {
            Log.d("Received Pattern Number", bundledParams.getString(BUNDLED_PARAMS_PATTERN_KEY) );

            String DpPatternStr = bundledParams.getString(BUNDLED_PARAMS_PATTERN_KEY);

            try {
                Integer DpPatternInt = Integer.parseInt(DpPatternStr, 16);
                if ( DpPatternInt>=0 && DpPatternInt <= 15) {
                    Log.d(PhUtilities.UI_TAG, "DP Pattern number set");
                    this.spinnerPatterNum.setSelection(DpPatternInt);
                }
                else {
                    Log.d(PhUtilities.UI_TAG, "DP default Pattern number set");
                    this.spinnerPatterNum.setSelection(0);
                }
            }
            catch (NumberFormatException e) {
                Log.d(PhUtilities.UI_TAG, "Number format exception occured during conversion");
            }
        }

        /* Connection Device Limit */
        if ( (bundledParams.containsKey(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY)) &&
             ( (Integer.valueOf(bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY)) < 0) &&
               (Integer.valueOf(bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY)) >= 5) ) ) {
            Log.d("Connection Devices Limit is <0 or >= 5, so setting to 01",
                    bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY));
            this.connectionDeviceLimit.setSelection(1);
        }
        else if (bundledParams.containsKey(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY)) {
            this.connectionDeviceLimit.setSelection(Integer.valueOf(bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT_KEY)));
        }

        /* Certificaiton Release */
        if ( (bundledParams.containsKey(BUNDLED_PARAMS_CERT_REL_KEY)) &&
                  (Integer.valueOf(bundledParams.getString(BUNDLED_PARAMS_CERT_REL_KEY)) >= 13)) {
            Log.d("Certificaiton Release is >= 13, so setting to CR8", bundledParams.getString(BUNDLED_PARAMS_CERT_REL_KEY));
            this.certificationVersion.setSelection(0);
        }
        else if (bundledParams.containsKey(BUNDLED_PARAMS_CERT_REL_KEY)) {
            this.certificationVersion.setSelection(Integer.valueOf(bundledParams.getString(BUNDLED_PARAMS_CERT_REL_KEY)) - CERT_REL_MIN_VALUE);
        }

        /* Listen P2P Mode */
        if(bundledParams.containsKey(BUNDLED_PARAMS_LIS_P2P_MODE_KEY)) {
            Log.d(PhUtilities.UI_TAG, "Listen P2P Command Received");
            switch (bundledParams.getString(BUNDLED_PARAMS_LIS_P2P_MODE_KEY)) {
                case BUNDLED_PARAMS_LIS_P2P_ENABLE_VALUE:
                    this.chkBoxListenP2p.setChecked(true);
                    if(this.chkBoxListenHCEF.isChecked()) {
                        this.chkBoxListenHCEF.setChecked(false);
                        this.handleListenHceFTechCheckBoxEvent();
                        Toast.makeText(PhDTAUIMainActivity.this,
                              "HCE-F is disabled, when P2P Listen is selected",
                              Toast.LENGTH_SHORT).show();
                    }
                    break;
                case BUNDLED_PARAMS_LIS_P2P_DISABLE_VALUE:
                    this.chkBoxListenP2p.setChecked(false);
                    break;
            }
            this.handleListenP2PCheckBoxEvent();
        }

        /* HCE Mode */
        if (bundledParams.containsKey(BUNDLED_PARAMS_HCE_MODE_KEY)) {
            Log.d(PhUtilities.UI_TAG, "HCE Mode Command Received");
            switch (bundledParams.getString(BUNDLED_PARAMS_HCE_MODE_KEY)) {
                case BUNDLED_PARAMS_HCE_MODE_NONE_VALUE:
                    this.chkBoxListenHce.setChecked(false);
                    this.handleListenHCECheckBoxEvent();
                    break;
                case BUNDLED_PARAMS_HCE_MODE_AB_VALUE:
                    this.chkBoxListenHce.setChecked(true);
                    this.handleListenHCECheckBoxEvent();
                    break;
                case BUNDLED_PARAMS_HCE_MODE_F_VALUE:
                    this.chkBoxListenHce.setChecked(true);
                    this.handleListenHCECheckBoxEvent();
                    this.chkBoxListenHCEF.setChecked(true);
                    this.handleListenHceFTechCheckBoxEvent();
                    break;
                default:
                    this.chkBoxListenHce.setChecked(false);
                    this.handleListenHCECheckBoxEvent();
                    break;
            }
        }

        /* UICC Mode */
        if (bundledParams.containsKey(BUNDLED_PARAMS_UICC_MODE_KEY)) {
            Log.d(PhUtilities.UI_TAG, "UICC Mode Command Received");
            switch (bundledParams.getString(BUNDLED_PARAMS_UICC_MODE_KEY)) {
                case BUNDLED_PARAMS_UICC_ENABLE_VALUE:
                    this.chkBoxListenUicc.setChecked(true);
                    this.handleListenUICCCheckBoxEvent();
                    break;
                case BUNDLED_PARAMS_UICC_DISABLE_VALUE:
                    this.chkBoxListenUicc.setChecked(false);
                    this.handleListenUICCCheckBoxEvent();
                    break;
                default:
                    this.chkBoxListenUicc.setChecked(false);
                    this.handleListenUICCCheckBoxEvent();
                    break;
            }
        }

        /* ESE Mode */
        if (bundledParams.containsKey(BUNDLED_PARAMS_ESE_MODE_KEY)) {
            Log.d(PhUtilities.UI_TAG, "ESE Mode Command Received");
            switch (bundledParams.getString(BUNDLED_PARAMS_ESE_MODE_KEY)) {
                case BUNDLED_PARAMS_ESE_ENABLE_VALUE:
                    this.chkBoxListenEse.setChecked(true);
                    this.handleListenESECheckBoxEvent();
                    break;
                case BUNDLED_PARAMS_ESE_DISABLE_VALUE:
                    this.chkBoxListenEse.setChecked(false);
                    this.handleListenESECheckBoxEvent();
                    break;
                default:
                    this.chkBoxListenEse.setChecked(false);
                    this.handleListenHCECheckBoxEvent();
                    break;
            }
        }

        /* Time Slot Number */
        if (bundledParams.containsKey(BUNDLED_PARAMS_TSNF_KEY)) {
            switch (bundledParams.getString(BUNDLED_PARAMS_TSNF_KEY)){
                case BUNDLED_PARAMS_TSNF_03_VALUE:
                    this.timeSlotNumberF.setSelection(0);
                    break;
                case BUNDLED_PARAMS_TSNF_F3_VALUE:
                    this.timeSlotNumberF.setSelection(1);
                    break;
                default:
                    this.timeSlotNumberF.setSelection(0);
                    break;
            }
        }
    }

    //** LLCP Implementaion for Automation */
    private void handleAutomaTestLLCPCommands(Bundle bundledParams) {
        Log.d(PhUtilities.UI_TAG, "In handleAutomaTestLLCPCommands");

        if (bundledParams.containsKey(BUNDLED_PARAMS_LLCP_KEY)) {
            switch (bundledParams.getString(BUNDLED_PARAMS_LLCP_KEY)) {
                case BUNDLED_PARAMS_LLCP_ENABLE_VALUE:
                    this.chkBoxLlcp.setChecked(true);
                    this.handleLLCPCheckBoxEvent();
                    break;
                case BUNDLED_PARAMS_LLCP_DISABLE_VALUE:
                    this.chkBoxLlcp.setChecked(false);
                    this.handleLLCPCheckBoxEvent();
                    break;
            }
        }

        if (bundledParams.containsKey(BUNDLED_PARAMS_LLCP_CONN_PDU_PARAMS_KEY)) {
            switch (bundledParams.getString(BUNDLED_PARAMS_LLCP_CONN_PDU_PARAMS_KEY)) {
                case BUNDLED_PARAMS_LLCP_ENABLE_CONN_PDU_PARAMS_VALUE:
                    this.chkBoxLlcpConnectPduPrms.setChecked(true);
                    break;
                case BUNDLED_PARAMS_LLCP_DISABLE_CONN_PDU_PARAMS_VALUE:
                    this.chkBoxLlcpConnectPduPrms.setChecked(false);
                    break;
            }
        }

        if (bundledParams.containsKey(BUNDLED_PARAMS_LLCP_PATTERN_KEY)) {
            switch (bundledParams.getString(BUNDLED_PARAMS_LLCP_PATTERN_KEY)) {
                case BUNDLED_PARAMS_LLCP_PATTERN_1200:
                    this.spinnerPatterNum.setSelection(0);
                    break;
                case BUNDLED_PARAMS_LLCP_PATTERN_1240:
                    this.spinnerPatterNum.setSelection(1);
                    break;
                case BUNDLED_PARAMS_LLCP_PATTERN_1280:
                    this.spinnerPatterNum.setSelection(2);
                    break;
                default:
                    this.spinnerPatterNum.setSelection(0);
                    break;
            }
        }
    }

    //** SNEP Implementaion for Automation */
    private void handleAutomaTestSNEPCommands(Bundle bundledParams) {
        Log.d(PhUtilities.UI_TAG, "In handleAutomaTestSNEPCommands");

        if (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_KEY)) {
            switch (bundledParams.getString(BUNDLED_PARAMS_SNEP_KEY)) {
                case BUNDLED_PARAMS_SNEP_ENABLE_VALUE:
                    this.chkBoxSnep.setChecked(true);
                    this.handleSNEPCheckBoxEvent();
                    Log.d("SNEP_ENABLE", "Received Enable SNEP Command");
                    break;
                case BUNDLED_PARAMS_SNEP_DISABLE_VALUE:
                    this.chkBoxSnep.setChecked(false);
                    Log.d("SNEP_DISABLE", "Received disable SNEP command ....");
                    if(null != phCustomSNEPRTD){
                        Log.d("SNEP_DISABLE","Received command to disable SNEP functionality");
                        phCustomSNEPRTD.handleBackBtnSnep();
                        Log.d("SNEP_DISABLE", "Completed closing SNEP dialog");
                    }
                    this.handleSNEPCheckBoxEvent();
                    break;
            }
        }

        if (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_CLIENT_KEY) &&
                (null != phCustomSNEPRTD)) {

            Log.d("BUNDLED_PARAMS_SNEP_CLIENT_KEY value is",
                    bundledParams.getString(BUNDLED_PARAMS_SNEP_CLIENT_KEY));

            switch (bundledParams.getString(BUNDLED_PARAMS_SNEP_CLIENT_KEY)) {
                case BUNDLED_PARAM_SNEP_TC_C_BIT_BV_01:
                    phCustomSNEPRTD.setTestCaseID(1);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_BIT_BI_01_0:
                    phCustomSNEPRTD.setTestCaseID(2);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_BIT_BI_01_1:
                    phCustomSNEPRTD.setTestCaseID(3);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_PUT_BV_01:
                    phCustomSNEPRTD.setTestCaseID(4);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_PUT_BV_02:
                    phCustomSNEPRTD.setTestCaseID(5);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_PUT_BI_01:
                    phCustomSNEPRTD.setTestCaseID(6);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_GET_BV_01:
                    phCustomSNEPRTD.setTestCaseID(7);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_GET_BV_02:
                    phCustomSNEPRTD.setTestCaseID(8);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_C_GET_BV_03:
                    phCustomSNEPRTD.setTestCaseID(9);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
                default:
                    //phCustomSNEPRTD.setTestCaseID(0);
                    phCustomSNEPRTD.clientTestCaseIdSelection();
                    break;
            }
            phCustomSNEPRTD.AUTOMATEST_FLAG = false;
        }
        //** RUN & STOP Implementaion for Automation for SNEP Client */
        if ( (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_CLIENT_KEY)) &&
                (bundledParams.getString(BUNDLED_PARAMS_SNEP_CLIENT_KEY).equals(
                        BUNDLED_PARAMS_SNEP_CLIENT_RUN)) ) {
            Log.d(PhUtilities.UI_TAG, "Received SNEP Client RUN command from AutomaTest");
            phCustomSNEPRTD.handleSnepRunClient();
        }
        else if ( (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_CLIENT_KEY)) &&
                (bundledParams.getString(BUNDLED_PARAMS_SNEP_CLIENT_KEY).equals(
                        BUNDLED_PARAMS_SNEP_CLIENT_STOP)) ) {
            Log.d(PhUtilities.UI_TAG, "Received SNEP Client STOP command from AutomaTest");
            phCustomSNEPRTD.handleSnepStopClient();
        }

        //** SNEP Server Related Configurations */
        if (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_SERVER_KEY) &&
                (null != phCustomSNEPRTD)) {
            Log.d("BUNDLED_PARAMS_SNEP_SERVER_KEY value is",
                    bundledParams.getString(BUNDLED_PARAMS_SNEP_SERVER_KEY));

            switch (bundledParams.getString(BUNDLED_PARAMS_SNEP_SERVER_KEY)) {
                case BUNDLED_PARAM_SNEP_OTHER_TEST_CASES:
                    phCustomSNEPRTD.setTestCaseID(1);
                    phCustomSNEPRTD.serverTestCaseIdSelection();
                    break;
                case BUNDLED_PARAM_SNEP_TC_S_RET_BI_01:
                    phCustomSNEPRTD.setTestCaseID(2);
                    phCustomSNEPRTD.serverTestCaseIdSelection();
                    break;
                default:
                    //phCustomSNEPRTD.setTestCaseID(0);
                    phCustomSNEPRTD.serverTestCaseIdSelection();
                    break;
            }
            phCustomSNEPRTD.AUTOMATEST_FLAG = false;
        }
        //** RUN & STOP Implementaion for Automation for SNEP Server */
        if ( (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_SERVER_KEY)) &&
                (bundledParams.getString(BUNDLED_PARAMS_SNEP_SERVER_KEY).equals(
                        BUNDLED_PARAMS_SNEP_SERVER_RUN)) ) {
            Log.d(PhUtilities.UI_TAG, "Received SNEP Server RUN command from AutomaTest");
            phCustomSNEPRTD.handleSnepRunServer();
        }
        else if ( (bundledParams.containsKey(BUNDLED_PARAMS_SNEP_SERVER_KEY)) &&
                (bundledParams.getString(BUNDLED_PARAMS_SNEP_SERVER_KEY).equals(
                        BUNDLED_PARAMS_SNEP_SERVER_STOP)) ) {
            Log.d(PhUtilities.UI_TAG, "Received SNEP Server STOP command from AutomaTest");
            phCustomSNEPRTD.handleSnepStopServer();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        editTxtLogFileName.setFocusable(false);
        if (PhUtilities.exitValue.equals("exit clicked")) {
            Toast.makeText(getApplicationContext(), PhUtilities.exitValue,
                    Toast.LENGTH_SHORT).show();
            finish();
        }

    }

    /** Onclick actions for all the widget */

    @Override
    public void onClick(View v) {

        switch (v.getId()) {
        /** Run button onclick actions for all the widget */

        case R.id.run:
            handleStartButtonEvent();
            break;
        /** Stop button onclick actions for all the widget */
        case R.id.stop:
            handleStopButtonEvent();
            break;
        /** Exit button onclick actions for all the widget */
        case R.id.exit_btn:
            handleExitButtonEvent();
            break;
        /** Create the log file with user entered name function here */
        case R.id.logtofiletext:
            handleLogtoFileCheckBoxEvent();
            break;

        case R.id.logtofile:
            break;

        /** Show the Custom alert dialog for Custom Message */
        case R.id.custom_msg:
            handleAutoModeCustomCheckMessageCheckBoxEvent();
            break;

        /** Show the Custom alert dialog for Show Message */
        case R.id.show_msg:
            handleAutoModeShowMessageCheckBoxEvent();
            break;

        /*** version number update if info check box is enabled */
        case R.id.versionnumber:
            handleVersionNumberCheckBoxEvent();
            break;

        case R.id.llcp_check_box:
            handleLLCPCheckBoxEvent();
            break;

        case R.id.snep_check_box:
            handleSNEPCheckBoxEvent();
            break;

        case R.id.p2p_poll_check_box:
            handlePollP2PCheckBoxEvent();
            break;

        case R.id.rd_wt_poll_check_box:
            handlePollRDWTCheckBoxEvent();
            break;

        case R.id.p2p_listen_check_box:
            handleListenP2PCheckBoxEvent();
            break;

        case R.id.uicc_listen_check_box:
            handleListenUICCCheckBoxEvent();
            break;

        case R.id.hce_listen_check_box:
            handleListenHCECheckBoxEvent();
            break;

        case R.id.ese_listen_check_box:
            handleListenESECheckBoxEvent();
            break;

        case R.id.a_p2p_poll_check_box:
            if(chkBoxPollP2PA.isChecked())
                chkBoxPollRDWTA.setChecked(true);
            else
                chkBoxPollRDWTA.setChecked(false);
        case R.id.b_p2p_poll_check_box:
        case R.id.f_p2p_poll_check_box:
            if(chkBoxPollP2PF.isChecked())
                chkBoxPollRDWTF.setChecked(true);
            else
                chkBoxPollRDWTF.setChecked(false);
            handlePollP2pTechCheckBoxEvent();
            break;

        case R.id.a_rdwt_poll_check_box:
            if(chkBoxPollRDWTA.isChecked())
                chkBoxPollP2PA.setChecked(true);
            else
                chkBoxPollP2PA.setChecked(false);
        case R.id.b_rdwt_poll_check_box:
        case R.id.f_rdwt_poll_check_box:
            if(chkBoxPollRDWTF.isChecked())
                chkBoxPollP2PF.setChecked(true);
            else
                chkBoxPollP2PF.setChecked(false);
            handlePollRdWrtTechCheckBoxEvent();
            break;

        case R.id.a_p2p_listen_check_box:
        case R.id.b_p2p_listen_check_box:
        case R.id.f_p2p_listen_check_box:
            handleListenP2pTechCheckBoxEvent();
            break;

        case R.id.a_uicc_listen_check_box:
        case R.id.b_uicc_listen_check_box:
        case R.id.f_uicc_listen_check_box:
            handleListenUiccTechCheckBoxEvent();
            break;

        case R.id.a_hce_listen_check_box:
            handleListenHceATechCheckBoxEvent();
            break;
        case R.id.b_hce_listen_check_box:
            handleListenHceBTechCheckBoxEvent();
            break;
        case R.id.f_hce_listen_check_box:
            handleListenHceFTechCheckBoxEvent();
            break;

        case R.id.a_ese_listen_check_box:
        case R.id.b_ese_listen_check_box:
        case R.id.f_ese_listen_check_box:
            handleListenEseTechCheckBoxEvent();
            break;

        }

    }

    private void technologyError(String message) {
        AlertDialog.Builder alertDialogBuilder=new AlertDialog.Builder(this);
        alertDialogBuilder.setMessage(message);
        alertDialogBuilder.setPositiveButton(getResources().getString(R.string.ok),new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface arg0, int arg1) {
            }
        });
        alertDialogBuilder.show();
    }
    private void checkForAnalogTestMode(){
        if(editTextCustomPatternNum.getText().toString().equals("1000")){
            /*In Analog mode Enable Poll[A,B,F(212&424)] & Listen[F-P2P & HCEType A&B]*/
            Log.i(PhUtilities.UI_TAG,"Enabling configurations for Analog mode");
            chkBoxPollP2p.setChecked(true);
            chkBoxPollRdWt.setChecked(true);
            chkBoxListenP2p.setChecked(true);
            chkBoxListenHce.setChecked(true);
            chkBoxListenUicc.setChecked(false);
            chkBoxListenEse.setChecked(false);

            chkBoxPollP2PA.setChecked(true);
            chkBoxPollP2PB.setChecked(false);
            chkBoxPollP2PF.setChecked(true);

            chkBoxPollRDWTA.setChecked(true);
            chkBoxPollRDWTB.setChecked(true);
            chkBoxPollRDWTF.setChecked(true);

            chkBoxListenP2PA.setChecked(false);
            chkBoxListenP2PB.setChecked(false);
            chkBoxListenP2PF.setChecked(true);

            chkBoxListenHCEA.setChecked(true);
            chkBoxListenHCEB.setChecked(true);
            chkBoxListenHCEF.setChecked(false);
            chkBoxListenHCEA.setEnabled(true);
            chkBoxListenHCEB.setEnabled(true);
            chkBoxListenHCEF.setEnabled(true);

            chkBoxListenUICCA.setChecked(false);
            chkBoxListenUICCB.setChecked(false);
            chkBoxListenUICCF.setChecked(false);

            chkBoxListenESEA.setChecked(false);
            chkBoxListenESEB.setChecked(false);
            chkBoxListenESEF.setChecked(false);
        }
    }

    private void onClickEventForAllTheViews() {
        customMessage.setOnClickListener(this);
        showMessage.setOnClickListener(this);
        chkBoxLlcp.setOnClickListener(this);
        chkBoxSnep.setOnClickListener(this);
        chkBoxInfo.setOnClickListener(this);
        chkBoxLogToFile.setOnClickListener(this);
        editTxtLogFileName.setOnClickListener(this);
        chkBoxPollP2p.setOnClickListener(this);
        chkBoxPollRdWt.setOnClickListener(this);
        chkBoxListenP2p.setOnClickListener(this);
        chkBoxListenUicc.setOnClickListener(this);
        chkBoxListenHce.setOnClickListener(this);
        chkBoxListenEse.setOnClickListener(this);
        chkBoxPollP2PA.setOnClickListener(this);
        chkBoxPollP2PB.setOnClickListener(this);
        chkBoxPollP2PF.setOnClickListener(this);
        chkBoxPollRDWTA.setOnClickListener(this);
        chkBoxPollRDWTB.setOnClickListener(this);
        chkBoxPollRDWTF.setOnClickListener(this);
        chkBoxListenP2PA.setOnClickListener(this);
        chkBoxListenP2PB.setOnClickListener(this);
        chkBoxListenP2PF.setOnClickListener(this);
        chkBoxListenUICCA.setOnClickListener(this);
        chkBoxListenUICCB.setOnClickListener(this);
        chkBoxListenUICCF.setOnClickListener(this);
        chkBoxListenHCEA.setOnClickListener(this);
        chkBoxListenHCEB.setOnClickListener(this);
        chkBoxListenHCEF.setOnClickListener(this);
        chkBoxListenESEA.setOnClickListener(this);
        chkBoxListenESEB.setOnClickListener(this);
        chkBoxListenESEF.setOnClickListener(this);

    }

    private void enableDiscovery() {
        txtVwAppStatus.setText(Html.fromHtml(currentStatusInitializing),TextView.BufferType.SPANNABLE);
        if (PhUtilities.NDK_IMPLEMENTATION && !mNfcAdapter.isEnabled()) {
            // 9. Start Execution of dta start task
            if((clientConnectionProgress==null) || !(clientConnectionProgress.isShowing())){
            clientConnectionProgress = ProgressDialog.show(PhDTAUIMainActivity.this, "", "Loading Please Wait..", true, true);
            clientConnectionProgress.setCancelable(false);
            }
            new Thread(){
                @Override
                public void run() {
                    super.run();
                    try {
                        if(!mNfcAdapter.isEnabled()){
                            phNXPJniHelper.startTest();
                            if(phNXPJniHelper.startPhDtaLibInit()==0){
                            Log.e(PhUtilities.UI_TAG, "DTA is Initializing");
                            }else{
                            Log.e(PhUtilities.UI_TAG, "Already DTA is Initialized");
                            }
                            getUpdatesFromView();
                            phNXPJniHelper.enableDiscovery(phDtaLibStructureObj);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    clientConnectionProgress.dismiss();
                }

            }.start();
            txtVwAppStatus.setText(Html.fromHtml(currentStatusRunning),TextView.BufferType.SPANNABLE);
        }
    }

    private void automodeOnRunButtonPressed() {
        Log.d(PhUtilities.UI_TAG, "Looping into Auto Mode");
        editTextCustomPatternNum.setHint("Custom");
        editTextCustomPatternNum.setEnabled(false);
        spinnerPatterNum.setEnabled(false);
        certificationVersion.setEnabled(false);
        timeSlotNumberF.setEnabled(false);
        connectionDeviceLimit.setEnabled(false);
        if (PhUtilities.CLIENT_SELECTED) {
            if (PhUtilities.SIMULATED_SERVER_SELECTED) {
                Log.d(PhUtilities.TCPSRV_TAG,
                        "Starting TCP/IP Server Simulator. On RUN button click.");

                new PhNioServerTask(this,portNumber)
                        .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
                // Utilities.SIMULATED_SERVER_SELECTED = false;
            } else {
                Log.d(PhUtilities.TCPCNT_TAG,
                        "Starting TCP/IP Client. On RUN button click.");
                if(inetAddress != null){
                    Log.d(PhUtilities.TCPCNT_TAG, "IPaddress captured from UI : " + inetAddress.getHostAddress());
                    Log.d(PhUtilities.TCPCNT_TAG, "Port Number captured from UI : " + portNumber);
                }
                new PhNioClientTask(this,inetAddress, portNumber)
                        .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
                // Utilities.CLIENT_SELECTED = false;
            }

        } else if (PhUtilities.SERVER_SELECTED) {
             Log.d(PhUtilities.TCPSRV_TAG,
                     "Starting TCP/IP Server. On RUN button click.");

             new PhNioServerTask(this,portNumber)
                     .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        }

        try {
            TimeUnit.MILLISECONDS.sleep(500);
        } catch (InterruptedException e1) {
            e1.printStackTrace();
        }

        sPatternNumber = PhUtilities.patternNUmberBuff == null ? "0" : PhUtilities.patternNUmberBuff;
    }

    /** Enable all the buttons */

    public void manualModeisON() {
        chkBoxLlcpConnectPduPrms.setEnabled(false);
        chkBoxPollP2p.setEnabled(true);
        chkBoxPollRdWt.setEnabled(true);
        chkBoxListenP2p.setEnabled(true);
        chkBoxListenUicc.setEnabled(true);
        chkBoxListenHce.setEnabled(true);
        chkBoxListenEse.setEnabled(true);

        chkBoxListenUicc.setChecked(false);
        chkBoxListenHce.setChecked(false);
        chkBoxListenEse.setChecked(false);

        chkBoxLlcp.setEnabled(true);
        chkBoxSnep.setEnabled(true);
        editTextCustomPatternNum.setEnabled(true);
        spinnerPatterNum.setEnabled(true);
        certificationVersion.setEnabled(true);
        timeSlotNumberF.setEnabled(true);
        connectionDeviceLimit.setEnabled(true);

        radioBtnAutoMode.setEnabled(true);
        radioBtnManualMode.setEnabled(true);
        spinnerPatterNum.setEnabled(true);
        chkBoxLogCat.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        editTxtLogFileName.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        handleAllTheModesEvent();
    }

    public void handleStopBtnInManualMode(){
        chkBoxLlcpConnectPduPrms.setEnabled(false);
        chkBoxPollP2p.setEnabled(true);
        chkBoxPollRdWt.setEnabled(true);
        chkBoxListenP2p.setEnabled(true);
        chkBoxListenUicc.setEnabled(true);
        chkBoxListenHce.setEnabled(true);
        chkBoxListenEse.setEnabled(true);
        chkBoxLlcp.setEnabled(true);
        chkBoxSnep.setEnabled(true);
        editTextCustomPatternNum.setEnabled(true);
        spinnerPatterNum.setEnabled(true);
        certificationVersion.setEnabled(true);
        timeSlotNumberF.setEnabled(true);
        connectionDeviceLimit.setEnabled(true);
        radioBtnAutoMode.setEnabled(true);
        radioBtnManualMode.setEnabled(true);
        spinnerPatterNum.setEnabled(true);
        chkBoxLogCat.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        editTxtLogFileName.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        enableBtnsWhichAreTicked();
        enableTechChkBoxsForModes();
    }

    /** Disable all the buttons */
    public void autoModeisON() {
        chkBoxLlcpConnectPduPrms.setEnabled(false);
        chkBoxLlcp.setEnabled(false);
        chkBoxSnep.setEnabled(false);
        chkBoxLlcp.setChecked(false);
        chkBoxSnep.setChecked(false);
        chkBoxLogToFile.setEnabled(false);
        spinnerPatterNum.setEnabled(false);
        certificationVersion.setEnabled(false);
        timeSlotNumberF.setEnabled(false);
        connectionDeviceLimit.setEnabled(false);
             /** enable mode */
        radioBtnAutoMode.setEnabled(true);
        radioBtnManualMode.setEnabled(true);
        radioBtnAutoMode.setEnabled(true);
        radioBtnManualMode.setEnabled(true);
        editTextCustomPatternNum.setEnabled(false);
        editTxtLogFileName.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxLogCat.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);

        chkBoxPollP2p.setEnabled(false);
        chkBoxPollRdWt.setEnabled(false);
        chkBoxListenUicc.setEnabled(false);
        chkBoxListenHce.setEnabled(false);
        chkBoxListenEse.setEnabled(false);

        chkBoxPollP2PA.setEnabled(false);
        chkBoxPollP2PF.setEnabled(false);

        chkBoxPollRDWTA.setEnabled(false);
        chkBoxPollRDWTB.setEnabled(false);
        chkBoxPollRDWTF.setEnabled(false);

        chkBoxListenP2PA.setEnabled(false);
        chkBoxListenP2PF.setEnabled(false);

        chkBoxListenUICCA.setEnabled(false);
        chkBoxListenUICCB.setEnabled(false);
        chkBoxListenUICCF.setEnabled(false);


        chkBoxListenHCEA.setEnabled(false);
        chkBoxListenHCEB.setEnabled(false);
        chkBoxListenHCEF.setEnabled(false);

        chkBoxListenESEA.setEnabled(false);
        chkBoxListenESEB.setEnabled(false);
        chkBoxListenESEF.setEnabled(false);
    }

    /** Disable all the buttons */

    public void disableAllOtherBtns() {
        chkBoxLlcp.setEnabled(false);
        chkBoxSnep.setEnabled(false);
        editTextCustomPatternNum.setEnabled(false);
        chkBoxLogCat.setFocusable(false);
        spinnerPatterNum.setEnabled(false);
        certificationVersion.setEnabled(false);
        timeSlotNumberF.setEnabled(false);
        connectionDeviceLimit.setEnabled(false);
        radioBtnAutoMode.setEnabled(false);
        radioBtnManualMode.setEnabled(false);
        chkBoxLogToFile.setEnabled(false);
        chkBoxLogCat.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        editTxtLogFileName.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);

        chkBoxPollP2p.setEnabled(false);
        chkBoxPollRdWt.setEnabled(false);
        chkBoxListenP2p.setEnabled(false);
        chkBoxListenUicc.setEnabled(false);
        chkBoxListenHce.setEnabled(false);
        chkBoxListenEse.setEnabled(false);

        chkBoxPollP2PA.setEnabled(false);
        chkBoxPollP2PF.setEnabled(false);

        chkBoxPollRDWTA.setEnabled(false);
        chkBoxPollRDWTB.setEnabled(false);
        chkBoxPollRDWTF.setEnabled(false);

        chkBoxListenP2PA.setEnabled(false);
        chkBoxListenP2PF.setEnabled(false);

        chkBoxListenUICCA.setEnabled(false);
        chkBoxListenUICCB.setEnabled(false);
        chkBoxListenUICCF.setEnabled(false);


        chkBoxListenHCEA.setEnabled(false);
        chkBoxListenHCEB.setEnabled(false);
        chkBoxListenHCEF.setEnabled(false);

        chkBoxListenESEA.setEnabled(false);
        chkBoxListenESEB.setEnabled(false);
        chkBoxListenESEF.setEnabled(false);

        chkBoxLlcpConnectPduPrms.setEnabled(false);

    }

    /** Enable the Custom Message and Show Message from user actions */
    public void selectionMode(boolean selectionMode) {
        customMessage.setEnabled(selectionMode);
        showMessage.setEnabled(selectionMode);
        txtVwAppStatus.setEnabled(selectionMode);

    }

    /**
     * On Hard ware Back Key is Pressed handled here
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            PhCustomExitDialog customExitDialog = new PhCustomExitDialog(this, "Are you sure you want to exit?", true);
            customExitDialog.show();
        }

        return super.onKeyDown(keyCode, event);
    }

    public static int DTA_STARTED = 1;
    public static int DTA_STOP = 2;

    /**
     * When user press the Stop Button and Closing all the process
     */
    private void disableDiscovery() {
        try {
            if(PhUtilities.NDK_IMPLEMENTATION){
            Log.v(PhUtilities.UI_TAG, "phDtaLibDisableDiscovery");
            phNXPJniHelper.phDtaLibDisableDiscovery();
            }
        } catch (NullPointerException e) {
            e.printStackTrace();
        } catch (ActivityNotFoundException e) {
        }

    }

    /**
     * Checking device contains NFC Tag and it is enabled or not
     */
    private void nfccheck() {
        boolean nfcEnable = false;
        nfcEnable = mNfcAdapter.isEnabled();
        // Pop AlertDialog to Reminder Start NFC
        if((phCustomSNEPRTD != null) && nfcEnable){
            Log.v(PhUtilities.UI_TAG,"Nothing to do");
        }else{
        final Context ctx = this;
        AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
        if (nfcEnable == true) {
            Log.d(PhUtilities.UI_TAG, "NFC ON");
            builder.setMessage(" Turn OFF NFC Service in DTA mode \n Wait for 5 seconds and Run DTA")
            // .setNegativeButton("Cancel", null)
                    .setPositiveButton("Enter",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    try {
                                        dialog.cancel();
                                        ctx.startActivity(new Intent(Settings.ACTION_NFC_SETTINGS));
                                        checkNfcServiceDialog = true;
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                            });
            builder.setCancelable(false);
            builder.show();
        }
        }
    }

    /**
     * Checking Auto Mode or Manual Mode is enabled
     */
    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {

        switch (group.getCheckedRadioButtonId()) {
        case R.id.auto_radio_button:
            editTextCustomPatternNum.setEnabled(false);
            editTextCustomPatternNum.setText("");
            autoModeisON();
            /**
             * Auto Mode is Enable Opening the Client Server Communication
             */
            if (radioBtnAutoMode.isChecked()) {

                PhCustomIPDialog customIPDialog = new PhCustomIPDialog(this);
                customIPDialog.show();
                radioBtnAutoMode.setEnabled(false);
                radioBtnManualMode.setEnabled(true);
                spinnerPatterNum.setEnabled(false);
                certificationVersion.setEnabled(false);
                timeSlotNumberF.setEnabled(false);
                connectionDeviceLimit.setEnabled(false);
                selectionMode(true);
            }
            break;

        case R.id.manual_radio_button:
            manualModeisON();
            /**
             * Manual Mode is Enable
             */
            if (radioBtnManualMode.isChecked()) {
                radioBtnManualMode.setEnabled(false);
                radioBtnAutoMode.setEnabled(true);
                selectionMode(false);
                customMessage.setChecked(false);
                showMessage.setChecked(false);
            }
            break;

    }
}

    /**
     * Edit Text validation For Custom Pattern Number
     */

    public class MaxLengthWatcher implements TextWatcher {

        private int maxLen = 0;

        public MaxLengthWatcher(int maxLen) {
            this.maxLen = maxLen;
        }

        public void afterTextChanged(Editable arg0) {

        }

        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
                int arg3) {

        }

        public void onTextChanged(CharSequence chars, int start, int before,
                int count) {
            Editable editable = editTextCustomPatternNum.getText();
            int len = editable.length();
            String checkValidite = null;
            checkValidite = chars.toString();
            Log.d(PhUtilities.UI_TAG, "Get pattern Number checkValidite "
                    + checkValidite);

            Log.d(PhUtilities.UI_TAG, "Get pattern Number length "
                    + checkValidite.length());
            if ((checkValidite.length() > 4)
                    && (checkValidite.matches(("[a-fA-F0-9]+")))) {
                Log.d(PhUtilities.UI_TAG,
                        "" + (checkValidite.matches(("[a-fA-F0-9]+"))));
                customPopValidation = new PhCustomPopValidation(
                        PhDTAUIMainActivity.this, getResources().getString(
                                R.string.warning));
                customPopValidation.show();
                Log.d(PhUtilities.UI_TAG,
                        "Entered the correct pattern number but it excedes 4 characters");
            } else if ((checkValidite.matches(("[a-fA-F0-9]+")))) {
                Log.d(PhUtilities.UI_TAG,
                        "" + (checkValidite.matches(("[a-fA-F0-9]+"))));

                Log.d(PhUtilities.UI_TAG, "Entered the correct pattern number");
            } else if ((checkValidite.length() != 0)) {
                customPopValidation = new PhCustomPopValidation(
                        PhDTAUIMainActivity.this, getResources().getString(
                                R.string.warning_for_wrong_key));
                Log.d(PhUtilities.UI_TAG,
                        "extractText"
                                + checkValidite.substring(0,
                                        checkValidite.length() - 1));
                editTextCustomPatternNum.setText(checkValidite.substring(0,
                        checkValidite.length() - 1));
                customPopValidation.show();
                Log.d(PhUtilities.UI_TAG, "Entered the wrong pattern number");
            } else {
                Log.d(PhUtilities.UI_TAG,
                        "Entered the else wrong pattern number");
            }

            checkForAnalogTestMode();
            /**
             *
             * Spinner implementation
             */
            if (editTextCustomPatternNum.getText().toString().length() > 0) {
                Log.d(PhUtilities.UI_TAG,
                        "Show pop user enter more than 4 characters");
                spinnerPatterNum.setEnabled(false);
                spinnerPatterNum.setAdapter(adapterCleanPatternNumber);
            } else {
                spinnerPatterNum.setEnabled(true);
                spinnerPatterNum.setAdapter(adapterPatternNumberDefault);
                Log.d(PhUtilities.UI_TAG, "Spinner is enabled");
                if(chkBoxLlcp.isChecked()){
                    editTextCustomPatternNum.setEnabled(false);
                    spinnerPatterNum.setAdapter(adapterPatternNumberLlcp);
                }else if(chkBoxSnep.isChecked()){
                        editTextCustomPatternNum.setEnabled(false);
                }else{
                    editTextCustomPatternNum.setEnabled(true);
                }
            }

            /*** Restricting edit text to maximum 4 characters */
            if (len > maxLen) {
                int selEndIndex = Selection.getSelectionEnd(editable);
                String str = editable.toString();
                String newStr = str.substring(0, maxLen);
                editTextCustomPatternNum.setText(newStr);
                editable = editTextCustomPatternNum.getText();
                int newLen = editable.length();
                if (selEndIndex > newLen) {
                    selEndIndex = editable.length();
                }
                Selection.setSelection(editable, selEndIndex);
            }
        }
    }

    /*** Spinner on selection */

    @Override
    public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
            long arg3) {
        switch(arg0.getId()){
        case R.id.spinner_text:
            Log.d(PhUtilities.UI_TAG,
                    "get pattern number " + spinnerPatterNum.getSelectedItem().toString());

            nxpHelperMainActivity.setsPatternnumber(spinnerPatterNum.getSelectedItem().toString());
            break;
        case R.id.certification_version:
            Log.d(PhUtilities.UI_TAG,
                    "get certification version " + certificationVersion.getSelectedItem().toString());
            nxpHelperMainActivity.setsCertificationVersion(certificationVersion.getSelectedItem().toString());
            break;
        case R.id.timeslotnumber_options:
            Log.d(PhUtilities.UI_TAG,
                    "get timeslot number " + timeSlotNumberF.getSelectedItem().toString());
            nxpHelperMainActivity.setsTimeSlotNumberF(timeSlotNumberF.getSelectedItem().toString());
            break;
        case R.id.condevicelimit_options:
            Log.d(PhUtilities.UI_TAG,
                    "get Connection device limit " + connectionDeviceLimit.getSelectedItem().toString());
            nxpHelperMainActivity.setsConnectionDeviceLimit(connectionDeviceLimit.getSelectedItem().toString());
            break;

        }


    }

    @Override
    public void onNothingSelected(AdapterView<?> arg0) {

    }

    /**
     * Enable and disable button a/c to auto or manual mode
     */

    @Override
    protected void onDestroy() {
        if (!mNfcAdapter.isEnabled() && PhUtilities.NDK_IMPLEMENTATION
                && !PhCustomExitDialog.deInitInCustomExitDialog) {
            Log.d(PhUtilities.UI_TAG, "Stop the Deinitialize the DTA");
            phNXPJniHelper.phDtaLibDisableDiscovery();
            android.os.Process.killProcess(android.os.Process.myPid());
        }else{
            android.os.Process.killProcess(android.os.Process.myPid());
        }
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        runBtn.setEnabled(true);
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
        nfccheck();
        if(!(mNfcAdapter.isEnabled())){
            if(PhUtilities.NDK_IMPLEMENTATION==true && phNXPJniHelper.startPhDtaLibInit()==0){
                Log.e(PhUtilities.UI_TAG, "DTA is Initializing");
            }else{
                Log.e(PhUtilities.UI_TAG, "Already DTA is Initialized");
            }
        }
    }
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    public void getUpdatesFromView() {

        phDtaLibStructureObj.pollP2P    = chkBoxPollP2p.isChecked();
        phDtaLibStructureObj.pollRdWt   = chkBoxPollRdWt.isChecked();
        phDtaLibStructureObj.listenP2P  = chkBoxListenP2p.isChecked();
        phDtaLibStructureObj.listenUicc = chkBoxListenUicc.isChecked();
        phDtaLibStructureObj.listenHce  = chkBoxListenHce.isChecked();
        phDtaLibStructureObj.listenEse  = chkBoxListenEse.isChecked();

        phDtaLibStructureObj.initializeRfTechObjs();
        phDtaLibStructureObj.pollP2pRfTech.technologyA = chkBoxPollP2PA.isChecked();
        phDtaLibStructureObj.pollP2pRfTech.technologyB = chkBoxPollP2PB.isChecked();
        phDtaLibStructureObj.pollP2pRfTech.technologyF = chkBoxPollP2PF.isChecked();

        phDtaLibStructureObj.pollRdWtRfTech.technologyA = chkBoxPollRDWTA.isChecked();
        phDtaLibStructureObj.pollRdWtRfTech.technologyB = chkBoxPollRDWTB.isChecked();
        phDtaLibStructureObj.pollRdWtRfTech.technologyF = chkBoxPollRDWTF.isChecked();

        phDtaLibStructureObj.listenUiccRfTech.technologyA = chkBoxListenUICCA.isChecked();
        phDtaLibStructureObj.listenUiccRfTech.technologyB = chkBoxListenUICCB.isChecked();
        phDtaLibStructureObj.listenUiccRfTech.technologyF = chkBoxListenUICCF.isChecked();

        phDtaLibStructureObj.listenHceRfTech.technologyA = chkBoxListenHCEA.isChecked();
        phDtaLibStructureObj.listenHceRfTech.technologyB = chkBoxListenHCEB.isChecked();
        phDtaLibStructureObj.listenHceRfTech.technologyF = chkBoxListenHCEF.isChecked();

        phDtaLibStructureObj.listenEseRfTech.technologyA = chkBoxListenESEA.isChecked();
        phDtaLibStructureObj.listenEseRfTech.technologyB = chkBoxListenESEB.isChecked();
        phDtaLibStructureObj.listenEseRfTech.technologyF = chkBoxListenESEF.isChecked();

        phDtaLibStructureObj.listenP2pRfTech.technologyA = chkBoxListenP2PA.isChecked();
        phDtaLibStructureObj.listenP2pRfTech.technologyB = chkBoxListenP2PB.isChecked();
        phDtaLibStructureObj.listenP2pRfTech.technologyF = chkBoxListenP2PF.isChecked();

        phDtaLibStructureObj.enableLlcp    = chkBoxLlcp.isChecked();
        phDtaLibStructureObj.enableSnep    = chkBoxSnep.isChecked();
        phDtaLibStructureObj.patternNum = Integer.parseInt(sPatternNumber,16);
        phDtaLibStructureObj.certificationVerNum = nxpHelperMainActivity.getsCertificationVersion();
        phDtaLibStructureObj.timeSlotNumberF = Integer.parseInt(nxpHelperMainActivity.getsTimeSlotNumberF(), 16);
        phDtaLibStructureObj.connectionDeviceLimit = Integer.parseInt(nxpHelperMainActivity.getsConnectionDeviceLimit(),16);
        phDtaLibStructureObj.dtaDebugFlag = nxpHelperMainActivity.getdtaDebugFlag();
        phDtaLibStructureObj.enableParamsInLlcpConnectPdu =
                chkBoxLlcpConnectPduPrms.isChecked();

        Log.d(PhUtilities.UI_TAG, "PollP2P:Y/N:A:B:F =" + phDtaLibStructureObj.pollP2P +
                ":" + phDtaLibStructureObj.pollP2pRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.pollP2pRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.pollP2pRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "PollRdWt:Y/N:A:B:F =" + phDtaLibStructureObj.pollRdWt +
                ":" + phDtaLibStructureObj.pollRdWtRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.pollRdWtRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.pollRdWtRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "ListenP2P:Y/N:A:B:F =" + phDtaLibStructureObj.listenP2P +
                ":" + phDtaLibStructureObj.listenP2pRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.listenP2pRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.listenP2pRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "ListenHCE:Y/N:A:B:F =" + phDtaLibStructureObj.listenHce +
                ":" + phDtaLibStructureObj.listenHceRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.listenHceRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.listenHceRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "ListenUICC:Y/N:A:B:F =" + phDtaLibStructureObj.listenUicc +
                ":" + phDtaLibStructureObj.listenUiccRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.listenUiccRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.listenUiccRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "ListenEse:Y/N:A:B:F =" + phDtaLibStructureObj.listenEse +
                ":" + phDtaLibStructureObj.listenEseRfTech.technologyA + ":" +
                ":" + phDtaLibStructureObj.listenEseRfTech.technologyB + ":" +
                ":" + phDtaLibStructureObj.listenEseRfTech.technologyF);
        Log.d(PhUtilities.UI_TAG, "patternNum ="
                + phDtaLibStructureObj.patternNum);
        Log.d(PhUtilities.UI_TAG, "timeSlotNumberF ="
                + phDtaLibStructureObj.timeSlotNumberF);
        Log.d(PhUtilities.UI_TAG, "connectionDeviceLimit ="
                + phDtaLibStructureObj.connectionDeviceLimit);
        Log.d(PhUtilities.UI_TAG, "EnableLLCP =" + phDtaLibStructureObj.enableLlcp);
        Log.d(PhUtilities.UI_TAG, "EnableSNEP =" + phDtaLibStructureObj.enableSnep);
    }

    public void setIpAdressAndPortNumber(String ipAddress, String portNumberString) throws UnknownHostException{
        inetAddress = InetAddress.getByName(ipAddress);
        portNumber = Integer.parseInt(portNumberString);
    }
    private void clientImplementation() {
        if(PhUtilities.CLIENT_SELECTED){
            clientConnectionProgress = ProgressDialog.show(PhDTAUIMainActivity.this, "ClientConnection", "Client trying to Connect to Server", true, true);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while(PhUtilities.countOfRetry <= PhUtilities.maxNumberOfRetries){
                        Log.d(PhUtilities.UI_TAG, " "+PhUtilities.countOfRetry);
                       try{
                           Thread.sleep(100);
                       } catch(Exception e){

                       }
                       if(PhUtilities.isClientConnected){
                           clientConnectionProgress.dismiss();
                           break;
                       }
                    }
                    if(!PhUtilities.isClientConnected){
                        clientConnectionProgress.dismiss();
                        final Builder alertDialog = new AlertDialog.Builder(PhDTAUIMainActivity.this);
                        alertDialog.setTitle("Client Message");
                        alertDialog.setMessage("Retried maximum number of times...No Luck");
                        alertDialog.setNeutralButton("Ok", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        });
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                alertDialog.show();
                            }
                        });
                    }
                }
            }).start();
        }
    }

    public void setFindViewByID(){
       // versionNumber = (TextView) findViewById(R.id.version_number);
        editTxtLogFileName = (EditText) findViewById(R.id.logtofiletext);
        editTextCustomPatternNum = (EditText) findViewById(R.id.custom);
        /**
         * Calling all the Check Box id's
         */
        chkBoxLogToFile = (CheckBox) findViewById(R.id.logtofile);
        chkBoxLogCat = (CheckBox) findViewById(R.id.logcat);
        chkBoxInfo = (CheckBox) findViewById(R.id.versionnumber);
        customMessage = (CheckBox) findViewById(R.id.custom_msg);
        showMessage = (CheckBox) findViewById(R.id.show_msg);
        /**
         * CheckBox for clearing selections under P2P category
         */
        /**
         * Calling all the Radio Group id's
         */
        autoManualRadioGroup = (RadioGroup) findViewById(R.id.auto_manual_rb);
        /**
         * Calling all the Radio Button id's
         */
        radioBtnAutoMode = (RadioButton) findViewById(R.id.auto_radio_button);
        radioBtnManualMode = (RadioButton) findViewById(R.id.manual_radio_button);
        chkBoxPollP2p=(CheckBox)findViewById(R.id.p2p_poll_check_box);
        chkBoxPollRdWt=(CheckBox)findViewById(R.id.rd_wt_poll_check_box);
        chkBoxListenP2p = (CheckBox) findViewById(R.id.p2p_listen_check_box);
        chkBoxListenUicc = (CheckBox) findViewById(R.id.uicc_listen_check_box);
        chkBoxListenHce = (CheckBox) findViewById(R.id.hce_listen_check_box);
        chkBoxLlcpConnectPduPrms=(CheckBox) findViewById(R.id.llcp_content_check_box);
        chkBoxListenEse = (CheckBox) findViewById(R.id.ese_listen_check_box);
        chkBoxLlcp = (CheckBox) findViewById(R.id.llcp_check_box);
        chkBoxSnep = (CheckBox) findViewById(R.id.snep_check_box);
        /**
         * Calling all the Text View id's
         */
        txtVwCopyRights = (TextView) findViewById(R.id.copyrights);
        txtVwAppStatus = (TextView) findViewById(R.id.tcp_ip_txt);
        txtVwLogMsg = (TextView) findViewById(R.id.log_msg);
        txtVwFwVer = (TextView) findViewById(R.id.firmwareversion_text_view);
        /**
         * Calling all the Spinner id's
         */
        spinnerPatterNum = (Spinner) findViewById(R.id.spinner_text);
        certificationVersion = (Spinner) findViewById(R.id.certification_version);
        timeSlotNumberF = (Spinner) findViewById(R.id.timeslotnumber_options);
        connectionDeviceLimit = (Spinner) findViewById(R.id.condevicelimit_options);
        tsnRelativeLyt = (RelativeLayout)findViewById(R.id.timeslotnumber_layout);
        connDevRelativeLyt = (RelativeLayout)findViewById(R.id.condevicelimit_layout);
        /**
         * Calling all the Button id's
         */
        runBtn = (Button) findViewById(R.id.run);
        stopBtn = (Button) findViewById(R.id.stop);
        exitBtn = (Button) findViewById(R.id.exit_btn);
        chkBoxLogCat.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        editTxtLogFileName.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);

        chkBoxPollP2PA = (CheckBox) findViewById(R.id.a_p2p_poll_check_box);
        chkBoxPollP2PB = (CheckBox) findViewById(R.id.b_p2p_poll_check_box);
        chkBoxPollP2PF = (CheckBox) findViewById(R.id.f_p2p_poll_check_box);
        chkBoxPollRDWTA= (CheckBox) findViewById(R.id.a_rdwt_poll_check_box);
        chkBoxPollRDWTB= (CheckBox) findViewById(R.id.b_rdwt_poll_check_box);
        chkBoxPollRDWTF= (CheckBox) findViewById(R.id.f_rdwt_poll_check_box);
        chkBoxListenP2PA=(CheckBox) findViewById(R.id.a_p2p_listen_check_box);
        chkBoxListenP2PB=(CheckBox) findViewById(R.id.b_p2p_listen_check_box);
        chkBoxListenP2PF=(CheckBox) findViewById(R.id.f_p2p_listen_check_box);
        chkBoxListenUICCA= (CheckBox) findViewById(R.id.a_uicc_listen_check_box);
        chkBoxListenUICCB= (CheckBox) findViewById(R.id.b_uicc_listen_check_box);
        chkBoxListenUICCF= (CheckBox) findViewById(R.id.f_uicc_listen_check_box);
        chkBoxListenHCEA= (CheckBox) findViewById(R.id.a_hce_listen_check_box);
        chkBoxListenHCEB= (CheckBox) findViewById(R.id.b_hce_listen_check_box);
        chkBoxListenHCEF= (CheckBox) findViewById(R.id.f_hce_listen_check_box);
        chkBoxListenESEA= (CheckBox) findViewById(R.id.a_ese_listen_check_box);
        chkBoxListenESEB= (CheckBox) findViewById(R.id.b_ese_listen_check_box);
        chkBoxListenESEF= (CheckBox) findViewById(R.id.f_ese_listen_check_box);
    }

    @Override
    public void onCheckedChanged(CompoundButton view, boolean arg1) {
        switch (view.getId()) {
        case R.id.logtofile:
            if (chkBoxLogToFile.isChecked()) {
                editTxtLogFileName.setFocusable(true);
                editTxtLogFileName.setFocusableInTouchMode(true);
            } else {
                editTxtLogFileName.setFocusable(false);
                editTxtLogFileName.setFocusableInTouchMode(false);
                editTxtLogFileName.setText("");
            }
            break;

        case R.id.logcat:
             if (chkBoxLogCat.isChecked()) {}
        break;
        }
    }
    private void appearDissappearOFViews() {
        if (PhUtilities.APPEAR_CUSTOM) {
            editTextCustomPatternNum.setVisibility(View.VISIBLE);
        } else if (PhUtilities.DISAPPER_CUSTOM) {
            editTextCustomPatternNum.setVisibility(View.INVISIBLE);
        } else if (PhUtilities.FADING_CUSTOM) {
            editTextCustomPatternNum.setEnabled(false);
        }

        /** Appear disappear for auto_mode */
        if (PhUtilities.APPEAR_AUTO_MODE) {
            radioBtnAutoMode.setVisibility(View.VISIBLE);
        } else if (PhUtilities.DISAPPER_AUTO_MODE) {
            radioBtnAutoMode.setVisibility(View.INVISIBLE);
        } else if (PhUtilities.FADING_AUTO_MODE) {
            radioBtnAutoMode.setEnabled(false);
        }
        /** Appear disappear for show message */
        if (PhUtilities.APPEAR_SHOW_MESSAGE) {
            showMessage.setVisibility(View.VISIBLE);
        } else if (PhUtilities.DISAPPER_SHOW_MESSAGE) {
            showMessage.setVisibility(View.INVISIBLE);
        } else if (PhUtilities.FADING_SHOW_MESSAGE) {
            showMessage.setEnabled(false);
        }
        /** Appear disappear for custom message */
        if (PhUtilities.APPEAR_CUSTOM_MESSAGE) {
            customMessage.setVisibility(View.VISIBLE);
        } else if (PhUtilities.DISAPPER_CUSTOM_MESSAGE) {
            customMessage.setVisibility(View.INVISIBLE);
        } else if (PhUtilities.FADING_CUSTOM_MESSAGE) {
            customMessage.setEnabled(false);
        }
        /** Appear disappear for TCP/IP Text message */
        if (PhUtilities.APPEAR_TCP_IP_STATUS) {
            txtVwAppStatus.setVisibility(View.VISIBLE);
        } else if (PhUtilities.DISAPPER_TCP_IP_STATUS) {
            txtVwAppStatus.setVisibility(View.INVISIBLE);
        } else if (PhUtilities.FADING_TCP_IP_STATUS) {
            txtVwAppStatus.setEnabled(false);
        }
        /** Appear disappear for card emulation */
        if (PhUtilities.APPEAR_CARD_EMULATION) {
            chkBoxListenUicc.setVisibility(View.VISIBLE);
            chkBoxListenHce.setVisibility(View.VISIBLE);
            chkBoxListenEse.setVisibility(View.VISIBLE);

        } else if (PhUtilities.DISAPPER_CARD_EMULATION) {
            chkBoxListenUicc.setVisibility(View.INVISIBLE);
            chkBoxListenHce.setVisibility(View.INVISIBLE);
            chkBoxListenEse.setVisibility(View.INVISIBLE);

        } else if (PhUtilities.FADING_CARD_EMULATION) {
            chkBoxListenUicc.setEnabled(false);
            chkBoxListenHce.setEnabled(false);
            chkBoxListenEse.setEnabled(false);
        }
        /** Appear disappear for Log Message */
        if (PhUtilities.APPEAR_LOG_MESSAGE) {
            editTxtLogFileName.setVisibility(View.VISIBLE);
            chkBoxLogToFile.setVisibility(View.VISIBLE);
            chkBoxLogCat.setVisibility(View.VISIBLE);

        } else if (PhUtilities.DISAPPER_LOG_MESSAGE) {
            txtVwLogMsg.setVisibility(View.INVISIBLE);
            editTxtLogFileName.setVisibility(View.INVISIBLE);
            chkBoxLogToFile.setVisibility(View.INVISIBLE);
            chkBoxLogCat.setVisibility(View.INVISIBLE);

        } else if (PhUtilities.FADING_LOG_MESSAGE) {
            editTxtLogFileName.setEnabled(false);
            txtVwLogMsg.setEnabled(false);
            chkBoxLogToFile.setEnabled(false);
            chkBoxLogCat.setEnabled(false);
        }

        /** Appear disappear for Device selection and spinner */
        if (PhUtilities.APPEAR_DEVICE) {
            txtVwFwVer.setVisibility(View.VISIBLE);
            chkBoxInfo.setVisibility(View.VISIBLE);

        } else if (PhUtilities.DISAPPER_DEVICE) {
            txtVwFwVer.setVisibility(View.INVISIBLE);
            chkBoxInfo.setVisibility(View.INVISIBLE);

        } else if (PhUtilities.FADING_DEVICE) {
            txtVwFwVer.setEnabled(false);
            chkBoxInfo.setEnabled(PhUtilities.WAVE_TWO_INFO_N_LOG);
        }

    }

    private void handleStartButtonEvent(){
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
        boolean technologySelectionerror =chkBoxListenP2p.isChecked() || chkBoxListenUicc.isChecked() || chkBoxListenHce.isChecked() || chkBoxListenEse.isChecked() || chkBoxPollP2p.isChecked() || chkBoxPollRdWt.isChecked();
        if(!mNfcAdapter.isEnabled() && technologySelectionerror){
            runBtn.setClickable(false);
            txtVwAppStatus.setText(Html.fromHtml(currentStatusInitializing),TextView.BufferType.SPANNABLE);
            runBtn.setBackgroundColor(Color.parseColor(getResources().getString(R.color.grey)));
            Log.e(PhUtilities.UI_TAG, "runButton clicked");
        /*
         * statusTextView.setText(Html.fromHtml(currentStatusRunning),
         * TextView.BufferType.SPANNABLE);
         */
//        statusTextView.post(new Runnable() {
//            public void run() {
                if(!PhUtilities.isServerRunning){
                    /*AlertDialog.Builder alertDialog = new AlertDialog.Builder(PhDTAUIMainActivity.this);
                    alertDialog.setTitle("Client Message");
                    alertDialog.setMessage("Server is not running");
                    alertDialog.setPositiveButton("Retry", new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(PhUtilities.TCPSRV_TAG, "trying to reconnect");

                        }
                    });
                    alertDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.cancel();

                        }
                    });
                    alertDialog.show(); */
                }
                clientImplementation();
//            }
//        });
        radioBtnAutoMode.setEnabled(false);
        radioBtnManualMode.setEnabled(false);

        /**
         * Console check weather it is checked
         */
        if (chkBoxLogCat.isChecked()) {
            Intent intent = new Intent(getBaseContext(),
                    PhLogCatActivity.class);
            startActivity(intent);
        }
        if (chkBoxLogToFile.isChecked()) {
            logFileName = editTxtLogFileName.getText().toString();
            try {
                Log.d(PhUtilities.UI_TAG,
                        "Inside Run, Creating file and writing log to it .... ");
                nxpHelperMainActivity.createFileWithGivenNameOnExternalStorageAndWriteLog(logFileName
                                + ".txt");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        /**
         * Creating files based on conditions checked
         */
        if (PhUtilities.LOG_DUT_MESSAGES) {
            try {
                if (!PhUtilities.FILE_NAME_DUT.equals("")) {
                    Log.e(PhUtilities.UI_TAG, "Creating DUT file");
                    nxpHelperMainActivity
                            .createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_DUT);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if (PhUtilities.LOG_LT_MESSAGES) {
            try {
                if (!PhUtilities.FILE_NAME_LT.equals("")) {
                    Log.e(PhUtilities.UI_TAG, "Crating LT file");
                    nxpHelperMainActivity.createFileWithGivenNameOnsdCard(PhUtilities.FILE_NAME_LT);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        /** Need To add once so file is changed */
        /**
         * NativeUtilities nativeUtilities = new
         * NativeUtilities();nativeUtilities.ndkImplementation();
         */

        /*** Need To remove once so file is changed */

        /*** NDK implementation */
        // loadJNILibrary();

        isRunClickedAtlLeastOnce = true;

        if (PhUtilities.NDK_IMPLEMENTATION) {
            // SOM
            // 4. Creating background Async Task for start dta
            // dtaOperationStartTask = new DtaOperationStart();
            // 5. Creating watchdog timer object
            /** check auto mode or manual mode */
            if (radioBtnManualMode.isChecked()) {
                Log.d(PhUtilities.UI_TAG, "Looping into Manual Mode");
                // Get pattern number from EditText
                String getPatternNoEditText = editTextCustomPatternNum
                        .getText().toString().trim();
                if (!editTextCustomPatternNum.getText().toString().isEmpty()) {
                    sPatternNumber = getPatternNoEditText;
                }
                // Get pattern number from Spinner
                else {
                    sPatternNumber = nxpHelperMainActivity
                            .getsPatternnumber();
                    Log.d(PhUtilities.UI_TAG,
                            "Get pattern number from spinner"
                                    + sPatternNumber);

                }
            } else if (radioBtnAutoMode.isChecked()) {
                automodeOnRunButtonPressed();
                }
        }

            // Common Code for both Auto & Manual Modes

        runBtn.setClickable(false);
        try {
            enableDiscovery();
        } catch (Exception e) {
            e.printStackTrace();
        }

        onClickColoringRunning = true;
        disableAllOtherBtns();
        stopBtn.setBackgroundColor(Color.parseColor(getResources()
                .getString(R.color.red)));
        exitBtn.setBackgroundColor(Color.parseColor(getResources()
                .getString(R.color.grey)));
        }else{
              technologyError(getResources().getString(R.string.technology_error));
        }
            nfccheck();

    }

    private void handleStopButtonEvent(){
        if((onClickColoringRunning) && (PhUtilities.STOP_BUTTON_HANDLE != 0xFFFF)){
            PhDTAJniEvent eEvent= new PhDTAJniEvent(PhDTAJniEvent.EventType.values()[PhUtilities.STOP_BUTTON_HANDLE]);
            }
            if ((PhUtilities.STOP_BUTTON_HANDLE ==
                 PhDTAJniEvent.EventType.PHDTALIB_TEST_CASE_EXEC_STARTED
                     .ordinal()) &&
                (!(bIsInAutomationMode))) {
              Log.d(PhUtilities.UI_TAG,
                    "alertDialogBuilder in test case execution in progress");
              AlertDialog.Builder alertDialogBuilder =
                  new AlertDialog.Builder(this);
              alertDialogBuilder.setTitle("Warning:");
              alertDialogBuilder.setMessage(
                  "Test case Execution is in progress, pressing yes will exit the application");
              alertDialogBuilder.setCancelable(false);
              alertDialogBuilder.setPositiveButton(
                  "Yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int arg1) {
                      stopBtn.setBackgroundColor(Color.parseColor(
                          getResources().getString(R.color.grey)));
                      runBtn.setBackgroundColor(Color.parseColor(
                          getResources().getString(R.color.green)));
                      runBtn.setEnabled(true);
                      runBtn.setClickable(true);
                      exitBtn.setBackgroundColor(Color.parseColor(
                          getResources().getString(R.color.orange)));
                      onClickColoringRunning = false;
                      spinnerPatterNum.setSelection(0);
                      certificationVersion.setSelection(0);
                      timeSlotNumberF.setSelection(0);
                      connectionDeviceLimit.setSelection(0);
                      dialog.cancel();
                      Log.v(PhUtilities.UI_TAG,
                            "calling phDtaLibDisableDiscovery");
                      phNXPJniHelper.phDtaLibDisableDiscovery();
                      android.os.Process.killProcess(
                          android.os.Process.myPid());
                    }
                  });
              alertDialogBuilder.setNegativeButton(
                  "No", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                    }
                  });
              alertDialogBuilder.show();
            }else{
            runBtn.setClickable(true);
            Log.e(PhUtilities.UI_TAG, "stop clicked");
            txtVwAppStatus.setText(Html.fromHtml(currentStatusStopped),
                    TextView.BufferType.SPANNABLE);

            /**
             * Mode check done here
             */
            if (radioBtnAutoMode.isChecked()) {
                autoModeisON();
            } else if (radioBtnManualMode.isChecked()) {
                handleStopBtnInManualMode();
            }

            if(chkBoxLlcp.isChecked()){
                editTextCustomPatternNum.setEnabled(false);
            }else if(chkBoxSnep.isChecked()){
                editTextCustomPatternNum.setEnabled(false);
                if(null != spinnerPatterNum)
                spinnerPatterNum.setEnabled(false);
                if(certificationVersion != null)
                certificationVersion.setEnabled(false);
                if(timeSlotNumberF != null)
                    timeSlotNumberF.setEnabled(false);
                if(connectionDeviceLimit != null)
                    connectionDeviceLimit.setEnabled(false);
            }

            /**
             * Change button background colour to user actions
             */
            if (onClickColoringRunning) {
                stopBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.grey)));
                runBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.green)));
                runBtn.setEnabled(true);
                exitBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.orange)));
                onClickColoringRunning = false;
                spinnerPatterNum.setSelection(0);
                disableDiscovery();

            } else {
                runBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.green)));
                runBtn.setEnabled(true);
                stopBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.grey)));
                exitBtn.setBackgroundColor(Color.parseColor(getResources()
                        .getString(R.color.orange)));
                Toast.makeText(getApplicationContext(), "Run before Stop",
                        Toast.LENGTH_LONG).show();
            }

            // This should be done only for auto mode// SOM
            PhUtilities.SIMULATED_SERVER_SELECTED = false;
            PhUtilities.CLIENT_SELECTED = false;
            PhUtilities.SERVER_SELECTED = false;
            PhUtilities.SIMULATED_CLIENT_SELECTED = false;
            }

    }

    private void handleExitButtonEvent(){
        if (onClickColoringRunning) {
            exitBtn.setBackgroundColor(Color.parseColor(getResources()
                    .getString(R.color.grey)));// Can be removed
            runBtn.setClickable(false);
            Toast.makeText(getApplicationContext(),
                    "Stop the application then exit", Toast.LENGTH_LONG)
                    .show();
        } else {
            /**
             * Exit pop up will open need to close the application or not
             */
            PhCustomExitDialog customExitDialog = new PhCustomExitDialog(
                    this,"Confirm Exit?",false);
            runBtn.setClickable(true);
            customExitDialog.show();
        }

    }

    public void deInit() {
        File file = null;
        boolean deleteStatus = false;
        final String filePath = "/data/nfc/";
        /*this file contains time stamp of (libnfc*.conf) configuration file. During NFC NFA Init,
         * All the configurations are updated from libnfc*.config only if timestamp is modified*/
        final String fileName = "libnfc-nxpConfigState.bin";
        file = new File(filePath, fileName);
        if (file.exists() == true) {
            /*
             * As DTA modifies configuration values(for ex: SWP1,SWP2),deleting
             * libnfc-nxpConfigState.bin file to revert back to previous
             * configuration done by NFC Service.
             */
            deleteStatus = file.delete();
            if (deleteStatus == true)
                Log.d(PhUtilities.UI_TAG,
                        "libnfc-nxpConfigState.bin file deleted successfully");
            else
                Log.e(PhUtilities.UI_TAG,
                        "libnfc-nxpConfigState.bin file delete Un-successfull");
        } else {
            Log.e(PhUtilities.UI_TAG,
                    "libnfc-nxpConfigState.bin file doesn't exisist");
        }
        if (PhUtilities.NDK_IMPLEMENTATION == true) {
            try {
                PhNXPJniHelper phNXPJniHelper = PhNXPJniHelper.getInstance();
                phNXPJniHelper.phDtaLibDeInit();
                android.os.Process.killProcess(android.os.Process.myPid());
            } catch (Exception e) {
                e.printStackTrace();
            } catch (NoSuchMethodError e) {
                e.printStackTrace();
            } catch (NoClassDefFoundError e) {
                e.printStackTrace();
            } catch (UnsatisfiedLinkError e) {
                e.printStackTrace();
            }

        } else {
            android.os.Process.killProcess(android.os.Process.myPid());
        }

    }

    private void handleLogtoFileCheckBoxEvent() {
        editTxtLogFileName.setFocusable(true);
        editTxtLogFileName.setFocusableInTouchMode(true);
        editTxtLogFileName.setEnabled(true);

        /**
         * Log File Edit Text Validation should be done here if Edit Text
         * empty check box is disabled if not it will be enable
         */
        editTxtLogFileName.addTextChangedListener(new TextWatcher() {
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
                chkBoxLogToFile = (CheckBox) findViewById(R.id.logtofile);
                editTxtLogFileName = (EditText) findViewById(R.id.logtofiletext);
                if (textInEditBox.length() < 1) {
                    Toast.makeText(getBaseContext(),
                            "Enter File Name To Store Log", 3).show();

                    chkBoxLogToFile.setEnabled(false);
                    chkBoxLogToFile.setFocusable(false);
                    chkBoxLogToFile.setChecked(false);
                } else {
                    chkBoxLogToFile.setEnabled(true);
                }
            }
        });
    }

    private void handleAutoModeCustomCheckMessageCheckBoxEvent() {
        /**
         * Open the Custom Message Pop Up
         */
        if (customMessage.isChecked()) {
            PhCustomMSGDialog customMSGDialog = new PhCustomMSGDialog(this);
            customMSGDialog.show();
        }

    }

    private void handleAutoModeShowMessageCheckBoxEvent() {
        /**
         * Open the Custom Message Pop Up
         */
        if (showMessage.isChecked()) {
            PhShowMSGDialog showMSGDialog = new PhShowMSGDialog(this);
            showMSGDialog.show();
        }
    }

    private void handleVersionNumberCheckBoxEvent() {
        /**
         * Set Version Number from DTA Library
         */
        if (chkBoxInfo.isChecked()) {
            if (PhUtilities.NDK_IMPLEMENTATION) {
                phNXPJniHelper.versionLib(phDtaLibStructureObj);
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getDtaLibVerMajor());
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getDtaLibVerMinor());
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getLibNfcVerMajor());
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getLibNfcVerMinor());
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getFwVerMajor());
                Log.d(PhUtilities.UI_TAG,
                        "" + phDtaLibStructureObj.getFwVerMinor());

                txtVwFwVer.setText("UI Version: "+ DTA_GUI_VERSION +
                        "\nFirmWare Version: "
                        + Integer.toHexString(phDtaLibStructureObj.getFwVerMajor()) + "."
                        + Integer.toHexString(phDtaLibStructureObj.getFwVerMinor())
                        + "\nMiddleWare Version:"
                        + Integer.toHexString(phDtaLibStructureObj.getLibNfcVerMajor()) + "."
                        + Integer.toHexString(phDtaLibStructureObj.getLibNfcVerMinor())
                        + " \nDTA Lib Version: "
                        + Integer.toHexString(phDtaLibStructureObj.getDtaLibVerMajor()) + "."
                        + Integer.toHexString(phDtaLibStructureObj.getDtaLibVerMinor()));

            } else {
                /**
                 * Handling the error if problem in loading the library
                 */
                editTextCustomPatternNum.setFocusable(false);
                PhCustomLibDialog customLibDialog = new PhCustomLibDialog(
                        PhDTAUIMainActivity.this, "VERSION_INFO");
                customLibDialog.show();
            }
        } else {

            txtVwFwVer.setText(getResources().getString(
                    R.string.firmare_version));
        }
    }

    private void handleLLCPCheckBoxEvent() {
        if(chkBoxLlcp.isChecked()){
            chkBoxSnep.setEnabled(false);
            chkBoxLlcpConnectPduPrms.setEnabled(true);
            chkBoxLlcpConnectPduPrms.setChecked(true);
            Log.e(PhUtilities.UI_TAG, "llcp is selected");
            spinnerPatterNum.setEnabled(true);
            certificationVersion.setEnabled(true);
            timeSlotNumberF.setEnabled(true);
            connectionDeviceLimit.setEnabled(true);
            editTextCustomPatternNum.setText("");
            spinnerPatterNum.setAdapter(adapterPatternNumberLlcp);
            editTextCustomPatternNum.setEnabled(false);
            chkBoxListenUicc.setChecked(false);
            chkBoxListenHce.setChecked(false);
            chkBoxListenEse.setChecked(false);
            chkBoxListenP2p.setChecked(true);
            chkBoxPollP2p.setChecked(true);
            chkBoxPollRdWt.setChecked(true);
            enableTechChkBoxsForModes();
            checkDefaultTechChkBxsForModes();
        }else{
            chkBoxSnep.setEnabled(true);
            chkBoxLlcpConnectPduPrms.setEnabled(false);
            chkBoxLlcpConnectPduPrms.setChecked(false);
            chkBoxListenUicc.setEnabled(true);
            chkBoxListenHce.setEnabled(true);
            chkBoxListenEse.setEnabled(true);
            spinnerPatterNum.setAdapter(adapterPatternNumberDefault);
            editTextCustomPatternNum.setEnabled(true);
        }
    }

    private void handleSNEPCheckBoxEvent() {
            try {
                editTextCustomPatternNum.setEnabled(true);
                spinnerPatterNum.setAdapter(adapterPatternNumberDefault);
                chkBoxLlcp.setChecked(false);
                chkBoxSnep.setChecked(false);
                if(phCustomSNEPRTD == null){
                    phCustomSNEPRTD = new PhCustomSNEPRTD(this);
                }
                phCustomSNEPRTD.show();
            } catch (Exception e) {
            }
    }

    private void handlePollP2PCheckBoxEvent() {
        /*As of now the polling can be done for technology and not for individual modes
         * So if P2P Poll is selected,RDWt Mode is also selected*/
        if(chkBoxPollP2p.isChecked()){
            chkBoxPollP2PA.setChecked(true);
            chkBoxPollP2PF.setChecked(true);
            chkBoxPollP2PA.setEnabled(true);
            chkBoxPollP2PF.setEnabled(true);
            chkBoxPollRdWt.setChecked(true);
            chkBoxPollRDWTA.setChecked(true);
            chkBoxPollRDWTB.setChecked(true);
            chkBoxPollRDWTF.setChecked(true);
            chkBoxPollRDWTA.setEnabled(true);
            chkBoxPollRDWTB.setEnabled(true);
            chkBoxPollRDWTF.setEnabled(true);
        }else{
            chkBoxPollP2PA.setChecked(false);
            chkBoxPollP2PF.setChecked(false);
            chkBoxPollP2PA.setEnabled(false);
            chkBoxPollP2PF.setEnabled(false);
            chkBoxPollRdWt.setChecked(false);
            chkBoxPollRDWTA.setChecked(false);
            chkBoxPollRDWTB.setChecked(false);
            chkBoxPollRDWTF.setChecked(false);
            chkBoxPollRDWTA.setEnabled(false);
            chkBoxPollRDWTB.setEnabled(false);
            chkBoxPollRDWTF.setEnabled(false);
        }
    }

    private void handlePollRDWTCheckBoxEvent() {
        /*As of now the polling can be done for technology and not for individual modes
         * So if P2P Poll is selected,RDWt Mode is also selected*/
        if(chkBoxPollRdWt.isChecked())
            chkBoxPollP2p.setChecked(true);
        else
            chkBoxPollP2p.setChecked(false);
        handlePollP2PCheckBoxEvent();
    }

    private void handleListenP2PCheckBoxEvent() {
        if(chkBoxListenP2p.isChecked()){
            chkBoxListenP2PA.setChecked(true);
            chkBoxListenP2PF.setChecked(true);
            chkBoxListenP2PA.setEnabled(true);
            chkBoxListenP2PF.setEnabled(true);
            if(chkBoxListenHCEF.isChecked()) {
                chkBoxListenHCEF.setChecked(false);
                Toast.makeText(PhDTAUIMainActivity.this, "HCE-F is disabled, when P2P Listen is selected", Toast.LENGTH_SHORT).show();
            }
        }else if(!chkBoxListenP2p.isChecked()){
            chkBoxListenP2PA.setChecked(false);
            chkBoxListenP2PF.setChecked(false);
            chkBoxListenP2PA.setEnabled(false);
            chkBoxListenP2PF.setEnabled(false);
        }
        if((!chkBoxListenHCEA.isChecked()) && (!chkBoxListenHCEB.isChecked()) && (!chkBoxListenHCEF.isChecked()))
        {
            chkBoxListenHce.setChecked(false);
            enableTechChkBoxsForModes();
        }

    }

    private void handleListenUICCCheckBoxEvent() {
        if(chkBoxListenUicc.isChecked()){
            chkBoxListenUICCA.setChecked(true);
            chkBoxListenUICCB.setChecked(true);
            chkBoxListenUICCF.setChecked(true);
            chkBoxListenUICCA.setEnabled(true);
            chkBoxListenUICCB.setEnabled(true);
            chkBoxListenUICCF.setEnabled(true);
        }else if(!chkBoxListenUicc.isChecked()){
            chkBoxListenUICCA.setChecked(false);
            chkBoxListenUICCB.setChecked(false);
            chkBoxListenUICCF.setChecked(false);
            chkBoxListenUICCA.setEnabled(false);
            chkBoxListenUICCB.setEnabled(false);
            chkBoxListenUICCF.setEnabled(false);
        }
    }

    private void handleListenHCECheckBoxEvent() {
        if(chkBoxListenHce.isChecked()){
            chkBoxListenHCEA.setChecked(true);
            chkBoxListenHCEB.setChecked(true);
            chkBoxListenHCEF.setChecked(false);
            chkBoxListenHCEA.setEnabled(true);
            chkBoxListenHCEB.setEnabled(true);
            chkBoxListenHCEF.setEnabled(true);
        }else if(!chkBoxListenHce.isChecked()){
            chkBoxListenHCEA.setChecked(false);
            chkBoxListenHCEB.setChecked(false);
            chkBoxListenHCEF.setChecked(false);
            chkBoxListenHCEA.setEnabled(false);
            chkBoxListenHCEB.setEnabled(false);
            chkBoxListenHCEF.setEnabled(false);
        }

        /* Enabling Listen P2P to default when HCE-F is not checked */
        if(!chkBoxListenHCEF.isChecked()) {
            chkBoxListenP2p.setChecked(true);
            handleListenP2PCheckBoxEvent();
        }
    }

    private void handleListenESECheckBoxEvent() {
        if(chkBoxListenEse.isChecked()){
            chkBoxListenESEA.setChecked(true);
            chkBoxListenESEB.setChecked(true);
            chkBoxListenESEF.setChecked(true);
            chkBoxListenESEA.setEnabled(true);
            chkBoxListenESEB.setEnabled(true);
            chkBoxListenESEF.setEnabled(true);
        }else if(!chkBoxListenEse.isChecked()){
            chkBoxListenESEA.setChecked(false);
            chkBoxListenESEB.setChecked(false);
            chkBoxListenESEF.setChecked(false);
            chkBoxListenESEA.setEnabled(false);
            chkBoxListenESEB.setEnabled(false);
            chkBoxListenESEF.setEnabled(false);
        }
    }

    private void handlePollP2pTechCheckBoxEvent() {
        if((!chkBoxPollP2PA.isChecked()) && (!chkBoxPollP2PB.isChecked()) && (!chkBoxPollP2PF.isChecked())){
                chkBoxPollP2p.setChecked(false);
        }
        /*Poll A&F state is common for P2P and RdWt mode.
         * So check for state of Technologies in RdWt mode also and then enable/disable chkboxes accordingly*/
        if((!chkBoxPollRDWTA.isChecked()) && (!chkBoxPollRDWTB.isChecked()) && (!chkBoxPollRDWTF.isChecked())){
            chkBoxPollRdWt.setChecked(false);
        }
        enableTechChkBoxsForModes();
    }

    private void handlePollRdWrtTechCheckBoxEvent() {
        if((!chkBoxPollRDWTA.isChecked()) && (!chkBoxPollRDWTB.isChecked()) && (!chkBoxPollRDWTF.isChecked())){
                chkBoxPollRdWt.setChecked(false);
        }
        /*Poll A&F state is common for P2P and RdWt mode.
        * So check for state of Technologies in P2P mode also and then enable/disable chkboxes accordingly*/
        if((!chkBoxPollP2PA.isChecked()) && (!chkBoxPollP2PB.isChecked()) && (!chkBoxPollP2PF.isChecked())){
            chkBoxPollP2p.setChecked(false);
        }
        enableTechChkBoxsForModes();
    }

    private void handleListenP2pTechCheckBoxEvent() {
        if((!chkBoxListenP2PA.isChecked()) && (!chkBoxListenP2PB.isChecked()) && (!chkBoxListenP2PF.isChecked()))
        {
            chkBoxListenP2p.setChecked(false);
            enableTechChkBoxsForModes();
        }
    }

    private void handleListenUiccTechCheckBoxEvent() {
        if((!chkBoxListenUICCA.isChecked()) && (!chkBoxListenUICCB.isChecked()) && (!chkBoxListenUICCF.isChecked()))
        {
                chkBoxListenUicc.setChecked(false);
                enableTechChkBoxsForModes();
        }
    }

    private void handleListenHceATechCheckBoxEvent(){
        if((!chkBoxListenHCEA.isChecked()) && (!chkBoxListenHCEB.isChecked()) && (!chkBoxListenHCEF.isChecked()))
        {
            chkBoxListenHce.setChecked(false);
            enableTechChkBoxsForModes();
        }

        if(chkBoxListenHCEF.isChecked()){
            chkBoxListenHCEF.setChecked(false);
        }
    }

    private void handleListenHceBTechCheckBoxEvent(){
        if((!chkBoxListenHCEA.isChecked()) && (!chkBoxListenHCEB.isChecked()) && (!chkBoxListenHCEF.isChecked()))
        {
            chkBoxListenHce.setChecked(false);
            enableTechChkBoxsForModes();
        }

        if(chkBoxListenHCEF.isChecked()){
            chkBoxListenHCEF.setChecked(false);
        }
    }

    private void handleListenHceFTechCheckBoxEvent() {
        if((!chkBoxListenHCEA.isChecked()) && (!chkBoxListenHCEB.isChecked()) && (!chkBoxListenHCEF.isChecked()))
        {
                chkBoxListenHce.setChecked(false);
                enableTechChkBoxsForModes();
        }
        Log.v("CheckBoxActivity", (chkBoxListenHCEA.isChecked() ? "HCEA checked" : "HCEA unchecked"));

        if(chkBoxListenHCEF.isChecked()){
            if(chkBoxListenP2p.isChecked()) {
                chkBoxListenP2p.setChecked(false);
                chkBoxListenP2PA.setChecked(false);
                chkBoxListenP2PF.setChecked(false);
                chkBoxListenP2PA.setEnabled(false);
                chkBoxListenP2PF.setEnabled(false);
                Toast.makeText(PhDTAUIMainActivity.this, "P2P Listen is disabled, when HCE-F is selected", Toast.LENGTH_SHORT).show();
            }else if((chkBoxListenHCEA.isChecked() || chkBoxListenHCEB.isChecked())){
                Toast.makeText(PhDTAUIMainActivity.this, "HCE-A and HCE-B is disabled, when HCE-F is selected ", Toast.LENGTH_SHORT).show();
            }
            chkBoxListenHCEA.setChecked(false);
            chkBoxListenHCEB.setChecked(false);
        } else { /* Enable P2P by default when HCE-F is not checked */
            chkBoxListenP2p.setChecked(true);
            handleListenP2PCheckBoxEvent();
        }
    }

    private void handleListenEseTechCheckBoxEvent() {
        if((!chkBoxListenESEA.isChecked()) && (!chkBoxListenESEB.isChecked()) && (!chkBoxListenESEF.isChecked()))
        {
                chkBoxListenEse.setChecked(false);
                enableTechChkBoxsForModes();
        }
    }

    private void handleAllTheModesEvent(){
        handlePollP2PCheckBoxEvent();
        handlePollRDWTCheckBoxEvent();
        handleListenP2PCheckBoxEvent();
        handleListenUICCCheckBoxEvent();
        handleListenHCECheckBoxEvent();
        handleListenESECheckBoxEvent();
    }

    public PhDtaLibStructure getPhDtaLibStructureObj() {
        return phDtaLibStructureObj;
    }

    public String getTxtVwAppStatus() {
        return txtVwAppStatus.getText().toString();
    }

    public void setTxtVwAppStatus(String strAppStatus) {
        this.txtVwAppStatus.setText(strAppStatus);
    }

    private void enableBtnsWhichAreTicked(){
        if(chkBoxPollP2PA.isChecked())
            chkBoxPollP2PA.setEnabled(true);
        if(chkBoxPollP2PB.isChecked())
            chkBoxPollP2PB.setEnabled(true);
        if(chkBoxPollP2PF.isChecked())
            chkBoxPollP2PF.setEnabled(true);
        if(chkBoxPollRDWTA.isChecked())
            chkBoxPollRDWTA.setEnabled(true);
        if(chkBoxPollRDWTB.isChecked())
            chkBoxPollRDWTB.setEnabled(true);
        if(chkBoxPollRDWTF.isChecked())
            chkBoxPollRDWTF.setEnabled(true);
        if(chkBoxListenP2PA.isChecked())
            chkBoxListenP2PA.setEnabled(true);
        if(chkBoxListenP2PB.isChecked())
            chkBoxListenP2PB.setEnabled(true);
        if(chkBoxListenP2PF.isChecked())
            chkBoxListenP2PF.setEnabled(true);
        if(chkBoxListenUICCA.isChecked())
            chkBoxListenUICCA.setEnabled(true);
        if(chkBoxListenUICCB.isChecked())
            chkBoxListenUICCB.setEnabled(true);
        if(chkBoxListenUICCF.isChecked())
            chkBoxListenUICCF.setEnabled(true);
        if(chkBoxListenHCEA.isChecked())
            chkBoxListenHCEA.setEnabled(true);
        if(chkBoxListenHCEB.isChecked())
            chkBoxListenHCEB.setEnabled(true);
        if(chkBoxListenHCEF.isChecked())
            chkBoxListenHCEF.setEnabled(true);
        if(chkBoxListenESEA.isChecked())
            chkBoxListenESEA.setEnabled(true);
        if(chkBoxListenESEB.isChecked())
            chkBoxListenESEB.setEnabled(true);
        if(chkBoxListenESEF.isChecked())
            chkBoxListenESEF.setEnabled(true);
    }

    private void enableTechChkBoxsForModes(){
        if(chkBoxPollP2p.isChecked()){
            chkBoxPollP2PA.setEnabled(true);
            chkBoxPollP2PF.setEnabled(true);
        }else{
            chkBoxPollP2PA.setEnabled(false);
            chkBoxPollP2PF.setEnabled(false);
        }
        if(chkBoxPollRdWt.isChecked()){
            chkBoxPollRDWTA.setEnabled(true);
            chkBoxPollRDWTB.setEnabled(true);
            chkBoxPollRDWTF.setEnabled(true);
        }else{
            chkBoxPollRDWTA.setEnabled(false);
            chkBoxPollRDWTB.setEnabled(false);
            chkBoxPollRDWTF.setEnabled(false);
        }
        if(chkBoxListenP2p.isChecked()){
            chkBoxListenP2PA.setEnabled(true);
            chkBoxListenP2PF.setEnabled(true);
        }else{
            chkBoxListenP2PA.setEnabled(false);
            chkBoxListenP2PF.setEnabled(false);
        }
        if(chkBoxListenUicc.isChecked()){
            chkBoxListenUICCA.setEnabled(true);
            chkBoxListenUICCB.setEnabled(true);
            chkBoxListenUICCF.setEnabled(true);
        }else{
            chkBoxListenUICCA.setEnabled(false);
            chkBoxListenUICCB.setEnabled(false);
            chkBoxListenUICCF.setEnabled(false);
        }
        if(chkBoxListenHce.isChecked()){
            chkBoxListenHCEA.setEnabled(true);
            chkBoxListenHCEB.setEnabled(true);
            chkBoxListenHCEF.setEnabled(true);
        }else{
            chkBoxListenHCEA.setEnabled(false);
            chkBoxListenHCEB.setEnabled(false);
            chkBoxListenHCEF.setEnabled(false);
        }
        if(chkBoxListenEse.isChecked()){
            chkBoxListenESEA.setEnabled(true);
            chkBoxListenESEB.setEnabled(true);
            chkBoxListenESEF.setEnabled(true);
        }else{
            chkBoxListenESEA.setEnabled(false);
            chkBoxListenESEB.setEnabled(false);
            chkBoxListenESEF.setEnabled(false);
        }
    }
    private void checkDefaultTechChkBxsForModes(){
        if(chkBoxPollP2p.isChecked()){
            chkBoxPollP2PA.setChecked(true);
            chkBoxPollP2PF.setChecked(true);
        }else{
            chkBoxPollP2PA.setChecked(false);
            chkBoxPollP2PF.setChecked(false);
        }
        if(chkBoxPollRdWt.isChecked()){
            chkBoxPollRDWTA.setChecked(true);
            chkBoxPollRDWTB.setChecked(true);
            chkBoxPollRDWTF.setChecked(true);
        }else{
            chkBoxPollRDWTA.setChecked(false);
            chkBoxPollRDWTB.setChecked(false);
            chkBoxPollRDWTF.setChecked(false);
        }
        if(chkBoxListenP2p.isChecked()){
            chkBoxListenP2PA.setChecked(true);
            chkBoxListenP2PF.setChecked(true);
        }else{
            chkBoxListenP2PA.setChecked(false);
            chkBoxListenP2PF.setChecked(false);
        }
        if(chkBoxListenUicc.isChecked()){
            chkBoxListenUICCA.setChecked(true);
            chkBoxListenUICCB.setChecked(true);
            chkBoxListenUICCF.setChecked(true);
        }else{
            chkBoxListenUICCA.setChecked(false);
            chkBoxListenUICCB.setChecked(false);
            chkBoxListenUICCF.setChecked(false);
        }
        if(chkBoxListenHce.isChecked()){
            chkBoxListenHCEA.setChecked(true);
            chkBoxListenHCEB.setChecked(true);
            chkBoxListenHCEF.setChecked(true);
        }else{
            chkBoxListenHCEA.setChecked(false);
            chkBoxListenHCEB.setChecked(false);
            chkBoxListenHCEF.setChecked(false);
        }
        if(chkBoxListenEse.isChecked()){
            chkBoxListenESEA.setChecked(true);
            chkBoxListenESEB.setChecked(true);
            chkBoxListenESEF.setChecked(true);
        }else{
            chkBoxListenESEA.setChecked(false);
            chkBoxListenESEB.setChecked(false);
            chkBoxListenESEF.setChecked(false);
        }
    }
}
