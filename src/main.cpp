// Uncomment to update with resin.io:
//#define RESIN_IO
// Set to resin.io dependent app ID (if using resin.io):
#define RESIN_APPLICATION "12345"

// ESP8266 WiFi support:
#include <ESP8266WiFi.h>

// I2C support:
#include <Wire.h>

// LCD support:
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// Matrix keypad support:
#include <Keypad_I2C/Keypad_I2C.h>
#include <Keypad.h>

// MQTT support:
#include <PubSubClient.h>

// Accelerometer support:
#include <MPU6050.h>

// Resin.io support:
#ifdef RESIN_IO
#include <resin.h>
Resin resin;
#endif /* RESIN_IO */

/*** WiFi setup ***/
const char* ssid = "YOUR_SSID_HERE";
const char* password = "YOUR_PASSWORD_HERE";

WiFiClient wifiClient;

/*** MQTT setup ***/
const char* mqtt_server = "mqtt.YOUR_MQTT.com";
PubSubClient mqtt(wifiClient);
char msg[50];
const char mqttTopicPerson[] = "udl/basement/dryer/notifyPerson";
const char mqttTopicMethod[] = "udl/basement/dryer/notifyMethod";
const char mqttTopicStatus[] = "udl/basement/dryer/status";
const unsigned int maxPersonLength = 20;
const unsigned int maxMethodLength = 10;
char notifyPerson[maxPersonLength] = "";
char notifyMethod[maxMethodLength] = "";

/*** Keypad setup ***/
#define KEYPAD_ADDR 0x38

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad_I2C keypad = Keypad_I2C( makeKeymap(keys), rowPins, colPins, ROWS, COLS, KEYPAD_ADDR );


/*** LCD setup ***/
const int LCD_ROWS = 2;
const int LCD_COLS = 16;
hd44780_I2Cexp lcd(0x3f);


/*** Accelerometer setup ***/
MPU6050 mpu;
// Minimum delta(accel) to count as motion:
#define JITTER 500
// Number of sequential running/stopped measurements to change state:
#define STATELIMIT 20
// When to throw out measurements and start fresh:
#define CLEARLIMIT 144000
int16_t lastX, lastY, lastZ;
unsigned int runCtr, stopCtr;
bool lastRunning;
bool dryerRunning;

/*** Function Definitions ***/

void mqtt_reconnect();
void wifi_reconnect();
void set_notify(const char*, const char*);
void set_notify_mqtt();
void set_status_mqtt();
void set_notify_lcd();
void set_status_lcd();
void keypadEvent(KeypadEvent);
void mqtt_received(char*, byte*, unsigned int);
void notify();


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting setup");

  // Init LCD
  if (lcd.begin(LCD_COLS, LCD_ROWS))
    Serial.println("ERROR IN lcd.begin()");
  Serial.print("LCD address: ");
  Serial.println(lcd.getProp(hd44780_I2Cexp::Prop_addr));

  // Init WiFi
  wifi_reconnect();

  // Init keypad
  keypad.begin();
  keypad.addEventListener(keypadEvent);
  Serial.println("Keypad configured");

  // Init MQTT
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(mqtt_received);

  // Init accelerometer
  //Wire.begin(); // already handled by LCD?
  Serial.print("Initializing accelerometer...");
  mpu.initialize();
  if (mpu.testConnection())
    Serial.println(" done.");
  else
  {
    Serial.println(" failed!  Dying...");
    lcd.setCursor(0, 1);
    lcd.print("NO ACCELEROMETER");
    while (true); // just die here
  }
  lastX = lastY = lastZ = 0;
  runCtr = stopCtr = 0;
  lastRunning = false;
  dryerRunning = false;

  #ifdef RESIN_IO
    // Init resin.io integration
    resin.Setup(RESIN_APPLICATION, ssid, password, false);
  #endif /* RESIN_IO */

  Serial.println("Ready.");
}

void loop()
{
  int16_t aX, aY, aZ, dX, dY, dZ;
  // Make sure we are connected
  if (WiFi.status() != WL_CONNECTED)
  {
    wifi_reconnect();
    set_notify_lcd();
  }
  if (!mqtt.connected())
  {
    mqtt_reconnect();
    set_notify_lcd();
  }

  // Service MQTT client
  mqtt.loop();

  // Service accel
  mpu.getAcceleration(&aX, &aY, &aZ);

  dX = abs(aX - lastX);
  dY = abs(aY - lastY);
  dZ = abs(aZ - lastZ);

  lastX = aX;
  lastY = aY;
  lastZ = aZ;

  /*
  Serial.print("aX: "); Serial.print(aX);
  Serial.print(" aY: "); Serial.print(aY);
  Serial.print(" aZ: "); Serial.println(aZ);
  Serial.print("dX: "); Serial.print(dX);
  Serial.print(" dY: "); Serial.print(dY);
  Serial.print(" dZ: "); Serial.println(dZ);
  */

  if ( (dX > JITTER) || (dY > JITTER) || (dZ > JITTER) )
  {
    if (lastRunning)
      runCtr++;
    else
    {
      stopCtr = 0;
      lastRunning = true;
    }
  }
  else
  {
    if (lastRunning)
    {
      lastRunning = false;
      runCtr = 0;
    }
    else
      stopCtr++;
  }

  if ( (runCtr > STATELIMIT) && (!dryerRunning) )
  {
    dryerRunning = true;
    set_status_mqtt();
    stopCtr = 0;
  }
  else if ( (stopCtr > STATELIMIT) && (dryerRunning) )
  {
    dryerRunning = false;
    set_status_mqtt();
    runCtr = 0;
  }

  if ( (runCtr > CLEARLIMIT) || (stopCtr > CLEARLIMIT) )
  {
      runCtr = 0;
      stopCtr = 0;
  }

  // Update LCD
  set_status_lcd();

  // Not sure why I  need to actively look for keypad events here; they are on a callback
  KeypadEvent key = keypad.getKey();
  if (key)
    Serial.println(key);

  #ifdef RESIN_IO
    // Check for application updates
    resin.Loop();
  #endif /* RESIN_IO */

  delay(100);
}

void keypadEvent(KeypadEvent key)
{
  // String literals in code are apparently of type String rather than const char[] :(
  const char* pBen = "Ben";
  const char* pPhi = "Phi";
  const char* pRachel = "Rachel";
  const char* pAndi = "Andi";
  const char* pRM = "R.M.";
  const char* mEmail = "email";
  const char* mSMS = "sms";
  const char* blank = "";

  Serial.print("Keypad event: ");
  Serial.println(key);

  if (keypad.getState() == PRESSED)
  {
    switch (key)
    {
      case '1': // Ben email
        set_notify(pBen, mEmail);
        break;
      case '2': // Ben sms
        set_notify(pBen, mSMS);
        break;
      case '4': // Phi email
        set_notify(pPhi, mEmail);
        break;
      case '5': // Phi sms
        set_notify(pPhi, mSMS);
        break;
      case '7': // Rachel email
        set_notify(pRachel, mEmail);
        break;
      case '8': // Rachel sms
        set_notify(pRachel, mSMS);
        break;
      case '3': // Andi email
        set_notify(pAndi, mEmail);
        break;
      case 'A': // Andi sms
        set_notify(pAndi, mSMS);
        break;
      case '6': // R.M. email
        set_notify(pRM, mEmail);
        break;
      case 'B': // R.M. sms
        set_notify(pRM, mSMS);
        break;
      default:  // clear
        set_notify(blank, blank);
        break;
    }
  }
}

void set_notify(const char* person, const char* method)
{
  // Called when the person/method is updated via keypad
  // Convenience function to update the LCD as well as send out
  // an MQTT blast

  Serial.print("Set notification: ");
  Serial.print(person);
  Serial.print(" - ");
  Serial.println(method);

  // Update the notification details
  strcpy(notifyPerson, person);
  strcpy(notifyMethod, method);

  // Update the LCD and the MQTT server
  set_notify_lcd();
  set_notify_mqtt();
}

void set_notify_lcd()
{
  // Update LCD
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  if (strlen(notifyPerson) > 1)
  {
    lcd.print(notifyPerson);
    lcd.print(" [");
    lcd.print(notifyMethod);
    lcd.print("]");
  }
  else
  {
    lcd.print("no notification");
  }
}

void set_status_lcd()
{
    lcd.setCursor(0, 0);
    lcd.print(dryerRunning ? "running" : "stopped");
    lcd.print("   ");
    // Calculation for real temperature from https://www.i2cdevlib.com/forums/topic/53-mpu-6050-temperature-compensation/
    String temp = String(round((mpu.getTemperature()/340. + 36.53) * 10) / 10.0).substring(0, 4);
    lcd.print(temp + (char)223 + "C"); // 223 == degree sign ⁰
}

void set_notify_mqtt()
{
  // Publish MQTT message
  mqtt.publish(mqttTopicPerson, notifyPerson, true);
  mqtt.publish(mqttTopicMethod, notifyMethod, true);
}

void set_status_mqtt()
{
    mqtt.publish(mqttTopicStatus, dryerRunning ? "running" : "stopped", true);
}


void mqtt_received(char* topic, byte* payload, unsigned int length)
{
  // Callback for dealing with MQTT messages received

  // Note that MQTT payloads are *not* null-terminated.  So we'll track
  // through them one byte at a time to do a cheap knock-off of strcpy()
  // and then either add a null terminator (if there's space) or just
  // truncate the last character and replace it with the null terminator
  // (if the string has filled up the array).  What a world, what a world.
  int i;
  
  Serial.print("MQTT message received: [");
  Serial.print(topic);
  Serial.print("] - [");

  // Boy a switch() would be nice here :(
  if (strcmp(topic, mqttTopicPerson) == 0)
  {
    for (i = 0; i < length && i < maxPersonLength; i++)
    {
      notifyPerson[i] = (char)payload[i];
      Serial.print(notifyPerson[i]);
    }
    if (i < maxPersonLength)
      i++;
    notifyPerson[i] = '\0';
  }

  if (strcmp(topic, mqttTopicMethod) == 0)
  {
    for (i = 0; i < length && i < maxMethodLength; i++)
    {
      notifyMethod[i] = (char)payload[i];
      Serial.print(notifyMethod[i]);
    }
    if (i < maxMethodLength)
      i++;
    notifyMethod[i] = '\0';
  }

  Serial.println("]");

  // We just received this via MQTT, so only update the LCD:
  set_notify_lcd();

}

void mqtt_reconnect()
{
  // Keep reconnecting to the MQTT server until we get there
  // (This is intentionally blocking!)
  
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT server...");
    lcd.setCursor(0, 0);
    lcd.print("MQTT connect:");
    lcd.setCursor(0, 1);
    lcd.print(mqtt_server);

    if (mqtt.connect("UDL Dryer"))
    {
      Serial.println(" connected.");
      mqtt.subscribe(mqttTopicPerson);
      mqtt.subscribe(mqttTopicMethod);
    }
    else
    {
      Serial.print(" failed; rc=");
      Serial.println(mqtt.state());
      Serial.println("Waiting to retry...");
      delay(5000);
    }
  }
}

void wifi_reconnect()
{
  Serial.print("Connecting to WiFi AP ");
  Serial.println(ssid);
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connect:");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected.  IP address: ");
  Serial.println(WiFi.localIP());

}
