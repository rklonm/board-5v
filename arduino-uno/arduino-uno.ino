#include <LiquidCrystal.h>
#include "IRremote.h"

#define irpin 9
#define buzzpin 6

int upfreq;
 
char* lastkey;

const int rs_l = 13, en_l = 12, d4_l = 3, d5_l = 10, d6_l = 5, d7_l = 8;
const int rs_r = 2, en_r = A3, d4_r = 4, d5_r = A4, d6_r = A5, d7_r = 7;

LiquidCrystal lcd_l(rs_l, en_l, d4_l, d5_l, d6_l, d7_l);
LiquidCrystal lcd_r(rs_r, en_r, d4_r, d5_r, d6_r, d7_r);

IRrecv irrecv(irpin);
decode_results results;
int last=LOW;

long unsigned lastir;

void setup() {
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(irpin, INPUT);

  irrecv.enableIRIn(); // запускаем прием
  Serial.begin(115200);
  
  // put your setup code here, to run once:
  lcd_l.begin(16, 2);
  lcd_l.print("init L");

  lcd_r.begin(16, 2);
  lcd_r.print("init R");

  tone(buzzpin, 3000, 50);
}

void loop() {

  
if ( irrecv.decode( &results )) { // если данные пришли

  long unsigned code=results.value;

  char* action="undefined";

// Для обработки повторений

  if(results.value==0xFFFFFFFF){
    code=lastir;    
   }else{
    lastir=results.value;  
   }
 
  switch (code){
    case 0x77E1C054 :
      action="A menu";
      break;

    case 0x77E13054 :
      action="A down";
      break;
    
    case 0x77E15054 :
      action="A up";
      break;

    case 0x77E19054 :
      action="A left";
      break;

    case 0x77E16054 :
      action="A right";
      break;

    case 0x77E13A54 :
      action="A center";
      break;

    case 0x77E1FA54 :
      action="A playpause";
      break;

    case 0x77E1A054 :
      action="A CntPlay";
      break;

    
    case 0xB722DD :
      action="pc bgm";
      break;

    case 0xB78877 :
      action="av/pc";
      break;

    case 0xB7A857 :
      action="picture";
      break;

    case 0xB702FD :
      action="red";
      break;

    case 0xB7C23D :
      action="blue";
      break;

    case 0xB7D02F :
      action="stereo";
      break;

    case 0xB742BD :
      action="yellow";
      break;

    case 0xB7827D :
      action="green";
      break;

    case 0xB79867 :
      action="pip";
      break;

    case 0xB7B847 :
      action="pippos";
      break;

    case 0xB728D7 :
      action="txt";
      break;

    case 0xB76897 :
      action="Exit";
      break;

    case 0xB730CF :
      action="SLEEP";
      break;

    case 0xB7B04F :
      action="OK";
      break;

    case 0xB708F7 :
      action="Menue";
      break;

    case 0xB7C837 :
      action="Preview";
      break;

    case 0xB738C7 :
      action="BackSpace";
      break;

    case 0xB718E7 :
      action="Mute";
      break;

    case 0xB750AF :
      action="0";
      break;

    case 0xB7906F :
      action="9";
      break;

    case 0xB710EF :
      action="8";
      break;

    case 0xB7E01F :
      action="7";
      break;

    case 0xB7609F :
      action="6";
      break;

    case 0xB7A05F :
      action="5";
      break;

    case 0xB720DF :
      action="4";
      break;

    case 0xB7C03F :
      action="3";
      break;

    case 0xB740BF :
      action="2";
      break;

    case 0xB7807F :
      action="1";
      break;

    case 0xB7F00F :
      action="Source";
      break;

    case 0xB748B7 :
      action="Power";
      break;

    case 0xB77887 :
      action="<-";
      break;

    case 0xB758A7 :
      action="->";
      break;

    case 0xB7D827 :
      action="UP";
      break;

    case 0xB7F807 :
      action="down";
      break;      
    }

    if(results.value==0xFFFFFFFF){
      onkeyretry();
    }else{
      onkeynew(action,code);
    }
    irrecv.resume(); // принимаем следующую команду
  }
}

void onkeyretry(){
  if(upfreq>=500){
    upfreq-=300;
  }
  dokeybeep();
}

void onkeynew(char* keyname,long unsigned keycode){
  
    lcd_r.clear();
    lcd_r.setCursor(0,0);
    lcd_r.print(keyname);
    lcd_r.setCursor(0,1);
    lcd_r.print(results.value,HEX);

    lcd_l.clear();
    lcd_l.setCursor(0,0);
    lcd_l.print(keyname);
    lcd_l.setCursor(0,1);
    lcd_l.print(results.value,HEX);
    upfreq=4000;
    dokeybeep();
    
//    Serial.println( "-------" );
//    Serial.println( results.value, HEX );
//    Serial.println( keyname );   
}



void dokeybeep(){
//    int freq=random(1, upfreq/100);           
//    tone(buzzpin, freq*100, 50);
      tone(buzzpin, upfreq+50, 40);
      tone(buzzpin, upfreq, 40);
      tone(buzzpin, upfreq-50, 40);


}
