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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.phdtaui.utils.PhUtilities;

public class PhLogCatActivity extends Activity {

    private Button backButton;
    private TextView logText;
    private TextView versionNumber;
    ProgressDialog consoleProgressDialog;

    @Override
    public void onResume() {
        super.onResume();
        if (PhUtilities.exitValue.equals("exit clicked")) {
            Toast.makeText(getApplicationContext(), PhUtilities.exitValue,
                    Toast.LENGTH_LONG).show();
            finish();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /**
         * Loading the Log Cat or Console Layout
         */
        setContentView(R.layout.ph_logcat);
        new LogCatTask().execute();
        consoleProgressDialog = ProgressDialog.show(PhLogCatActivity.this, "Console", "Loading Console. Please Wait... ", true, true);
        new Thread(new Runnable() {

            @Override
            public void run() {
                try{
                    Thread.sleep(2000);
                } catch(Exception e){

                }
                consoleProgressDialog.dismiss();
            }
        }).start();
    }

      class LogCatTask extends AsyncTask<Void, Void, Void> implements OnClickListener{
        StringBuilder log ;
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            /**
             * Calling Button id's
             */
            backButton = (Button) findViewById(R.id.back);
                     /**
             * Calling Text View id's
             */
            logText = (TextView) findViewById(R.id.log);

            /**
             * Registering the On Click listener
             */
            // START - get version number from manifest file
            //versionNumber = (TextView) findViewById(R.id.version_number);

            //versionNumber.setText("UI Version: " + "05.05");
            //versionNumber.setTextSize(12);
            // END - get version number from manifest file

            /**
             * Appending the Log Cat or Console Text
             */
        }
        @Override
        protected Void doInBackground(Void... params) {
                     try {
                Process logcatProgress = Runtime.getRuntime().exec("logcat -d");
                BufferedReader bufferedReader = new BufferedReader(
                        new InputStreamReader(logcatProgress.getInputStream()));
                log= new StringBuilder();
                String line = "";
                while ((line = bufferedReader.readLine()) != null) {
                    log.append(line);
                }
                         } catch (IOException e) {
            }
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            logText.setText(log.toString());
            backButton.setOnClickListener(this);
        }
        @Override
        public void onClick(View v) {
            finish();
        }
    }
}
