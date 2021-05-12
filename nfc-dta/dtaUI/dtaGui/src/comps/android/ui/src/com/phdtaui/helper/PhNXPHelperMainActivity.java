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

package com.phdtaui.helper;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.Date;
import java.util.zip.CRC32;
import java.util.zip.Checksum;

import com.phdtaui.utils.PhUtilities;

import android.util.Log;

public class PhNXPHelperMainActivity {

    public String sPatternnumber;
    public String sCertificationVersion;
    public String sTimeSlotNumberF;
    public String sConnectionDeviceLimit;
    public boolean dtaDebugFlag;

    public String getsPatternnumber() {
        Log.d(PhUtilities.UI_TAG, "Get Pattern Number = " + sPatternnumber);
        return sPatternnumber;
    }

    public void setsPatternnumber(String sPatternnumber) {
        this.sPatternnumber = sPatternnumber;
        Log.d(PhUtilities.UI_TAG, "Set Pattern Number = " + sPatternnumber);
    }

    public String getsCertificationVersion(){
        Log.d(PhUtilities.UI_TAG, "Get Certification Version = " + sCertificationVersion);
        return sCertificationVersion;
    }

    public void setsCertificationVersion(String sCertificationVersion){
        this.sCertificationVersion = sCertificationVersion;
        Log.d(PhUtilities.UI_TAG, "Set Certification Version = " + this.sCertificationVersion);
    }

    public String getsTimeSlotNumberF(){
        Log.d(PhUtilities.UI_TAG, "Get time slot number = " + sTimeSlotNumberF);
        return sTimeSlotNumberF;
    }

    public void setsTimeSlotNumberF(String sTimeSlotNumberF){
        this.sTimeSlotNumberF = sTimeSlotNumberF;
        Log.d(PhUtilities.UI_TAG, "Set time slot number = " + this.sTimeSlotNumberF);
    }

    public String getsConnectionDeviceLimit(){
        Log.d(PhUtilities.UI_TAG, "Get connection device limit = " + sConnectionDeviceLimit);
        return sConnectionDeviceLimit;
    }

    public void setsConnectionDeviceLimit(String sConnectionDeviceLimit){
        this.sConnectionDeviceLimit = sConnectionDeviceLimit;
        Log.d(PhUtilities.UI_TAG, "Set connection device limit = " + this.sConnectionDeviceLimit);
    }

    public boolean getdtaDebugFlag(){
        Log.d(PhUtilities.UI_TAG, "Get DTA debug flag = " + dtaDebugFlag);
        return dtaDebugFlag;
    }

    public void setdtaDebugFlag(boolean dtaDebugFlag){
        this.dtaDebugFlag = dtaDebugFlag;
        Log.d(PhUtilities.UI_TAG, "Set DTA debug flag = " + this.dtaDebugFlag);
    }
    /**
     * This method creates a new file with given file name if file already
     * exists in the specified directory then file writes data into it
     *
     * @param fileName
     *            String
     * @throws IOException
     */
    public void createFileWithGivenNameOnExternalStorageAndWriteLog(
            String fileName) throws IOException {
        File makeDir = new File("/sdcard/nxpdtalog/");
        if (!makeDir.exists()) {
            makeDir.mkdir();
        }
        File file = new File("/sdcard/nxpdtalog/" + fileName);
        if (!file.exists()) {
            file.createNewFile();
        }

        String timeStamp = "\n\r####################################### \n\r TIME STAMP: "
                + new Date().toString()
                + "\n\r#######################################\n\r";
        StringBuilder strLogTextToFile = new StringBuilder(timeStamp);
        strLogTextToFile.append(getLogCat());

        try {
            FileWriter fileWritter = new FileWriter(file, true);
            BufferedWriter bufferWritter = new BufferedWriter(fileWritter);
            bufferWritter.write(strLogTextToFile.toString());
            bufferWritter.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method creates file with given fileName in /sdCard directory
     *
     * @param fileName
     *            String
     * @throws IOException
     */
    public void createFileWithGivenNameOnsdCard(String fileName)
            throws IOException {
        File makeDir = new File("/sdcard/nxpdtalog/");
        if (!makeDir.exists()) {
            makeDir.mkdir();
        }
        File file = new File("/sdcard/nxpdtalog/" + fileName);
        if (!file.exists()) {
            file.createNewFile();
        }
    }

    /**
     * This method returns logcat until this application is invoked because
     * logcat -d is used it will exit once this application is invoked and
     * returns String Builder with whole logcat content
     *
     * @return strLogTextToFile StringBuilder
     * @throws IOException
     */
    protected StringBuilder getLogCat() throws IOException {
        Process logProcess = Runtime.getRuntime().exec("logcat -d");
        BufferedReader bufferReader = new BufferedReader(new InputStreamReader(
                logProcess.getInputStream()));

        StringBuilder strLogTextToFile = new StringBuilder();
        String line = "";
        while ((line = bufferReader.readLine()) != null) {
            strLogTextToFile.append(line);
        }
        return strLogTextToFile;
    }
    /**
     * This method generates CheckSum for given string, this is used to compare CRC
     * @param input
     * @return
     */
    public String getCRC32CheckSumForGivenString(String stringInput){
        Log.d(PhUtilities.UI_TAG, "String Recevied for CRC is "+stringInput);
        stringInput = stringInput.substring(2, (stringInput.length() - 1));
        Log.d(PhUtilities.UI_TAG, "String after removing headers for CRC is "+stringInput);
        // get bytes from string
        byte bytes[] = stringInput.getBytes();

        Checksum checksum = new CRC32();

        // update the current checksum with the specified array of bytes
        checksum.update(bytes, 0, bytes.length);

        // get the current checksum value
        String hexCRC = Long.toHexString(checksum.getValue()).toUpperCase();

        int hexCRCLength = hexCRC.length();
        String zeroString = "0"; //this is used when hex CRC has less than 8 characters to prefix that CRC with 0
        while(8 - hexCRCLength != 0){
            hexCRC = zeroString + hexCRC;
            hexCRCLength ++;
        }
        return hexCRC;
    }

    /**
     * This method appends all the input strings sent and appends them and returns a string
     * @param stringInputs String...
     * @return StringBuilder
     */
    public StringBuilder concatenateAllStrings(String... stringInputs){
        StringBuilder stringAfterAppend = new StringBuilder();
        for(String input : stringInputs){
            stringAfterAppend.append(input);
        }
        return stringAfterAppend;
    }

    /**
     * This method removes CRC field in the string received from LT / DUT and returns substring
     * The returned string is used to calculate CRC and then comapre for any possible data loss
     * @param input
     * @return StringBuilder
     */
    public StringBuilder getSubString(String inputMessage){
        String[] subStrings = inputMessage.split(",");
        StringBuilder stringToBeCheckedForCRC = new StringBuilder();
        System.out.println("Length : "+subStrings.length);
        int pos = 0;
        appendSubStrings(stringToBeCheckedForCRC, subStrings, pos);
        System.out.println("Actual Sub String: "+stringToBeCheckedForCRC.toString());
        return stringToBeCheckedForCRC;
    }
    /**
     * This method takes StringBuilder object and array strings(after splitting) and from which
     * position substrings has to start appending. It appends from starting substring(from pos)
     * to last but one substring(in order to remove CRC substring while calculating number of bytes)
     * @param stringToBeCheckedForCRC StringBuilder
     * @param subStrings String[]
     * @param pos int
     */
    public void appendSubStrings(StringBuilder stringToBeCheckedForCRC, String[] subStrings, int pos) {
        do{
            stringToBeCheckedForCRC.append(subStrings[pos]);
            stringToBeCheckedForCRC.append(",");
            pos++;
        }while((pos+1) < subStrings.length);
    }

    /**
     * This method calculates number of bytes for input string using character set UTF-8
     * @param calNumOfBytes String
     * @return numOfBytes int
     */
    public int getNumberOfBytes(String calNumOfBytes){
        try {
            Log.d(PhUtilities.UI_TAG, "String received for calculation of number of bytes" + calNumOfBytes);
            String[] subStrings = calNumOfBytes.split(",");
            StringBuilder actualString = new StringBuilder();
            appendSubStringsForGettingNumberOfBytes(actualString, subStrings, 2);
            return actualString.toString().getBytes("utf8").length;
        } catch (UnsupportedEncodingException ex) {
            return 0;
        }
    }

    /**
    * This method takes StringBuilder object and array strings(after splitting) and from which
    * position substrings has to start appending. It appends from starting substring(from pos)
    * to last but one substring(in order to remove CRC substring while calculating number of bytes)
    * @param stringToBeCheckedForCRC StringBuilder
    * @param subStrings String[]
    * @param pos int
    */
    public void appendSubStringsForGettingNumberOfBytes(StringBuilder stringToBeCheckedForCRC, String[] subStrings, int pos) {
        do{
            stringToBeCheckedForCRC.append(subStrings[pos]);
            pos++;
        }while((pos+1) < subStrings.length);
    }
}
