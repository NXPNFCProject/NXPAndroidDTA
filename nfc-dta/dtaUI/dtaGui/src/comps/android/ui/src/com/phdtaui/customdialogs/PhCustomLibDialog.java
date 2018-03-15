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
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;
import com.phdtaui.utils.PhUtilities;

public class PhCustomLibDialog extends Dialog implements
        android.view.View.OnClickListener {

    private String libErrorMSG;
    private Context mContext;
    private Button yesExit, noExit;

    /**
     *
     * @param Constructor used for the Base Context or Parent Activity
     */
    public PhCustomLibDialog(Context context, String libErrorMSG) {
        super(context);
        this.libErrorMSG = libErrorMSG;
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
        setContentView(R.layout.ph_custom_lib_dialog);
        setCanceledOnTouchOutside(false);

        TextView errorTextView = (TextView) findViewById(R.id.textView1);
        /**
         * Show the pop up for Loading library
         */
        if (libErrorMSG.equals("NO_LIBRARY_FILE")) {
            errorTextView.setText(mContext.getResources().getString(
                    R.string.lib_error));
        } else if (libErrorMSG.equals("PATH_MISMATCH")) {
            errorTextView.setText(mContext.getResources().getString(
                    R.string.path_error));
        } else if (libErrorMSG.equals("VERSION_INFO")) {
            errorTextView.setText(mContext.getResources().getString(
                    R.string.version_error));
        }

        /**
         *Yes and No  Button id's declared here
         */
        yesExit = (Button) findViewById(R.id.yesexit);
        noExit = (Button) findViewById(R.id.noexit);
        /**
         * Registering On Click Listener  for Yes and No
         */
        yesExit.setOnClickListener(this);
        noExit.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        /*** if user press yes exit the application else dismiss the dialog */
        if (v.getId() == R.id.yesexit) {
            dismiss();
            android.os.Process.killProcess(android.os.Process.myPid());
            Log.d(PhUtilities.UI_TAG, "User pressed the Yes button "
                    + (v.getId() == R.id.yesexit));

        } else if (v.getId() == R.id.noexit) {
            Log.d(PhUtilities.UI_TAG, "User pressed the No button "
                    + (v.getId() == R.id.noexit));
            dismiss();
        }
    }
}
