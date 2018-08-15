#include <SoftwareSerial.h>
/* Create object named SIM900 of the class SoftwareSerial */
SoftwareSerial SIM900(7, 8);

char msg;
int FlowPulse1; //measuring the rising edges of the signal
int FlowPulse2; //measuring the rising edges of the signal
int Calc1;
int Calc2;
int flowsensor1 = 2;    //The pin location of the sensor
int flowsensor2 = 3;    //The pin location of the sensor

int output=0;
int msgStatus=0;

String url = String("\"AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update?api_key=3N3IFGCE5I4KFTAV&field1=");
String url1 = String("&field2=");
String url2 = String("\"");

void openConnection()
{
  Serial.println("HTTP get method :");
  Serial.print("AT\\r\\n");
  SIM900.println("AT"); /* Check Communication */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);
  /* Configure bearer profile 1 */
  Serial.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\\r\\n");    
  SIM900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  /* Connection type GPRS */
  delay(5000);
  ShowSerialData();
  delay(5000);
  Serial.print("AT+SAPBR=3,1,\"APN\",\"www\"\\r\\n");  
  SIM900.println("AT+SAPBR=3,1,\"APN\",\"www\"");  /* APN of the provider */
  delay(5000);
  ShowSerialData();
  delay(5000);
  Serial.print("AT+SAPBR=1,1\\r\\n");
  SIM900.println("AT+SAPBR=1,1"); /* Open GPRS context */
  delay(5000);
  ShowSerialData();
  delay(5000);
  Serial.print("AT+SAPBR=2,1\\r\\n");
  SIM900.println("AT+SAPBR=2,1"); /* Query the GPRS context */
  delay(5000);
  ShowSerialData();
  delay(5000);
  Serial.print("AT+HTTPINIT\\r\\n");
  SIM900.println("AT+HTTPINIT"); /* Initialize HTTP service */
  delay(5000); 
  ShowSerialData();
  delay(5000);
  Serial.print("AT+HTTPPARA=\"CID\",1\\r\\n");
  SIM900.println("AT+HTTPPARA=\"CID\",1");  /* Set parameters for HTTP session */
  delay(5000);
  ShowSerialData();
  delay(15000); 
}

void closeConnection()
{
  Serial.print("AT+HTTPTERM\\r\\n");  
  SIM900.println("AT+HTTPTERM");  /* Terminate HTTP service */
  delay(5000);
  ShowSerialData();
  delay(5000);
  Serial.print("AT+SAPBR=0,1\\r\\n");
  SIM900.println("AT+SAPBR=0,1"); /* Close GPRS context */
  delay(5000);
  ShowSerialData();
  delay(5000);
}
//Function For Calculating Flowrate and the difference

int Flowsensor()
{
 FlowPulse1 = 0;      //Set Nb Tops to 0 ready for calculations
 FlowPulse2 = 0;      //Set Nb Tops to 1 ready for calculations
 sei();            //Enables interrupts
 delay (500);      //Wait 1 second
 cli();          //Disable interrupts
 
 Calc1 = (FlowPulse1 * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour 

 //sei();            //Enables interrupts
 //delay (1000);      //Wait 1 second
 //cli();          //Disable interrupts
 Calc2 = (FlowPulse2 * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour 

 Serial.println ("\nFlow Sensor 1 --");
 Serial.print (Calc1, DEC); //Prints the number calculated above
 Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
 Serial.println ("Flow Sensor 2 --");
 Serial.print (Calc2, DEC); //Prints the number calculated above
 Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line 

 return Calc1-Calc2 ;
}

void rpm1 ()     //This is the function that the interrupt calls 
{ 
 FlowPulse1++;  //This function measures the rising and falling edge of sensors signal
}

void rpm2 ()     //This is the function that the interrupt calls 
{ 
 FlowPulse2++;  //This function measures the rising and falling edge of sensors signal
}

//For Sending SMS
int SendMessage()
{
  Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900.println("AT+CMGS=\"+919033440365\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900.println("Leakage");// The SMS text you want to send
  delay(100);
   SIM900.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  return 1;
}
void setup() {

  SIM900.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600);         //This is the setup function where the serial port is initialized,
  pinMode(flowsensor1, INPUT); //initializes digital pin 5 as an input
  pinMode(flowsensor2, INPUT); //initializes digital pin 6 as an input
  attachInterrupt(digitalPinToInterrupt(flowsensor1), rpm1, RISING); //and the interrupt is attached
  attachInterrupt(digitalPinToInterrupt(flowsensor2), rpm2, RISING); //and the interrupt is attached

  openConnection();
}
void loop()
{  
  String urlfinal= String(url)+int(Calc1)+String(url1)+String(Calc2)+String(url2);

  output=Flowsensor();
  Serial.println("Defference :: ");
 Serial.print(output, DEC);
 Serial.println("");
    
  Serial.print("AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update?api_key=3N3IFGCE5I4KFTAV&field1=\"\\r\\n");
  SIM900.println(urlfinal);  /* Set parameters for HTTP session */
  delay(3000);
  ShowSerialData();
  delay(3000);
  Serial.print("AT+HTTPACTION=0\\r\\n");
  SIM900.println("AT+HTTPACTION=0");  /* Start GET session */
  delay(3000);
  ShowSerialData();
  delay(1000);
  Serial.print("AT+HTTPREAD\\r\\n");
  SIM900.println("AT+HTTPREAD");  /* Read data from HTTP server */
  delay(1000);
  ShowSerialData();
  delay(1000);

}

void ShowSerialData()
{
  while(SIM900.available()!=0)  /* If data is available on serial port */
  Serial.write(char (SIM900.read())); /* Print character received on to the serial monitor */
}
