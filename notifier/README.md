# Notification daemon for dryer sensor

This should be run on a server somewhere in the same network as the dryer
sensor.  It will listen for MQTT messages and notify people as appropriate.

This uses [Twilio](https://www.twilio.com) for SMS notification and
[AWS SNS](https://aws.amazon.com/sns/) for email notification.  SNS can also
do SMS notification but requires much more configuration than Twilio and so
is out of scope for this.  (It requires setup for email as well which is
also out of scope, but I am using it myself so leaving it in as a reference.
You could always use another email notifcation service instead.)  SNS
requires a schema to be defined on the backend that matches what is provided
in this file.

To use, configure the Twilio information and/or the SNS URL at the top of
the file and run!

## Caveats:

Sending an SMS notification to a user not defined in the list of phone
numbers will cause a traceback.  This could easily be caught but I am
lazy and have a limited number of users to notify so it was easy to make
sure everything worked for my purposes :)
