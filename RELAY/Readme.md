# LOCK
Create a password lock with a relay

This program makes use of a relay, a 4x4 keypad and a 4x20 LCD
Display to simulate a lock. To unlock the relay the user must
enter a unique password into the device by using the keypad.
After a certain amount of tries, the device will be blocked
for a couple of seconds. The user has a limited number of tries
before the lock stays locked indefinitely.

The first time the program runs in the device, it will ask the
user to enter a 20 character long password. After this, the
password wont be able to change any more.
