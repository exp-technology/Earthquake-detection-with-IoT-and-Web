#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <time.h>
#include <PString.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
//perocobaan millis()
#define firebaseURl "deteksi-gempa.firebaseio.com"
#define authCode "JQSN6jxFFUctV4IdFsVwlQzUM3SjVsFxrlc6HGnl"
#define buzzer D0
const char* ssid = "smart-parking";
const char* password = "11112222";
unsigned long waktu,waktu2, jeda;
float checking ;
// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
//int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
//int16_t temperature; // variables for temperature data
//int16_t settingan;
char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
double gx , gy,gz,gx1,gy1,gz1;
double gxt,gyt,gzt, gxt1,gyt1,gzt1;
double hasil_x,hasil_y,hasil_z,hasil_accel;

//fungsi waktu
int timezone = 7 * 3600;
int dst = 0;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(buzzer,OUTPUT);
waktu = millis();
waktu2 = 0, jeda = 3000;
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
//  delay(10);
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x1c); // PWR_MGMT_1 register
  Wire.write(0b00011000); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
Pengaturan_wifi();
Pengaturan_data();
pengaturan_waktu();
ambil_sampel();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis () - waktu >= 2000){
    waktu = millis();
    Wire.beginTransmission(MPU_ADDR);
    
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 3*2, true); // request a total of 7*2=14 registers
    
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    gxt = (double)accelerometer_x;
    gyt = (double)accelerometer_y;
    gzt = (double)accelerometer_z;
    gx = (gxt /2048.0)/10;
    gy = (gyt/2048)/10;
    gz = (gzt/2048)/10;
    hasil_x = gx1 - gx;
    hasil_y = gy1 - gy;
    hasil_z = gz1 - gz;
    hasil_accel = (sqrt((pow(hasil_x, 2)) + (pow(hasil_y, 2)) + (pow(hasil_z, 2))));
    Serial.print(hasil_accel,6);
    char buffer[40];
    PString stri(buffer, sizeof(buffer));
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    stri.print(p_tm->tm_mday);
    stri.print("-");
    stri.print(p_tm->tm_mon + 1);
    stri.print("-");
    stri.print(p_tm->tm_year + 1900);
    stri.print("/");
    stri.print(p_tm->tm_hour);
    stri.print("/");
    stri.print(p_tm->tm_min);
    stri.print("/");
    stri.print(p_tm->tm_sec);
//    Serial.print(stri);Serial.println(" iini yang 2 detik");
    const String dir2 = (String) stri;
    double SR;
    if(hasil_accel > 0.039000 && hasil_accel <= 0.092000 ){
      SR = hasil_accel * 16.98 + 3.34;
      Firebase.set(dir2, SR);
      digitalWrite(buzzer,HIGH);
//    Serial.println("ada gempa");
    }
    else if (hasil_accel > 0.092000 && hasil_accel <= 0.18  ){
      SR = hasil_accel * 10 + 4.1;
      Firebase.set(dir2, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.18 && hasil_accel <= 0.34){
      SR = hasil_accel * 5.63 + 4.99;
      Firebase.set(dir2, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.34 && hasil_accel <= 0.65 ){
      SR = hasil_accel * 2.9 + 6.01;
      Firebase.set(dir2, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.65 && hasil_accel <= 1.24 ){
//      SR = hasil_accel * 16.98 + 3.34;
      Firebase.set(dir2, ">= 8");
      digitalWrite(buzzer,HIGH);
    }
    
  }
  if(millis()-waktu2 >= jeda){
    waktu2 = millis();
    jeda = 6000;

    Wire.beginTransmission(MPU_ADDR);
    
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 3*2, true); // request a total of 7*2=14 registers
    
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    gxt = (double)accelerometer_x;
    gyt = (double)accelerometer_y;
    gzt = (double)accelerometer_z;
    gx = (gxt /2048.0)/10;
    gy = (gyt/2048)/10;
    gz = (gzt/2048)/10;
    hasil_x = gx1 - gx;
    hasil_y = gy1 - gy;
    hasil_z = gz1 - gz;
    hasil_accel = (sqrt((pow(hasil_x, 2)) + (pow(hasil_y, 2)) + (pow(hasil_z, 2))));
    Serial.print(hasil_accel,6);Serial.print("  ini ");
    char buffer[40];
    int length = 0;
    PString stri2(buffer + length, sizeof(buffer)-length);
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    stri2.print(p_tm->tm_mday);
    stri2.print("-");
    stri2.print(p_tm->tm_mon + 1);
    stri2.print("-");
    stri2.print(p_tm->tm_year + 1900);
    stri2.print("/");
    stri2.print(p_tm->tm_hour);
    stri2.print("/");
    stri2.print(p_tm->tm_min);
    stri2.print("/");
    stri2.print(p_tm->tm_sec);
    
    const String dir = (String)stri2;
    Serial.println(dir);
    
    Firebase.set(dir, hasil_accel);
    double SR;

    if(hasil_accel < 0.001700 ){
//      SR = hasil_accel * 73.17 + 1.88;
      Firebase.set(dir, "<=2");
//    Serial.println("ada gempa");
      digitalWrite(buzzer,LOW);
    }
    else if(hasil_accel > 0.001700 && hasil_accel <= 0.014000 ){
      SR = hasil_accel * 73.17 + 1.88;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,LOW);
//    Serial.println("ada gempa");
    }

    else if(hasil_accel > 0.014 && hasil_accel <= 0.039 ){
      SR = hasil_accel * 36 + 2.5;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,LOW);
//    Serial.println("ada gempa");
    }
    else if(hasil_accel > 0.039000 && hasil_accel <= 0.092000 ){
      SR = hasil_accel * 16.98 + 3.34;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,HIGH);
//    Serial.println("ada gempa");
    }
    else if (hasil_accel > 0.092000 && hasil_accel <= 0.18  ){
      SR = hasil_accel * 10 + 4.1;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.18 && hasil_accel <= 0.34){
      SR = hasil_accel * 5.63 + 4.99;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.34 && hasil_accel <= 0.65 ){
      SR = hasil_accel * 2.9 + 6.01;
      Firebase.set(dir, SR);
      digitalWrite(buzzer,HIGH);
    }
    else if (hasil_accel > 0.65 && hasil_accel <= 1.24 ){
//      SR = hasil_accel * 16.98 + 3.34;
      Firebase.set(dir, ">= 8");
      digitalWrite(buzzer,HIGH);
    }
  }
}



void Pengaturan_wifi(){
  Serial.println();
  Serial.print("Wifi connecting");
  Serial.print(ssid);

  WiFi.begin(ssid,password);

  Serial.println();
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  delay(2000);
  Serial.println(WiFi.localIP());
  Serial.println("wif.iconnect");
}

void Pengaturan_data(){
  Serial.print("coba firebase");
  Firebase.begin(firebaseURl,authCode);
  delay(1000);
   checking = Firebase.getFloat("check");
  while(checking != 1){
//    Firebase.begin(firebaseURl,authCode);
     checking = Firebase.getFloat("check");
    delay(1000);
  }
//  hitung = Firebase.getFloat("antrian");
//  Serial.println(hitung);
waktu = millis();
waktu2 = millis();
}

void ambil_sampel(){
  long acx = 0,acy = 0,acz = 0;
  int n_sample = 1000;
  for(int f=1 ; f<=n_sample; f++){
  Wire.beginTransmission(MPU_ADDR);
  
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 3*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  acx += Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  acy += Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  acz += Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  }
  
  acx = acx / n_sample;
  acy= acy /n_sample;
  acz= acz/n_sample;
  gxt1 = (double)acx;
  gyt1 = (double)acy;
  gzt1 = (double)acz;
  gx1 = (gxt1 /2048.0)/10;
  gy1 = (gyt1/2048.0)/10;
  gz1 = (gzt1/2048.0)/10;
  Serial.println("sampel");
  Serial.print(" | ax sip= "); Serial.print(acx);
  Serial.print(" | ay = "); Serial.print(acy);
  Serial.print(" | az = ");Serial.print(acz);
  Serial.print(" | gx = "); Serial.print(gx1,6);
  Serial.print(" | gy = "); Serial.print(gy1,6);
  Serial.print(" | gz = ");Serial.println(gz1,6 );
  Wire.endTransmission(true);
  delay(100);

}

void pengaturan_waktu(){
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
//  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
//     Serial.print("*");
     delay(1000);
  }
//  Serial.println("\nTime response....OK"); 
}
