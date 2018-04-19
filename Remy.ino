//  Remy - Gate Controller
//  Copyright (C) 2018 Scott Finneran
//
//  Remy is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  Remy is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with Remy.  If not, see <http://www.gnu.org/licenses/>.

#include "project.h"

#undef REAL_BOARD
#ifdef REAL_BOARD
INT16U remotePin = 4;
INT16U pePin = 3;
INT16U relay2Pin = 2;
INT16U relay1Pin = 1;
INT16U switch1_3Pin = 13;
INT16U switch1_2Pin = 12;
INT16U switch1_1Pin = 11;

INT16U switch2_3Pin = 9;
INT16U switch2_2Pin = 8;
INT16U switch2_1Pin = 7;
#else
INT16U remotePin = 9;
INT16U pePin = 12;
INT16U relay2Pin = 2;
INT16U relay1Pin = 1;
INT16U switch1_3Pin = 3;
INT16U switch1_2Pin = 4;
INT16U switch1_1Pin = 5;

INT16U switch2_3Pin = 6;
INT16U switch2_2Pin = 7;
INT16U switch2_1Pin = 8;
#endif

enum STATE
{
    STATE_INVALID   = 0,
    STATE_CLOSED    = 1,
    STATE_CLOSING   = 2,
    STATE_OPENING   = 3,
    STATE_OPEN      = 4,
};

#define GATE_REVERSE_DELAY_MS (2000)

STATE gateState = STATE_OPEN;

INT16U mainLoopPeriodicity = 20;
INT16U loopCounter = 0;

bool remotePressDetected = false;
bool previousRemoteValue = true;

INT32U travelTimerStart = 0;
#ifdef REAL_BOARD
INT32U travelTimes[] = {4000, 8000, 12000, 16000, 20000, 24000, 28000, 32000};
#else
// Hacked version with no switch attached and only pull-ups being read
INT32U travelTimes[] = {4000, 8000, 12000, 16000, 20000, 24000, 28000, 4000};
#endif

void setup()
{

    // start serial connection
    Serial.begin(9600);

    // Signal from Merlin remote control receiver.
    // Open collector device is attached so provide pull-up
    pinMode(remotePin, INPUT_PULLUP);

    // Signal from photoelectric (magic eye) sensor
    // Open collector device is attached so provide pull-up
    pinMode(pePin, INPUT_PULLUP);

    // Motor relay control outputs
    pinMode(relay1Pin, OUTPUT);
    pinMode(relay2Pin, OUTPUT);

    // Config switch bank #1
    pinMode(switch1_3Pin, INPUT_PULLUP);
    pinMode(switch1_2Pin, INPUT_PULLUP);
    pinMode(switch1_1Pin, INPUT_PULLUP);

    // Config switch bank #1
    pinMode(switch2_3Pin, INPUT_PULLUP);
    pinMode(switch2_2Pin, INPUT_PULLUP);
    pinMode(switch2_1Pin, INPUT_PULLUP);

    previousRemoteValue = digitalRead(remotePin);
}

void loop()
{
    if ((loopCounter++ % 50) == 0)
    {
        showStatus();
    }

    delay(mainLoopPeriodicity);

    scanInputs();

    handleGateState();
}

void showStatus()
{

    Serial.print("Remy Version 1. State:");
    Serial.print(gateState);
    Serial.print(" :  ");
    Serial.print(digitalRead(relay1Pin), HEX);
    Serial.print(" :  ");
    Serial.print(digitalRead(relay2Pin), HEX);
    Serial.print(" :  ");
    Serial.print(Switch1(), HEX);
    Serial.print(" :  ");
    Serial.print(Switch2(), HEX);
    Serial.print(" :  ");
    Serial.print(remotePressDetected, HEX);
    Serial.print(" :  ");
    Serial.print(peIsClear(), HEX);
    Serial.print(" :  ");
    Serial.print(travelOpenTime());
    Serial.print(" :  ");
    Serial.print(travelCloseTime());
    Serial.println();
}

void scanInputs()
{
    // Schmit trigger on the board should clean up the transition but also need to debounce


    // Look for edges (falling) on remote input
    bool currentValue = digitalRead(remotePin);

    if (previousRemoteValue && !currentValue)
    {
        remotePressDetected = true;
    }
    else
    {
#if 0
        // This will be cleared on read. Would be FAR cleaner with protothreads & semaphore.
        remotePressDetected = false;
#endif
    }

    // Update for next loop around
    previousRemoteValue = currentValue;
}

void handleGateState()
{

    switch (gateState)
    {
    case STATE_CLOSED:
        // If we see an edge on the control switch (remote or local), start opening.
        if (remotePressed())
        {
            startOpening();
        }
        break;

    case STATE_CLOSING:
        // Run until timer expires. If PE indicates an obstruction, start opening.
        if (!peIsClear())
        {
            driveMotorStop();
            delay(GATE_REVERSE_DELAY_MS); // Delay before throwing into reverse
            Serial.println("OBSTRUCTION - Opening");
            startOpening();
        }

        if (timerExpired())
        {
            finishClosing();
        }

        if (remotePressed())
        {
            driveMotorStop();
            delay(GATE_REVERSE_DELAY_MS); // Delay before throwing into reverse
            startOpening();
            Serial.println("INTERRUPTION - Opening");
        }
        break;

    case STATE_OPENING:
        // Run until timer expires. PE has no effect on opening.
        if (timerExpired())
        {
            finishOpening();
        }

        if (remotePressed())
        {
            driveMotorStop();
            delay(GATE_REVERSE_DELAY_MS); // Delay before throwing into reverse
            startClosing();
            Serial.println("INTERRUPTION - Closing");
        }
        break;

    case STATE_OPEN:
        // Read remote pressed state first to clear edge state if path is obstructed
        if (remotePressed() && peIsClear())
        {
            startClosing();
        }
        break;

    default:
        Serial.print("INVALID STATE!!!!  ");
        Serial.println(gateState, HEX);
        // gateState = STATE_
        break;
    };
}

bool peIsClear()
{
    return digitalRead(pePin);
}

bool remotePressed()
{
    bool result = remotePressDetected;
    remotePressDetected = false;

    return result;
//    return !digitalRead(remotePin);
}

INT16U Switch1()
{
    return ((digitalRead(switch1_3Pin) ? 4 : 0) +
            (digitalRead(switch1_2Pin) ? 2 : 0) +
            (digitalRead(switch1_1Pin) ? 1 : 0));
}

INT16U Switch2()
{
    return ((digitalRead(switch2_3Pin) ? 4 : 0) +
            (digitalRead(switch2_2Pin) ? 2 : 0) +
            (digitalRead(switch2_1Pin) ? 1 : 0));
}

INT32U travelOpenTime()
{
    return travelTimes[Switch1()];
}

INT32U travelCloseTime()
{
    return travelTimes[Switch2()];
}

void restartTimer()
{
    Serial.println("restartTimer");
    travelTimerStart = millis();
}

bool timerExpired()
{
    bool result = true;

    switch (gateState)
    {
    case STATE_OPENING:
        result = (millis() - travelTimerStart > travelOpenTime());
        break;
    case STATE_CLOSING:
        result = (millis() - travelTimerStart > travelCloseTime());
        break;
    default:
        Serial.println("INVALID TIMER STATE");
        result = true;
        break;
    }

    return result;
}

void driveMotorForward()
{
    Serial.println("driveMotorForward");
    digitalWrite(relay1Pin, HIGH);
    digitalWrite(relay2Pin, LOW);
}

void driveMotorReverse()
{
    Serial.println("driveMotorReverse");
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, HIGH);
}

void driveMotorStop()
{
    Serial.println("driveMotorStop");
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
}

void startOpening()
{
    Serial.println("startOpening");
    restartTimer();
    driveMotorForward();

    gateState = STATE_OPENING;
}

void finishOpening()
{
    Serial.println("finishOpening");
    //restartTimer();
    driveMotorStop();

    gateState = STATE_OPEN;
}

void startClosing()
{
    Serial.println("startClosing");
    restartTimer();
    driveMotorReverse();

    gateState = STATE_CLOSING;
}

void finishClosing()
{
    Serial.println("finishClosing");
    //restartTimer();
    driveMotorStop();

    gateState = STATE_CLOSED;
}
