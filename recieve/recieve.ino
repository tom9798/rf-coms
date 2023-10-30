int t = 0;
int interval = 750;
int message[256];
int bit = 0;
int msgLen = 8;

void setup()
{
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), recieved, CHANGE);
  Serial.println("Waiting for message...");

  while (bit < msgLen * 8)
  {
    Serial.println("Waiting for message...");
    delay(1000);
  }

  // if (bit == msgLen*8){
  for (int i = 0; i < msgLen * 8; i += 8)
  {
    // int c = 0;
    for (int j = 0; j < 8; j++)
    {
      Serial.print(message[i + j]);
      //   c += message[i+j] * pow(2, 7-j);
    }
    // Serial.print(c);
  }
  // }

  if (bit == msgLen * 8)
  {
    Serial.println("Message recieved: ");

    for (int i = 0; i < msgLen * 8; i += 8)
    {
      int c = 0;
      for (int j = 0; j < 8; j++)
      {
        c += message[i + j] * pow(2, 8 - j);
      }
      Serial.print((char)c);
    }

    bit++;
  }
}

void loop()
{
}

void recieved()
{
  int time = micros() - t;

  if (digitalRead(2) == HIGH)
  {
    t = micros();
    return;
  }
  else
  {
    if (time >= interval / 2)
    {
      // recieved 1
      message[bit] = 1;
    }
    else
    {
      // recieved 0
      message[bit] = 0;
    }
    bit++;
  }
  return;
}