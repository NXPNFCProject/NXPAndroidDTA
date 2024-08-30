/*
* Copyright 2024 NXP
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

package com.nxp.nfcdta;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.cardemulation.CardEmulation;
import android.nfc.cardemulation.NfcFCardEmulation;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import android.os.Message;
import android.os.Handler;

public class MainActivity extends Activity {
  private static final String TAG = "DTA_Main";
  private final String SYSPROP_DEBUG = "nfc.dta.debug_enabled";
  private final String SYSPROP_CONFIGTLV = "nfc.dta.configTLV";
  private final String SYSPROP_SKIPNDEFREAD = "nfc.dta.skipNdefRead";
  private static boolean mDebug = true;
  private NfcAdapter mNfcAdapter;
  private final String TLV_DELIMITER = "_";
  private NfcFCardEmulation mNfcFCardEmulation;
  private CardEmulation mNfcCardEmulation;
  private static Thread mFrameDebugThread;
  private static Boolean mDebugThreadExit = false;
  private static byte[] mRecvFrame;
  private static byte[] mSendFrame;
  private static String mFileTag = "";
  private String mPatternNo = "";
  private String curConfigTlv = "";
  private boolean mReaderStatus = false;
  private boolean mPatternStatus = false;
  private boolean mIsSettingCleared = true;
  private boolean mIsModeSwitched = false;
  private static boolean mTestStatus = false;
  private TagOperation mTagOp;
  private TextView mTextView;
  private Spinner mSpinnerPattern;
  private Switch mCdlSw;
  private Switch mTestSw;
  private CheckBox mCECb;
  private Button mBtnExit;
  private static Bundle mExtras = null;
  private Map mConfigParams;
  private enum TESTMODE {
    MODE_UNKNOWN,
    MODE_READER,
    MODE_CARDEMULATION,
    MODE_LLCP
  }

  /*
   * Automation Test Related Code for working with Intents on different NFC Forum Test Tools
   */
  public final String BUNDLED_PARAMS_PATTERN_KEY = "pattern";
  public final String BUNDLED_PARAMS_HCE_MODE_KEY = "hce_mode";
  public final String BUNDLED_PARAMS_CONN_DEV_LIMIT = "swcdl";
  public final String BUNDLED_PARAMS_TEST_MODE = "test_mode";
  public final String BUNDLED_PARAMS_EXIT = "exit";
  private TESTMODE mTestMode = TESTMODE.MODE_UNKNOWN;
  static final int ENABLE_READER_MODE   = 1;
  static final int DISABLE_READER_MODE  = 2;
  static final int ENABLE_CE_MODE       = 3;

  private DtaApplicationHandler mHandler = new DtaApplicationHandler();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    Log.i(TAG, "onCreate");
    mConfigParams = new HashMap();
    mConfigParams.put("NFCC_CONFIG_CONTROL", "8501");
    mConfigParams.put("PF_BIT_RATE", "1801");
    mConfigParams.put("PACM_BIT_RATE", "6801");
    mConfigParams.put("PN_NFC_DEP_PSL", "2801");
    mConfigParams.put("PA_DEVICES_LIMIT", "0901");
    mConfigParams.put("PB_DEVICES_LIMIT", "1401");
    mConfigParams.put("PF_DEVICES_LIMIT", "1A01");
    mConfigParams.put("PV_DEVICES_LIMIT", "2F01");
    mConfigParams.put("LB_FWI_ADC_FO", "3C01");
    mConfigParams.put("LI_A_RATS_TC1", "5C01");
    mConfigParams.put("LA_NFCID1", "3304");
    mConfigParams.put("LB_NFCID0", "3904");
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mTagOp = new TagOperation();
    mExtras = new Bundle();
    mExtras.putInt(NfcAdapter.EXTRA_READER_PRESENCE_CHECK_DELAY,
                   10 * 60 * 1000);
    IntentFilter filter =
        new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
    registerReceiver(mReceiver, filter);

    mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
    if (mNfcAdapter != null) {
      mNfcCardEmulation = CardEmulation.getInstance(mNfcAdapter);
      mNfcFCardEmulation = NfcFCardEmulation.getInstance(mNfcAdapter);
      ComponentName hceService =
              new ComponentName(getPackageName(), NfcCardService.class.getName());
      if (!mNfcCardEmulation.isDefaultServiceForCategory(
              hceService, CardEmulation.CATEGORY_PAYMENT)) {
        Intent intent = new Intent(CardEmulation.ACTION_CHANGE_DEFAULT);
        intent.putExtra(CardEmulation.EXTRA_CATEGORY,
                CardEmulation.CATEGORY_PAYMENT);
        intent.putExtra(CardEmulation.EXTRA_SERVICE_COMPONENT, hceService);
        startActivityForResult(intent, 0);
      } else {
        Log.i(TAG, "HCE already default service!");
      }

      ComponentName hceFService =
              new ComponentName(getPackageName(), NfcFCardService.class.getName());
      if (!mNfcCardEmulation.isDefaultServiceForCategory(
              hceFService, CardEmulation.CATEGORY_PAYMENT)) {
        Intent intent = new Intent(CardEmulation.ACTION_CHANGE_DEFAULT);
        intent.putExtra(CardEmulation.EXTRA_CATEGORY,
                CardEmulation.CATEGORY_PAYMENT);
        intent.putExtra(CardEmulation.EXTRA_SERVICE_COMPONENT, hceFService);
      } else {
        Log.i(TAG, "HCEF already default service!");
      }
    }

    String value = getProperty(SYSPROP_DEBUG);
    mDebug = (!(value.isEmpty()) || value.equals("true"));
    if (mDebug) {
      initFrameDebugThread();
      mFrameDebugThread.start();
    }

    mSpinnerPattern = findViewById(R.id.spinner_pattern);
    mCdlSw = findViewById(R.id.switch_cdl);
    mCECb = findViewById(R.id.checkBox_ce);
    mTestSw = findViewById(R.id.switch_test);
    mTestSw.setOnCheckedChangeListener(
        new CompoundButton.OnCheckedChangeListener() {
          @Override
          public void onCheckedChanged(CompoundButton compoundButton,
                                       boolean isChecked) {
            mTestStatus = isChecked;
            if (isChecked) {
              mSpinnerPattern.setEnabled(false);
              mCdlSw.setEnabled(false);
              mBtnExit.setEnabled(false);
              mCECb.setEnabled(false);
              mPatternNo = mSpinnerPattern.getSelectedItem().toString();
              Log.i(TAG, "Selected Pattern No. " + mPatternNo);
              validatePatternNo(mPatternNo);
              if (mTestMode != TESTMODE.MODE_UNKNOWN) {
                String configTlvs = generateConfigTlv();
                mIsSettingCleared = false;
                if (!configTlvs.isEmpty()) {
                  setProperty(SYSPROP_CONFIGTLV, configTlvs);
                  String status = "true";
                  if (mTestMode == TESTMODE.MODE_READER) {
                    if (mPatternNo.equals("0000")) {
                      status = "false";
                    }
                    setProperty(SYSPROP_SKIPNDEFREAD, status);
                  }
                  setNfcDisable();
                  mPatternStatus = true;
                  setNfcEnable();
                } else {
                  String msg = "NFC ON";
                  if (mTestMode == TESTMODE.MODE_READER) {
                    Message hmsg = mHandler.obtainMessage();
                    hmsg.what = ENABLE_READER_MODE;
                    mHandler.sendMessage(hmsg);
                    msg += " and Reader Mode is active";
                  } else if (mTestMode == TESTMODE.MODE_CARDEMULATION) {
                    if (mIsModeSwitched) {
                      Message hmsg = mHandler.obtainMessage();
                      hmsg.what = DISABLE_READER_MODE;
                      mHandler.sendMessage(hmsg);
                      hmsg.what = ENABLE_CE_MODE;
                      mHandler.sendMessage(hmsg);
                    }
                    msg += " and Card Emulation is active";
                  }
                  mTextView.setText(msg);
                }
              } else {
                mTestSw.setChecked(false);
              }
            } else {
              clearSettings();
            }
          }
        });

    mTextView = findViewById(R.id.textView_nfcinfo);
    String msg = "NFC OFF";
    if (mNfcAdapter != null) {
      if (mNfcAdapter.isEnabled()) {
        msg = "NFC ON";
      } else {
        mSpinnerPattern.setEnabled(false);
        mCdlSw.setEnabled(false);
        mCECb.setEnabled(false);
        mTestSw.setEnabled(false);
      }
    }
    mTextView.setText(msg);

    mBtnExit = findViewById(R.id.button_exit);
    mBtnExit.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        finish();
      }
    });

    try {
      PackageInfo pInfo =
          getPackageManager().getPackageInfo(getPackageName(), 0);
      TextView appInfo = findViewById(R.id.textView_appInfo);
      appInfo.setText("Version: " + pInfo.versionName);
    } catch (PackageManager.NameNotFoundException e) {
      e.printStackTrace();
    }

    /* Parse Intent on Create */
    this.onNewIntent(this.getIntent());
  }

  @Override
  protected void onStop() {
    Log.i(TAG, "onStop");
    Message hmsg = mHandler.obtainMessage();
    hmsg.what = DISABLE_READER_MODE;
    mHandler.sendMessage(hmsg);
    setProperty(SYSPROP_CONFIGTLV, "");
    setProperty(SYSPROP_SKIPNDEFREAD, "false");
    if (!mPatternNo.isEmpty()) {
      setNfcDisable();
      setNfcEnable();
      try {
        Thread.sleep(300);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    super.onStop();
    if (mNfcAdapter != null) {
      mNfcAdapter.disableForegroundDispatch(this);
      if (mNfcAdapter.isEnabled()) {
        boolean ceFStatus = mNfcFCardEmulation.disableService(this);
        Log.i(TAG, "disableNfcFService status: " + ceFStatus);
      }
    }
  }

  @Override
  public void onStart() {
    Log.i(TAG, "onStart");
    super.onStart();
    IntentFilter tagDetected =
            new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
    IntentFilter ndefDetected =
            new IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED);
    IntentFilter techDetected =
            new IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED);
    IntentFilter[] nfcIntentFilter =
            new IntentFilter[]{techDetected, tagDetected, ndefDetected};
    PendingIntent pendingIntent = PendingIntent.getActivity(
            this, 0,
            new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP),
            PendingIntent.FLAG_IMMUTABLE);
    if (mNfcAdapter != null) {
      mNfcAdapter.enableForegroundDispatch(this, pendingIntent, nfcIntentFilter,
              null);
    }
  }

  @Override
  public void onResume() {
    Log.i(TAG, "onResume");
    super.onResume();
    if (mNfcAdapter != null) {
      if (!mNfcAdapter.isEnabled()) {
        mSpinnerPattern.setEnabled(false);
        mCdlSw.setEnabled(false);
        mCECb.setEnabled(false);
        mTestSw.setEnabled(false);
        mTextView.setText("NFC OFF");
      }
    }
  }

  @Override
  public void onPause() {
    Log.i(TAG, "onPause");
    super.onPause();
  }

  @Override
  protected void onNewIntent(Intent intent) {
    super.onNewIntent(intent);
    String action = intent.getAction();
    Log.i(TAG, "Intent received is " + action);

    Bundle bundledParams = intent.getExtras();
    if (bundledParams != null) {
      /* Handle DP & Tag Operations commands received from AutomationTest */
      handleAutomaTestDPCommands(bundledParams);
    }
  }

  /**
   * DP & Tag Operations Implementation for Automation
   */
  private void handleAutomaTestDPCommands(Bundle bundledParams) {
    Log.d(TAG, "In handleAutomaTestDPCommands");

    if (bundledParams.containsKey(BUNDLED_PARAMS_PATTERN_KEY)) {
      Log.d("Received Pattern Number", bundledParams.getString(BUNDLED_PARAMS_PATTERN_KEY));

      String DpPatternStr = bundledParams.getString(BUNDLED_PARAMS_PATTERN_KEY);
      try {
        Integer DpPatternInt = Integer.parseInt(DpPatternStr, 16);
        if (DpPatternInt >= 0 && DpPatternInt <= 15) {
          Log.d(TAG, "DP Pattern number set");
          this.mSpinnerPattern.setSelection(DpPatternInt);
        } else if (DpPatternInt == 17) {
          Log.d(TAG, "DP Pattern number 11 set");
          this.mSpinnerPattern.setSelection(16);
        } else if (DpPatternInt == 18) {
          Log.d(TAG, "DP Pattern number 12 set");
          this.mSpinnerPattern.setSelection(17);
        } else if (DpPatternInt == 33) {
          Log.d(TAG, "DP Pattern number 21 set");
          this.mSpinnerPattern.setSelection(18);
        } else if (DpPatternInt == 49) {
          Log.d(TAG, "DP Pattern number 31 set");
          this.mSpinnerPattern.setSelection(19);
        } else {
          Log.d(TAG, "DP default Pattern number set");
          this.mSpinnerPattern.setSelection(0);
        }
      } catch (NumberFormatException e) {
        Log.d(TAG, "Number format exception occurred during conversion");
      }
    }
    if (bundledParams.containsKey(BUNDLED_PARAMS_TEST_MODE)) {
      Log.d(TAG, "Test mode: " + bundledParams.getString(BUNDLED_PARAMS_TEST_MODE));
      this.mTestSw.setChecked("true".equals(bundledParams.getString(BUNDLED_PARAMS_TEST_MODE)));
    }

    if (bundledParams.containsKey(BUNDLED_PARAMS_CONN_DEV_LIMIT)) {
      Log.d(TAG, "Conn dev val: " + bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT));
      this.mCdlSw.setChecked(
              "true".equals(bundledParams.getString(BUNDLED_PARAMS_CONN_DEV_LIMIT)));
    }

    if (bundledParams.containsKey(BUNDLED_PARAMS_HCE_MODE_KEY)) {
      Log.d(TAG, "HCE Mode : " + bundledParams.getString(BUNDLED_PARAMS_HCE_MODE_KEY));
      this.mCECb.setChecked("true".equals(bundledParams.getString(BUNDLED_PARAMS_HCE_MODE_KEY)));
    }

    if (bundledParams.containsKey(BUNDLED_PARAMS_EXIT)) {
      Log.d(TAG, "Exit command received");
      finish();
    }
  }

  @Override
  protected void onDestroy() {
    Log.i(TAG, "onDestroy");
    unregisterReceiver(mReceiver);
    if (mFrameDebugThread != null) {
      synchronized (mFrameDebugThread) {
        mDebugThreadExit = true;
        Log.i(TAG, "Notify");
        mFrameDebugThread.notify();
      }
    }
    clearSettings();
    super.onDestroy();
  }

  private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
    @Override
    public void onReceive(Context context, Intent intent) {
      final String action = intent.getAction();
      if (action.equals(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED)) {
        final int state = intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE,
                                             NfcAdapter.STATE_OFF);
        Log.i(TAG, "Receiver:  ACTION_ADAPTER_STATE_CHANGED " + state);
        switch (state) {
        case NfcAdapter.STATE_OFF:
          mTextView.setText("NFC OFF");
          break;
        case NfcAdapter.STATE_TURNING_OFF:
        case NfcAdapter.STATE_TURNING_ON:
          break;
        case NfcAdapter.STATE_ON:
          if (mPatternStatus) {
            Message hmsg = mHandler.obtainMessage();
            mPatternStatus = false;
            if (!mPatternNo.isEmpty()) {
              String msg = "NFC ON";
              try {
                Thread.sleep(300);
              } catch (InterruptedException e) {
                e.printStackTrace();
              }
              if (mTestMode == TESTMODE.MODE_READER) {
                hmsg.what = ENABLE_READER_MODE;
                mHandler.sendMessage(hmsg);
                msg += " and Reader Mode is active";
              } else if (mTestMode == TESTMODE.MODE_CARDEMULATION) {
                hmsg.what = ENABLE_CE_MODE;
                mHandler.sendMessage(hmsg);
                msg += " and Card Emulation is active";
              }
              mTextView.setText(msg);
            }
          }
          break;
        }
      }
    }
  };

  private void setProperty(String key, String value) {
    try {
      Class SystemProperties = Class.forName("android.os.SystemProperties");
      Log.d(TAG, "setProperty: " + key + " = " + value);
      Method set =
          SystemProperties.getMethod("set", String.class, String.class);
      set.invoke(SystemProperties, key, value);
    } catch (Exception e) {
      Log.d(TAG, "setProperty exception");
      e.printStackTrace();
    }
  }

  private static String getProperty(String key) {
    String value = "";
    try {
      Class SystemProperties = Class.forName("android.os.SystemProperties");
      Method get = SystemProperties.getMethod("get", String.class);
      value = (String)get.invoke(SystemProperties, key);
      Log.d(TAG, "getProperty: " + key + " = " + value);
    } catch (Exception e) {
      Log.d(TAG, "getProperty exception");
      e.printStackTrace();
    }
    return value;
  }

  private void setNfcEnable() {
    boolean status = false;
    if (mNfcAdapter != null) {
      try {
        Class NfcManager = Class.forName(mNfcAdapter.getClass().getName());
        Method setEnable = NfcManager.getDeclaredMethod("enable");
        setEnable.setAccessible(true);
        status = (Boolean) setEnable.invoke(mNfcAdapter);
      } catch (Exception e) {
        Log.d(TAG, "setNfcEnable exception");
        e.printStackTrace();
      }
    }
    Log.d(TAG, "setNfcEnable Status: " + status);
  }

  private void setNfcDisable() {
    boolean status = false;
    if (mNfcAdapter != null) {
      try {
        Class NfcManager = Class.forName(mNfcAdapter.getClass().getName());
        Method setDisable = NfcManager.getDeclaredMethod("disable");
        setDisable.setAccessible(true);
        status = (Boolean) setDisable.invoke(mNfcAdapter);
      } catch (Exception e) {
        Log.d(TAG, "setNfcDisable exception");
        e.printStackTrace();
      }
    }
    Log.d(TAG, "setNfcDisable Status: " + status);
  }

  private void enableReaderMode() {
    if (mNfcAdapter != null) {
      mTagOp.setPatternNo(mPatternNo);
      int readerFlags = generateReaderFlags();
      Log.i(TAG, "enableReaderMode Flag: " + readerFlags);
      mNfcAdapter.enableReaderMode(this, new NfcAdapter.ReaderCallback() {
        @Override
        public void onTagDiscovered(final Tag tag) {
          Log.i(TAG, "***** Tag is detected *****\n");
          runOnUiThread(new Runnable() {
            @Override
            public void run() {
              if (mTestStatus) {
                Toast
                        .makeText(getApplicationContext(), "Tag detected",
                                Toast.LENGTH_SHORT)
                        .show();
                mTagOp.processDetectedTag(tag);
                listenTagRemoved(tag);
                Toast
                        .makeText(getApplicationContext(), "Test Execution Completed",
                                Toast.LENGTH_SHORT)
                        .show();
              } else {
                Toast
                        .makeText(getApplicationContext(), "Test is not running",
                                Toast.LENGTH_SHORT)
                        .show();
              }
            }
          });
        }
      }, readerFlags, mExtras);
      mReaderStatus = true;
    }
    Log.i(TAG, "enableReaderMode status: " + mReaderStatus);
  }

  private void disableReaderMode() {
    if (mReaderStatus && (mNfcAdapter != null) && mNfcAdapter.isEnabled()) {
      mNfcAdapter.disableReaderMode(this);
      mReaderStatus = false;
      Log.i(TAG, "ReaderMode status: " + mReaderStatus);
    }
  }

  private void enableListenService() {
    if (mNfcAdapter != null) {
      if (mNfcAdapter.isEnabled()) {
        ComponentName hceService =
                new ComponentName(getPackageName(), NfcCardService.class.getName());
        boolean ceStatus =
                mNfcCardEmulation.setPreferredService(this, hceService);
        Log.i(TAG, "HCE setPreferredService status: " + ceStatus);
        ComponentName hceFService = new ComponentName(
                getPackageName(), NfcFCardService.class.getName());
        ceStatus = mNfcFCardEmulation.enableService(this, hceFService);
        Log.i(TAG, "enableNfcFService status: " + ceStatus);
        ceStatus = mNfcCardEmulation.setPreferredService(this, hceFService);
        Log.i(TAG, "HCEF setPreferredService status: " + ceStatus);
      }
    }
  }

  private void listenTagRemoved(final Tag tag) {
    if (mNfcAdapter != null) {
      mNfcAdapter.ignore(tag, 100, new NfcAdapter.OnTagRemovedListener() {
        @Override
        public void onTagRemoved() {
          Log.i(TAG, "***** Tag is removed *****");
          mTagOp.clearData();
        }
      }, null);
    }
  }

  private int generateReaderFlags() {
    int flags = NfcAdapter.FLAG_READER_NFC_A | NfcAdapter.FLAG_READER_NFC_B |
                NfcAdapter.FLAG_READER_NFC_F | NfcAdapter.FLAG_READER_NFC_V;

    if (mPatternNo.equals("0000")) {
      flags |= NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK;
    } else if (mPatternNo.equals("0007") || mPatternNo.equals("0009")) {
      flags &= ~NfcAdapter.FLAG_READER_NFC_B;
    }

    return flags;
  }

  private String generateConfigTlv() {
    String configTlv = "";
    configTlv +=
        mConfigParams.get("NFCC_CONFIG_CONTROL") + "00" + TLV_DELIMITER;
    if (mTestMode == TESTMODE.MODE_READER) {
      String devLimit = mCdlSw.isChecked() ? "02" : "01";
      configTlv += mConfigParams.get("PA_DEVICES_LIMIT") + devLimit + TLV_DELIMITER;
      configTlv += mConfigParams.get("PB_DEVICES_LIMIT") + devLimit + TLV_DELIMITER;
      configTlv += mConfigParams.get("PF_DEVICES_LIMIT") + devLimit + TLV_DELIMITER;
      configTlv += mConfigParams.get("PV_DEVICES_LIMIT") + devLimit + TLV_DELIMITER;
    } else if (mTestMode == TESTMODE.MODE_CARDEMULATION) {
      configTlv += mConfigParams.get("LA_NFCID1") + "01020304" + TLV_DELIMITER;
      configTlv += mConfigParams.get("LI_A_RATS_TC1") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("LB_FWI_ADC_FO") + "81" + TLV_DELIMITER;
      configTlv += mConfigParams.get("LB_NFCID0") + "01020304" + TLV_DELIMITER;
    }

    switch (mPatternNo) {
    case "0000":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "00" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    case "0001":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    case "0011":
    case "0021":
    case "0031":
    case "0012":
      configTlv = "";
      break;
    case "0002":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    case "0003":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "00" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "00";
      break;
    case "0004":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "00";
      break;
    case "0005":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "00" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    case "0006":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "02";
      break;
    case "0007":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "00";
      break;
    case "0008":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "02";
      break;
    case "0009":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "01";
      break;
    case "000A":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PACM_BIT_RATE") + "01";
      break;
    case "000B":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01";
      break;
    case "1200":
    case "1240":
    case "1280":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "01" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    case "1201":
    case "1241":
    case "1281":
      configTlv += mConfigParams.get("PF_BIT_RATE") + "02" + TLV_DELIMITER;
      configTlv += mConfigParams.get("PN_NFC_DEP_PSL") + "01";
      break;
    default:
      configTlv = "";
      Log.i(TAG, "Invalid Pattern No.");
    }

    if (curConfigTlv.equals(configTlv)) {
      configTlv = "";
    } else {
      curConfigTlv = configTlv;
    }

    return configTlv;
  }

  private void validatePatternNo(final String pattern) {
    TESTMODE newTestMode = TESTMODE.MODE_UNKNOWN;
    mIsModeSwitched = false;

    if (mCECb.isChecked() &&
        (pattern.equals("0000") || pattern.equals("0001"))) {
      mCdlSw.setChecked(false);
      newTestMode = TESTMODE.MODE_CARDEMULATION;
    } else if (pattern.equals("0000") || pattern.equals("0001") ||
               pattern.equals("0011") || pattern.equals("0021") ||
               pattern.equals("0031") || pattern.equals("0002") ||
               pattern.equals("0012") || pattern.equals("0003") ||
               pattern.equals("0004") || pattern.equals("0005") ||
               pattern.equals("0006") || pattern.equals("0007") ||
               pattern.equals("0008") || pattern.equals("0009") ||
               pattern.equals("000A") || pattern.equals("000B")) {
      newTestMode = TESTMODE.MODE_READER;
    } else if (pattern.equals("1200") || pattern.equals("1201") ||
               pattern.equals("1240") || pattern.equals("1241") ||
               pattern.equals("1280") || pattern.equals("1281")) {
      newTestMode = TESTMODE.MODE_LLCP;
    }

    if ((mTestMode != TESTMODE.MODE_UNKNOWN) && (newTestMode != mTestMode)) {
      mIsModeSwitched = true;
    }
    mTestMode = newTestMode;
  }

  private void clearSettings() {
    if (!mIsSettingCleared) {
      Log.i(TAG, "clearSettings");
      String msg = "NFC OFF";
      mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
      if ((mNfcAdapter != null) && mNfcAdapter.isEnabled()) {
        msg = "NFC ON";
      }
      mTextView.setText(msg);
      mBtnExit.setEnabled(true);
      mSpinnerPattern.setEnabled(true);
      mCdlSw.setEnabled(true);
      mCECb.setEnabled(true);
      mIsSettingCleared = true;
    } else {
      Log.i(TAG, "Unable to clearSettings");
    }
  }

  public static boolean getTestStatus() { return mTestStatus; }

  final class DtaApplicationHandler extends Handler {
    @Override
    public void handleMessage(Message hmsg) {
      switch (hmsg.what) {
        case ENABLE_READER_MODE: {
          enableReaderMode();
          break;
        }

        case DISABLE_READER_MODE: {
          disableReaderMode();
          break;
        }

        case ENABLE_CE_MODE: {
          enableListenService();
          break;
        }

        default:
          Log.e(TAG, "Unknown message received");
          break;
      }
    }
  }

  public static void printPacket(final String fileTag, byte[] recvPacket,
                                 byte[] sendPacket) {
    if (mDebug) {
      synchronized (mFrameDebugThread) {
        mFileTag = fileTag;
        mRecvFrame = recvPacket.clone();
        mSendFrame = sendPacket.clone();
        Log.i(TAG, "Notify");
        mFrameDebugThread.notify();
      }
    }
  }

  private static void initFrameDebugThread() {
    if (mFrameDebugThread == null) {
      Log.i(TAG, "initFrameDebugThread");
      mFrameDebugThread = new Thread() {
        public void run() {
          Log.i(TAG, "FrameDebugThread started");
          mDebugThreadExit = false;
          synchronized (mFrameDebugThread) {
            while (!mDebugThreadExit) {
              try {
                Log.i(TAG, "Waiting");
                wait();
                if (mDebugThreadExit) {
                  break;
                }
                String msg = "";
                Log.i(mFileTag, "Received frame: ");
                for (int i = 0; i < mRecvFrame.length; i++) {
                  msg += String.format("0x%02X ", mRecvFrame[i]);
                  if ((i != 0) && ((i % 256) == 0)) {
                    Log.i(mFileTag, msg);
                    msg = "";
                  }
                }
                Log.i(mFileTag, msg);
                msg = "";
                Log.i(mFileTag, "Send frame: ");
                for (int i = 0; i < mSendFrame.length; i++) {
                  msg += String.format("0x%02X ", mSendFrame[i]);
                  if ((i != 0) && ((i % 256) == 0)) {
                    Log.i(mFileTag, msg);
                    msg = "";
                  }
                }
                Log.i(mFileTag, msg);
              } catch (InterruptedException e) {
                e.printStackTrace();
              }
            }
          }
          Log.i(TAG, "FrameDebugThread exited");
          mFrameDebugThread = null;
        }
      };
    }
  }
}
