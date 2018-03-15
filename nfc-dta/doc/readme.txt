
########################################################################

1. File to be Modified is "dtaConfig.mk"

2. By default release package is configured to build for PN557/PN81T
   with ANDROID-O & GO Middleware.

3. Refer to the below table for furhter details.

4. How to read the table: Check Android Version, set the flags based on
   the CHIP (PN81T / PN80T)
########################################################################

________________________________________________________________________
                    |       FLAG            |        CHIP     |
---------------------------------------------------------------
                    |                       | PN81T  | PN80T  |
---------------------------------------------------------------
Android - GO (8.1)  |                       |        |        |
        &           |FLAG_NXP_HAL_EXTNS     | true   | true   |
Android - O (8.0)   |THREAD_PRIO_SUPPORT    | false  | false  |
                    |ANDROID_O              | false  | false  |
                    |AOSP_MASTER            | true   | true   |
                    |NCI_2_0                | true   | false  |
                    |NFC_NXP_CHIP_TYPE      | PN557  | PN553  |
                    |                       |        |        |
---------------------------------------------------------------
Android - N/M/L/K   |                       |        |        |
                    |FLAG_NXP_HAL_EXTNS     |        | false  |
                    |THREAD_PRIO_SUPPORT    |        | true   |
                    |ANDROID_O              |        | false  |
                    |AOSP_MASTER            |        | false  |
                    |NCI_2_0                |        | false  |
                    |ANDROID_O_MR1          |        | false  |
                    |NFC_NXP_CHIP_TYPE      |        | PN553  |
                    |                       |        |        |
---------------------------------------------------------------
Android - Master    |                       |        |        |
                    |FLAG_NXP_HAL_EXTNS     | true   |        |
                    |THREAD_PRIO_SUPPORT    | false  |        |
                    |ANDROID_O              | true   |        |
                    |AOSP_MASTER            | true   |        |
                    |NCI_2_0                | true   |        |
                    |ANDROID_O_MR1          | true   |        |
                    |NFC_NXP_CHIP_TYPE      | PN557  |        |
                    |                       |        |        |
---------------------------------------------------------------
________________________________________________________________________
