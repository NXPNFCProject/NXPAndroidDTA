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
