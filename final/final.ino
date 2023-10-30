// Pin declaration
int sendPin = 5;
int recPin = 3;

// Send variables
int incomingByte = 0;
int bufSize = 0;
int interval = 750 * 100;
byte toSend[64];

// Recieve variables
int t = 0;
bool message[512]; // 64 bytes
int bit = 0;
int msgLen = 1;

void setup()
{
  Serial.begin(9600);
  pinMode(sendPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(recPin), recieved, CHANGE);
}

void loop()
{
  // Send
  if (Serial.available() > 0)
  {
    bufSize = Serial.readBytes(toSend, Serial.available());

    Serial.println(0b10000000 | ((byte)(bufSize + 1) & 0b01111111), BIN);
    for (int i = 0; i < bufSize; i++)
    {
      Serial.println(toSend[i]);
    }
    sendByte(0b10000000 | ((byte)(bufSize - 1) & 0b01111111)); // sends the Length message
  }

  // Recieve
  if (bit == msgLen * 8)
  {
    // Figure out what I just recieved
    byte command = 0;
    if (msgLen == 1)
    {
      for (int i = 0; i < 8; i++)
      {
        command += ((int)message[i]) << (7 - i);
      }
      Serial.println(command, BIN);
    }
    else
    {
      for (int i = 0; i < msgLen * 8; i += 8)
      {
        for (int j = 0; j < 8; j++)
        {
          command += ((int)message[i + j]) << (7 - j);
        }
        Serial.println(command, BIN);
        command = 0;
      }
    }

    if (0b10000000 & command != 0)
    {
      msgLen = (((int)(command & 0b01111111)) + 1);
      sendByte(0b01010101);
    }

    if (command == 0b01010101)
    {
      // sends the message
      for (int i = 0; i < bufSize; i++)
      {
        sendByte(toSend[i]);
      }
    }

    // Reset the bit counter and flush the message buffer
    bit = 0;
    msgLen = 1;
    for (int i = 0; i < 8; i++)
    {
      message[i] = 0;
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
    delayMicroseconds(interval / 3);
    digitalWrite(sendPin, LOW);
    delayMicroseconds((interval * 2) / 3);
  }
  else
  {
    digitalWrite(sendPin, HIGH);
    delayMicroseconds((interval * 2) / 3);
    digitalWrite(sendPin, LOW);
    delayMicroseconds(interval / 3);
  }
}

void recieved()
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
      // recieved 1
      message[bit] = true;
    }
    else
    {
      // recieved 0
      message[bit] = false;
    }
    bit++;
  }
  return;
}