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

package com.phdtaui.customdialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;
import com.phdtaui.utils.PhUtilities;

public class PhShowMSGDialog extends Dialog implements
        android.view.View.OnClickListener {

    private Context mContext;
    private EditText resultOrExecute, testCaseId, patternNumber,
            statusCardEmulation, rfu1, rfu2;
    /**
     *
     * @param Constructor used for the Base Context or Parent Activity
     */
    public PhShowMSGDialog(Context context) {
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
        setContentView(R.layout.ph_show_msg_dialog_layout);
        setCanceledOnTouchOutside(false);

        /**
         * Declared Edit Text id's
         */
        resultOrExecute = (EditText) findViewById(R.id.result_execute_edit_txt);
        testCaseId = (EditText) findViewById(R.id.test_case_cb_edit_txt);
        patternNumber = (EditText) findViewById(R.id.pattern_cb_edit_txt);
        statusCardEmulation = (EditText) findViewById(R.id.status_cb_edit_txt);
        rfu1 = (EditText) findViewById(R.id.rfu1_cb_edit_txt);
        rfu2 = (EditText) findViewById(R.id.rfu2_cb_edit_txt);

        resultOrExecute.setText(PhUtilities.executeOrResultBuff);
        testCaseId.setText(PhUtilities.testCaseIdBuff);
        patternNumber.setText(PhUtilities.patternNUmberBuff);

        /**
         * Set Text From Client and Server Pop Up
         */
        if (PhUtilities.cardEmulationModeBuff.equals("")) {
            Log.d(PhUtilities.UI_TAG, "" + PhUtilities.cardEmulationModeBuff);
            statusCardEmulation.setText("  ");
        } else {
            statusCardEmulation.setText(PhUtilities.cardEmulationModeBuff);
        }

        if (PhUtilities.rfu1Buff.equals("")) {
            rfu1.setText("  ");
        } else {
            rfu1.setText(PhUtilities.rfu1Buff);
        }
        if (PhUtilities.rfu2Buff.equals("")) {
            rfu2.setText("  ");
        } else {
            rfu2.setText(PhUtilities.rfu2Buff);
        }

        /**
         * Dismiss
         */
        Button dismissBtn = (Button) findViewById(R.id.ok_btn);
        dismissBtn.setOnClickListener(this);

        /**
         * Hide the keybap when not focus
         */
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {
        case R.id.ok_btn:
            dismiss();
            break;

        }

    }
}
