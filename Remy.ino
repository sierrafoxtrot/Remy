/*

    Remy - Gate Controller
    Copyright (C) 2018 Scott Finneran

    Windsock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Windsock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Windsock.  If not, see <http://www.gnu.org/licenses/>.

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

enum
{
    STATE_UNDEFINED = 0,
    STATE_CLOSED    = 1,
    STATE_CLOSING   = 2,
    STATE_OPENING   = 3,
    STATE_OPEN      = 4,
    STATE_MAX       = STATE_OPEN
};

int gateState = STATE_CLOSED;

unsigned long travelTimerStart = 0;
unsigned long travelTime  = 20000L;

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
    pinMode(relay2Pin, OUTPUT);
    pinMode(relay1Pin, OUTPUT);

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
    Serial.print(digitalRead(switch1_3Pin));
    Serial.print(" :  ");
    Serial.print(digitalRead(switch2_1Pin));
    Serial.print(" :  ");
    Serial.print(Switch1(), HEX);
    Serial.print(" :  ");
    Serial.print(Switch2(), HEX);
    Serial.print(" :  ");
    Serial.print(digitalRead(remotePin), HEX);
    Serial.print(" :  ");
    Serial.print(digitalRead(pePin), HEX);

    Serial.println();

    delay(500);

    switch (gateState)
    {
    STATE_CLOSED:

        break;

    STATE_CLOSING:
        break;

    STATE_OPENING:

        break;

    STATE_OPEN:
        break;

    default:
        Serial.println("INVALID STATE!!!!");
        // gateState = STATE_
    };
}

void restartTimer()
{
    Serial.println("restartTimer");
    travelTimerStart = millis();
}

bool timerExpired()
{
    return (millis() - travelTimerStart > travelTime);
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

void startClosing()
{
    Serial.println("startClosing");
    restartTimer();
    driveMotorReverse();

    gateState = STATE_CLOSING;
}
