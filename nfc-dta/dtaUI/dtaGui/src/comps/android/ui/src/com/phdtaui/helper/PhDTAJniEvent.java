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

public class PhDTAJniEvent
{
    public enum EventType
    {
        PHDTALIB_TEST_CASE_EXEC_STARTED,
        PHDTALIB_TEST_CASE_EXEC_COMPLETED
    }

    public EventType evtType;

    public PhDTAJniEvent (EventType eEvent)
    {
        this.evtType = eEvent;
    }
}
