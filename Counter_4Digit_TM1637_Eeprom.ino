#include <EEPROM.h>

#define bt_up     A0
#define bt_down   A1
#define bt_reset  A2

#define buzzer 13

#define data_pin  2
#define clock_pin 3
                       /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/
const int digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
                          
                         // 12%,  25%,  38%,  50%,  63%,  75%,  88%, 100% 
const int Brightness[] = { 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

long Counter=0;
int flag1=0, flag2=0, timer=0;

void setup(){ // put your setup code here, to run once  
 
pinMode(bt_up,    INPUT_PULLUP);
pinMode(bt_down,  INPUT_PULLUP);
pinMode(bt_reset, INPUT_PULLUP);

pinMode(buzzer, OUTPUT);

pinMode(clock_pin, OUTPUT);
pinMode(data_pin, OUTPUT);

start();
writeValue(Brightness[6]); // set Brightness 0 to 7
stop();


// clear display
write_data(0x00, 0x00, 0x00, 0x00);


 // Indicate that system is ready
 for (int i = 9; i >=0; i--) {
write_data(digits[i%10], digits[i%10], digits[i%10], digits[i%10]);
 delay(1000); 
 }
 
if(EEPROM.read(0)==0){}
else{
eeprom_write();
EEPROM.write(0, 0);
}

eeprom_read(); 
}

void loop(){  

if(digitalRead(bt_up) == 0){ 
if(flag1==0){ flag1=1;
digitalWrite(buzzer, HIGH);
Counter = Counter+1;
if(Counter>9999){Counter=0;} 
eeprom_write();
delay(100); 
 }
}else{flag1=0;}

if(digitalRead(bt_down) == 0){ 
if(flag2==0){ flag2=1;
digitalWrite(buzzer, HIGH);
Counter = Counter-1;
if(Counter<0){Counter=9999;}
eeprom_write();
delay(100);  
 }  
}else{flag2=0;}

if(digitalRead(bt_reset) == 0){ 
digitalWrite(buzzer, HIGH);
if(timer<100){timer=timer+1;}
if(timer==100){
Counter=0;
eeprom_write(); 
 }   
}else{timer=0;}


write_data(digits[(Counter/1000)%10], digits[(Counter/100)%10], digits[(Counter/10)%10], digits[Counter%10]);
delay(10); 
digitalWrite(buzzer, LOW);
}

void eeprom_write(){ 
EEPROM.write(1,  Counter%10);
EEPROM.write(2, (Counter/10)%10);
EEPROM.write(3, (Counter/100)%10);
EEPROM.write(4, (Counter/1000)%10);
}

void eeprom_read(){
Counter=EEPROM.read(4)*1000+EEPROM.read(3)*100+EEPROM.read(2)*10+EEPROM.read(1);
}

void write_data(uint8_t dig1, uint8_t dig2, uint8_t dig3, uint8_t dig4){
  start();
  writeValue(0x40);
  stop();

  start();
  writeValue(0xc0);
  writeValue(dig1);
  writeValue(dig2);
  writeValue(dig3);
  writeValue(dig4);
  stop();
}

void start(void){
  digitalWrite(clock_pin,HIGH);//send start signal to TM1637
  digitalWrite(data_pin,HIGH);
  delayMicroseconds(5);

  digitalWrite(data_pin,LOW);
  digitalWrite(clock_pin,LOW);
  delayMicroseconds(5);
}

void stop(void){
  digitalWrite(clock_pin,LOW);
  digitalWrite(data_pin,LOW);
  delayMicroseconds(5);

  digitalWrite(clock_pin,HIGH);
  digitalWrite(data_pin,HIGH);
  delayMicroseconds(5);
}

bool writeValue(uint8_t value){
  for(uint8_t i = 0; i < 8; i++){
    digitalWrite(clock_pin, LOW);
    delayMicroseconds(5);
    digitalWrite(data_pin, (value & (1 << i)) >> i);
    delayMicroseconds(5);
    digitalWrite(clock_pin, HIGH);
    delayMicroseconds(5);
  }

  // wait for ACK
  digitalWrite(clock_pin,LOW);
  delayMicroseconds(5);
  pinMode(data_pin,INPUT);
  digitalWrite(clock_pin,HIGH);
  delayMicroseconds(5);

  bool ack = digitalRead(data_pin) == 0;
  pinMode(data_pin,OUTPUT);
  return ack;
}
