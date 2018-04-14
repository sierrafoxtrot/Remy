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
#if 1
  //start serial connection
    Serial.begin(9600);
#endif
    //print out the value of the pushbutton
    Serial.println("Hello World. Version 1\n");


    pinMode(remotePin, INPUT_PULLUP);
    pinMode(pePin, INPUT_PULLUP);
    pinMode(relay2Pin, OUTPUT);
    pinMode(relay1Pin, OUTPUT);

    pinMode(switch1_3Pin, INPUT_PULLUP);
    pinMode(switch1_2Pin, INPUT_PULLUP);
    pinMode(switch1_1Pin, INPUT_PULLUP);

    pinMode(switch2_3Pin, INPUT_PULLUP);
    pinMode(switch2_2Pin, INPUT_PULLUP);
    pinMode(switch2_1Pin, INPUT_PULLUP);
//    pinMode(13, OUTPUT);
}

void loop()
{
    Serial.print("Hello World. Version 1.");
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
#if 0
    digitalWrite(relay1Pin, digitalRead(switch1_3Pin));

//    digitalWrite(relay1Pin, HIGH);
    digitalWrite(relay2Pin, LOW);
    delay(500);

//    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, HIGH);
    delay(2000);
#endif

#if 1

    digitalWrite(relay1Pin, digitalRead(switch1_3Pin));
    digitalWrite(relay2Pin, digitalRead(switch2_1Pin));
#endif

#if 0
            //read the pushbutton value into a variable
            int sensorVal = digitalRead(2);

            //print out the value of the pushbutton
            Serial.println(sensorVal);

            // Keep in mind the pullup means the pushbutton's
            // logic is inverted. It goes HIGH when it's open,
            // and LOW when it's pressed. Turn on pin 13 when the
            // button's pressed, and off when it's not:
            if (sensorVal == HIGH) {
                digitalWrite(13, LOW);
            }
            else {
                digitalWrite(13, HIGH);
            }
#endif
}
