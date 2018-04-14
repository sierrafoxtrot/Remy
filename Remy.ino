/*

    Remy - Gate Controller
    Copyright (C) 2018 Scott Finneran

    Remy is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Remy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Remy.  If not, see <http://www.gnu.org/licenses/>.

 */

int remotePin = 4;
int pePin = 3;
int relay2Pin = 2;
int relay1Pin = 1;
int switch1_3Pin = 13;
int switch1_2Pin = 12;
int switch1_1Pin = 11;

int switch2_3Pin = 9;
int switch2_2Pin = 8;
int switch2_1Pin = 7;

enum STATE
{
    STATE_INVALID   = 0,
    STATE_CLOSED    = 1,
    STATE_CLOSING   = 2,
    STATE_OPENING   = 3,
    STATE_OPEN      = 4,
};

STATE gateState = STATE_OPEN;

int mainLoopPeriodicity = 750;

unsigned long travelTimerStart = 0;
unsigned long travelTimes[] = {4000, 8000, 12000, 16000, 20000, 24000, 28000, 32000};

int Switch1()
{
    return ((digitalRead(switch1_3Pin) ? 4 : 0) +
            (digitalRead(switch1_2Pin) ? 2 : 0) +
            (digitalRead(switch1_1Pin) ? 1 : 0));
}

int Switch2()
{
    return ((digitalRead(switch2_3Pin) ? 4 : 0) +
            (digitalRead(switch2_2Pin) ? 2 : 0) +
            (digitalRead(switch2_1Pin) ? 1 : 0));
}

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
}

void loop()
{
    Serial.print("Remy Version 1.");
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
    Serial.print(remotePressed(), HEX);
    Serial.print(" :  ");
    Serial.print(peClear(), HEX);
    Serial.print(" :  ");
    Serial.print(travelOpenTime());
    Serial.print(" :  ");
    Serial.print(travelCloseTime());



    delay(mainLoopPeriodicity);


    Serial.println();

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
        if (!peClear())
        {
            driveMotorStop();
            delay(1000); // Delay before throwing into reverse
            Serial.println("OBSTRUCTION - Opening");
            startOpening();
        }

        if (timerExpired())
        {
            finishClosing();
        }

        if (remotePressed())
        {
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
            startClosing();
            Serial.println("INTERRUPTION - Closing");
        }
        break;

    case STATE_OPEN:
        if (peClear() && remotePressed())
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

bool peClear()
{
    return digitalRead(pePin);
}

bool remotePressed()
{
    return !digitalRead(remotePin);
}

unsigned long travelOpenTime()
{
    return travelTimes[Switch1()];
}

unsigned long travelCloseTime()
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
