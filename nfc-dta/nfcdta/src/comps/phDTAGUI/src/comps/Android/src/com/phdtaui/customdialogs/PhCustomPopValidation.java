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
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.mainactivity.R;

public class PhCustomPopValidation extends Dialog implements
        android.view.View.OnClickListener {

    private String mString;
    private Button yesExit;
    /**
     *
     * @param Constructor used for the Base Context or Parent Activity
     */
    public PhCustomPopValidation(Context context, String mString) {
        super(context);
        this.mString = mString;
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
        setContentView(R.layout.ph_custom_popup_validation);
        setCanceledOnTouchOutside(false);

        /**
         * Declared the TextView id's Custom Pop
         */
        TextView textView = (TextView) findViewById(R.id.textView1);
        textView.setText(mString);

        yesExit = (Button) findViewById(R.id.yesexit);
        yesExit.setOnClickListener(this);
    }
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.yesexit) {
            dismiss();
        }
    }

}
