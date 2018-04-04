Dryer Sensor
============

![Dryer Sensor installed and running](img/installed.jpg)

This is a simple ESP8266-based sensor to detect when a clothes dryer has
stopped and send a message to an MQTT server.  The MQTT server can then
send a notification as appropriate (e.g. SMS, email, etc.)

(Note that the MQTT server is not part of this project but is separate.
This project does not directly notify anyone, merely inform the MQTT server
when the dryer changes state and what the new state is, or when the
keypad is used to change the person and method to notify.)

The sensor is built from the following:

* Wemos D1 Mini ESP8266
* HD44780 2-line LCD character display
* MPU6050 accelerometer
* 4x4 matrix keypad, exposed via I2C

![Internals of the unit](img/guts.jpg)

Some sketches for cases are available in the `cases` directory in
[OpenSCAD](http://www.openscad.org/) format.  The 3D cases can be used for
3D printing, while the 2D cases can be laser cut from thin pieces of wood
or similar.  (You may need to modify the cases a bit depending on the sizes
of the components used.)

Notifications are provided by a daemon (under "notifier") that listens for
MQTT messages and sends the relevant information to an external notification
system.  Currently [Twilio](https://www.twilio.com) and
[Amazon SNS](https://aws.amazon.com/sns/) are supported but this could easily
be extended to others.
