#!/usr/bin/env python3

import paho.mqtt.client as paho
from twilio.rest import Client as twilio
import time
import httplib2, urllib
from email.utils import formatdate

# Fill in your phone numbers...
phone_numbers = {"person1": "+10000000000", "person2": "+11111111111"}

# Fill in Twilio account data for SMS notification...
twilio_sid = "ACXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
twilio_auth = "put_auth_code_here"
twilio_from = "+10000000000"

# Fill in AWS SNS data for email notification...
aws_sns_URL = "https://xxxxxxxxxx.execute-api.us-east-1.amazonaws.com/udl/"

# MQTT path
dryerPath = 'udl/basement/dryer/'

dryerNotifyPerson = ''
dryerNotifyMethod = ''
dryerCurStatus = 'stopped'

def notify(person, method, subject, message):
    print("Notifying " + person + " via " + method + " re: " + subject)
    if method == "sms":
      twilio_client = twilio(twilio_sid, twilio_auth)
      sms = twilio_client.messages.create(
        to = phone_numbers[person],
        from_ = twilio_from,
        body = message
      )
      print("Response: " + sms.sid)

    elif method == "email":
      data = { 'subject': subject, 'message': message }
      url = aws_sns_URL + person + '/' + method + '?' + urllib.parse.urlencode(data)
      h = httplib2.Http("cache")
      (resp, content) = h.request(url, "POST", headers={
          'Pragma': 'cache',
          'Expires': formatdate(),
          'Cache-Control': 'max-age=3600',
          'Last-Modified': formatdate()
      })
      print("Response: " + str(resp))

    else:
      print("Unknown notification type: " + method)

def on_connect(client, userdata, flags, rc):
    global dryerPath
    print("Connected with result code " + str(rc))
    client.subscribe(dryerPath + '#')

def on_message(client, userdata, msg):
    global dryerPath
    global dryerCurStatus
    global dryerNotifyPerson
    global dryerNotifyMethod
    topic = msg.topic
    payload = msg.payload.decode('UTF-8')
    if topic == dryerPath + 'notifyPerson':
        print("Dryer notify person: " + payload)
        dryerNotifyPerson = payload.lower().strip()
        if dryerNotifyPerson == 'r.m.':
            dryerNotifyPerson = 'rm'
    elif topic == dryerPath + 'notifyMethod':
        print("Dryer notify method: " + payload)
        dryerNotifyMethod = payload
    elif topic == dryerPath + 'status':
        print("Dryer status: " + payload)
        if dryerCurStatus == 'running' and payload == 'stopped' and dryerNotifyPerson != '':
            # Notify someone
            notify(
                dryerNotifyPerson,
                dryerNotifyMethod,
                'UDL Notification: Dryer finished',
                'Dryer finished at ' + time.strftime('%Y-%m-%d %H:%M:%S')
            )
            client.publish(dryerPath + 'notifyPerson', '', retain=True)
            client.publish(dryerPath + 'notifyMethod', '', retain=True)
        dryerCurStatus = payload
    elif topic == dryerPath + 'temperature':
        print("Dryer temperature: " + payload)


client = paho.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("mqtt.mccollams.com", 1883, 60)
client.loop_forever()
