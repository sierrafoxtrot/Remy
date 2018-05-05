# What is Remy?

It's an Arduino sketch for replacing the CMOS logic with an Arduino compatible LeoStick from Freetronics.

So, it's probably of little use to most people beyond curiosity.

# What was the itch that Remy originally scratched?

A "smart" gate controller that had died. Rather than pouring hundreds of dollars into replacing what is essentially a few analogue timers, the 4000 series CMOS devices were replaced with the LeoStick, ProtoStick and a couple of DIP switches and LEDs. Essentially the LeoStick operates the darlingtons and relays of the original board and makes use of the signal conditioning (not much more than some schmidt trigger inputs) on the sensors. The existing Merlin remote still performs the heavy lifting for the radio side of things.

# How does it work?

The code is pretty straight forward but basically independant open and close timers configurable in situ via a few dip switches. A photoelectric "seeing eye" sensor prevents dented cars and pedestrians. An input recognises commands from a simple keyfob remote.

# Hardware
The LeoStick is a pretty sweet arduino type board based somewhat on the Leonardo.

The lovely folks at Freetronics were kind enough to include one in the swag bag for LCA 2012 in Ballarat and it has been a solution desperately in need of a problem... until now!

Scott Finneran (SierraFoxtrot)
scottfinneran+githubATgmailDOTcom
