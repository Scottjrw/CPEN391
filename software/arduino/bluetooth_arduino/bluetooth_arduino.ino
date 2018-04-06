#include <SoftwareSerial.h>

int bluetoothTx = 8;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 9;  // RX-I pin of bluetooth mate, Arduino D3

int button_typing = 6;
int button_mode = 7;

int current_mode = 0;

bool button_typing_pressed;
bool button_mode_pressed;
bool drawing;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  pinMode(button_typing, INPUT_PULLUP);
  pinMode(button_mode, INPUT_PULLUP);
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  button_typing_pressed = false;
  button_mode_pressed = false;
  drawing = false;

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
    Serial.print((char)bluetooth.read());  
//    if (c.equals("49")){ // 49 is ascii equivalent to 1
//      digitalWrite(led, HIGH);
//    }
//    else if (c.equals("48")){  // 48 is ascii equivalent to 0
//      digitalWrite(led, LOW);
//    }
  }

  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }

  if (digitalRead(button_mode) == LOW){
    if (!button_mode_pressed){
      current_mode = current_mode + 1;
      bluetooth.print(2);
      button_mode_pressed = true;
    }
  }
  else {
    button_mode_pressed = false;
    delay(20);
  }

  if (digitalRead(button_typing) == LOW){
    if (!button_typing_pressed){
      Serial.println("button press");
      bluetooth.print(1);
      if (drawing == false){
        digitalWrite(wand_led, HIGH);
        drawing = true;
      }
      else {
        drawing = false;
      }
      button_typing_pressed = true;
    }
  }
  else {
    if (drawing == false){
      digitalWrite(wand_led, LOW);
    }
    button_typing_pressed = false;
    delay(20);
  }
  // and loop forever and ever!
}
