// Pin declaration
int sendPin = 5;
int recPin = 3;

// Send variables
int incomingByte = 0;
int bufSize = 0;
int interval = 36; // 36us = 27.7kHz
byte serialBuffA = 0;
byte serialBuffB = 0;

int rPin = 10;
int gPin = 11;
int bPin = 12;

// Receive variables
int t = 0;
bool message[24]; // 3 bytes
int bit = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(sendPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(recPin), received, CHANGE);
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
}

void loop()
{
  // Send
  if (Serial.available() > 5)
  {
    for (int i = 0; i < 3; i++)
    {
      serialBuffA = Serial.read();
      serialBuffB = Serial.read();
      sendByte(getHex(serialBuffA, serialBuffB));
    }
    Serial.flush();
  }

  // Receive
  if (bit == 24)
  {

    // Figure out what I just received
    byte r = 0;
    byte g = 0;
    byte b = 0;
    for (int i = 0; i < 8; i++)
    {
      r += ((byte)message[i]) << (7 - i);
    }
    for (int i = 0; i < 8; i++)
    {
      g += ((byte)message[i + 8]) << (7 - i);
    }
    for (int i = 0; i < 8; i++)
    {
      b += ((byte)message[i + 16]) << (7 - i);
    }

    analogWrite(rPin, r);
    analogWrite(gPin, g);
    analogWrite(bPin, b);

    // Debug prints
    // Serial.print("R: ");
    // Serial.println(r, BIN);
    // Serial.print(" G: ");
    // Serial.println(g, BIN);
    // Serial.print(" B: ");
    // Serial.println(b, BIN);

    bit = 0;
    for (int i = 0; i < 24; i++)
    {
      message[i] = false;
    }
  }
}

void sendByte(byte b)
{
  bool result[8];
  for (int i = 0; i < 8; i++)
  {
    sendBit((b & (0b10000000 >> i)) != 0);
  }
}

void sendBit(bool bit)
{
  if (bit)
  {
    digitalWrite(sendPin, HIGH);
    delayMicroseconds((interval * 2) / 3);
    digitalWrite(sendPin, LOW);
    delayMicroseconds(interval / 3);
  }
  else
  {
    digitalWrite(sendPin, HIGH);
    delayMicroseconds(interval / 3);
    digitalWrite(sendPin, LOW);
    delayMicroseconds((interval * 2) / 3);
  }
}

void received()
{
  int time = micros() - t;
  if (digitalRead(recPin) == HIGH)
  {
    t = micros();
    return;
  }
  else
  {
    if (time >= interval / 2)
    {
      // received 1
      message[bit] = true;
    }
    else
    {
      // received 0
      message[bit] = false;
    }
    bit++;
  }
  return;
}

byte getHex(byte dataA, byte dataB)
{
  byte hexa = 0;

  if (dataA <= 0x39)
  {
    hexa += (dataA - 0x30) << 4; // preserve: 1 - 9
  }
  else
  {
    hexa += (dataA - 0x37) << 4; // preserve A - F
  }

  if (dataB <= 0x39)
  {
    hexa += (dataB - 0x30); // preserve: 1 - 9
  }
  else
  {
    hexa += (dataB - 0x37); // preserve A - F
  }

  return hexa;
}
