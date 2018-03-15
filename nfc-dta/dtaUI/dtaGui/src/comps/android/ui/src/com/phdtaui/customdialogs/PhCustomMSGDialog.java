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

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.text.Selection;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;
import com.phdtaui.utils.PhUtilities;

public class PhCustomMSGDialog extends Dialog implements
        android.view.View.OnClickListener {

    private Context mContext;

    private EditText patternNumberEditText;
    PhCustomPopValidation customPopValidation;
    /**
     *
     * @param Constructor used for the Base Context or Parent Activity
     */
    public PhCustomMSGDialog(Context context) {
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
        setContentView(R.layout.ph_custom_msg_dialog_layout);
        setCanceledOnTouchOutside(false);

        /*** Dismiss         */
        Button dismissBtn = (Button) findViewById(R.id.ok_btn);
        dismissBtn.setOnClickListener(this);
             /**
         * Declared Edit Text id's Test Case ,Status,Pattern Number Rfu1,Rfu2
         */
        patternNumberEditText = (EditText) findViewById(R.id.pattern_no_edit_txt);

        patternNumberEditText.addTextChangedListener(new MaxLengthWatcher(4));
        /** * Hide soft keyboard of all edit text */
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

    /** Edit Text validation For Custom Pattern Number*/

    public class MaxLengthWatcher implements TextWatcher {

        private int maxLen = 0;

        public MaxLengthWatcher(int maxLen) {
            this.maxLen = maxLen;
        }

        public void afterTextChanged(Editable arg0) {

        }

        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,int arg3) {

        }

        public void onTextChanged(CharSequence chars, int start, int before,int count) {

            Editable editable = patternNumberEditText.getText();
            int len = editable.length();
            String checkValidite = null;
            checkValidite = chars.toString();
            Log.d(PhUtilities.UI_TAG, "Get pattern Number checkValidite "+ checkValidite);

            Log.d(PhUtilities.UI_TAG, "Get pattern Number length "
                    + checkValidite.length());
            if ((checkValidite.length() > 4)
                    && (checkValidite.matches(("[a-fA-F0-9]+")))) {
                Log.d(PhUtilities.UI_TAG,
                        "" + (checkValidite.matches(("[a-fA-F0-9]+"))));
                customPopValidation = new PhCustomPopValidation(mContext,
                        mContext.getResources().getString(R.string.warning));
                customPopValidation.show();
                Log.d(PhUtilities.UI_TAG,
                        "Entered the correct pattern number but it excedes 4 characters");
            } else if ((checkValidite.matches(("[a-fA-F0-9]+")))) {
                Log.d(PhUtilities.UI_TAG,
                        "" + (checkValidite.matches(("[a-fA-F0-9]+"))));

                Log.d(PhUtilities.UI_TAG, "Entered the correct pattern number");
            } else if ((checkValidite.length() != 0)) {
                customPopValidation = new PhCustomPopValidation(mContext,
                        mContext.getResources().getString(
                                R.string.warning_for_wrong_key));
                Log.d(PhUtilities.UI_TAG,
                        "extractText"
                                + checkValidite.substring(0,
                                        checkValidite.length() - 1));
                patternNumberEditText.setText(checkValidite.substring(0,
                        checkValidite.length() - 1));
                customPopValidation.show();
                Log.d(PhUtilities.UI_TAG, "Entered the wrong pattern number");
            } else {
                Log.d(PhUtilities.UI_TAG, "Entered the else wrong pattern number");
            }

            /*** Restricting edit text to maximum 4 characters */
            if (len > maxLen) {
                int selEndIndex = Selection.getSelectionEnd(editable);
                String str = editable.toString();
                String newStr = str.substring(0, maxLen);
                patternNumberEditText.setText(newStr);
                editable = patternNumberEditText.getText();
                int newLen = editable.length();
                if (selEndIndex > newLen) {
                    selEndIndex = editable.length();
                }
                Selection.setSelection(editable, selEndIndex);
            }
        }
    }
}
