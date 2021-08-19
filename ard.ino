#include <DMD2.h>
#include <fonts/SystemFont5x7.h> 
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <MQUnifiedsensor.h>

#define LED_OUTPUT  1
 #define   THRESHOLD_DAY_LOW      4
  #define   THRESHOLD_NIGHT_HIGH   4.5
  
//Definitions for MQ135
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A5 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  
//#define calibration_button 13 //Pin to calibrate your sensor
//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

#define DHTPIN 5
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

// 10 bit ADC
#define ADC_MAX_DIGITAL  1024

// Voltage sensor scaling
#define V_SENSOR_SCALE    5

// DAC reference for +5v
#define POS_ADC_REF   5.1 

SoftwareSerial espSerial(2,3);

String str;
char chr[5];

SoftDMD dmd(2,1);
static void ScanDMD()
{ 
  dmd.scanDisplay();
}
 
  
void setup(){
  
Serial.begin(9600);
//MQ135
MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(102.2); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
 MQ135.init(); 
 float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  //MQ135.serialDebug(true);

//ESP SERIAL WRITE
espSerial.begin(115200);

//DHT BEGIN
dht.begin();

//RED LED DISPLAY
Serial.println("red_display_setup()");
 // Setup DMD
 dmd.selectFont(SystemFont5x7);           // Font used
 dmd.clearScreen();
 
delay(2000);
}


void loop()
{
float h = dht.readHumidity();
Serial.print("H: ");
Serial.print(h);
Serial.print("% ");
dtostrf(h, 4, 2, chr);
dmd.drawString( 2, 9, "Humd=");
dtostrf(h, 4, 2, chr);
dmd.drawString( 33, 9, chr);

float t = dht.readTemperature();
Serial.print(" T: ");
Serial.print(t);
Serial.println("C ");
dtostrf(t, 4, 2, chr);
dmd.drawString( 2, 0, "Temp=");
dtostrf(t, 4, 2, chr);
dmd.drawString( 33, 0, chr);

dmd.clearScreen();


MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
float a = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
//MQ135.serialDebug(); // Will print the table on the serial port
Serial.print("Air qality : ");
Serial.print(a);
Serial.print(" ppm \n");

float distance = analogRead(A4);       // reads the value of the sharp sensor  
Serial.print("Distance: ");
Serial.println(distance);

//battery ampere
unsigned int average = analogRead(A3);
float avr =analogRead(A3)*(POS_ADC_REF/ADC_MAX_DIGITAL);
float ba=(avr-2.55)/0.185;

//battery voltage
float analogValue1 = analogRead(A2);
Serial.println(analogValue1);
float bv = (POS_ADC_REF/ADC_MAX_DIGITAL)*V_SENSOR_SCALE*analogValue1;

//solar voltage
float analogValue2 = analogRead(A0);
Serial.println(analogValue2);
float sv = (POS_ADC_REF/ADC_MAX_DIGITAL)*V_SENSOR_SCALE*analogValue2;

//solar current
unsigned int average2 = analogRead(A1);
float avr2 =analogRead(A1)*(POS_ADC_REF/ADC_MAX_DIGITAL);
float sa=(avr2-2.55)/0.185;  

#ifdef LED_OUTPUT
  if (sv > THRESHOLD_NIGHT_HIGH)
  {
      led_turn_off();
      Serial.print("LED_turned_OFF");
  }
  else if (sv< THRESHOLD_DAY_LOW)
  {
      led_turn_on();
      Serial.print("LED_turned_ON");
  }
#endif

espSerial.print('<');                   //<(start mark)
espSerial.print((int)t,DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)h, DEC); 
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)a,DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)distance, DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)bv, DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)ba, DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)sv, DEC);
espSerial.print(',');                   //,(data seperator)
espSerial.print((int)sa, DEC);
espSerial.print('>'); 
espSerial.println();

delay(10000);
}

static bool led_status;


#ifdef LED_CONTROL_WITH_PWM

#define LED_OUTPUT_PIN  10


void led_turn_on()
{
  Serial.println("led_turn_on()");
  led_status = HIGH;
  analogWrite(LED_OUTPUT_PIN, 0);

}
void led_turn_off()
{
  Serial.println("led_turn_off()");
  led_status = LOW;
  analogWrite(LED_OUTPUT_PIN, 255);
}
void led_setup()
{
  Serial.println("LED setup()");
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  led_turn_off();

}
void led_toggle()
{
  if (led_status == LOW)
    led_turn_on();
  else
    led_turn_off();

}


#else


#define LED_OUTPUT_PIN  4

void led_turn_on()
{
  Serial.println("led_turn_on()");
  led_status = HIGH;
  digitalWrite(LED_OUTPUT_PIN, 0);

}
void led_turn_off()
{
  Serial.println("led_turn_off()");
  led_status = LOW;
  digitalWrite(LED_OUTPUT_PIN, 255);
}
void led_setup()
{
  Serial.println("LED setup()");
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  led_turn_off();

}
void led_toggle()
{
  if (led_status == LOW)
    led_turn_on();
  else
    led_turn_off();

}
#endif
