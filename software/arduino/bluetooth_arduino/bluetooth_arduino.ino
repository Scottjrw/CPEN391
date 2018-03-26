#include <SoftwareSerial.h>

int bluetoothTx = 8;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 9;  // RX-I pin of bluetooth mate, Arduino D3

int button = 4;
int led = 10;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
}

void loop()
{
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    String c = (String)bluetooth.read();
    Serial.print(c);  
    if (c.equals("49")){ // 49 is ascii equivalent to 1
      digitalWrite(led, HIGH);
    }
    else if (c.equals("48")){  // 48 is ascii equivalent to 0
      digitalWrite(led, LOW);
    }
  }
  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }
  if (digitalRead(button) == LOW){
    Serial.println("button press");
    bluetooth.print(1);
    while(digitalRead(button) == LOW) {
      
    }
    delay(500);
  }
  // and loop forever and ever!
}
