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
