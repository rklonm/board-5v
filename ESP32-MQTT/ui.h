#ifndef UI_H
#define UI_H

// 0 = null
//
//  A  B
//
//  C  D
//    
//  E  F
// 
//  G  H
//

struct RGB {
  byte r;
  byte g;
  byte b;
};


class UI{
  public:
  

  // widgets
  int count_widgets=10;
  String  w_name[10]={ "nil", "A",      "B",     "C",   "D",   "E",       "F",        "G",        "H",        "boot"    };
  int        w_x[10]={ 0,      0,        123,     0,     123,   0,        123,        0,          123,        0         };  
  int        w_y[10]={ 0,      0,        0,       81,    81,    162,      162,        243,        243,        81        };  
  int        w_w[10]={ 0,      117,      117,     117,   117,   117,      117,        117,        117,        240       };  
  int        w_h[10]={ 0,      75,       75,      75,    75,    75,       75,         75,         75,         75        };
  int w_textsize[10]={ 0,      0,        0,       0,     0,     0,        0,          0,          0,          0         };
  String w_theme[10]={ "nil", "wht",    "blu",   "nil", "nil", "yel",     "yel",      "yel",      "red",      "gre"     };
  String  w_capt[10]={ "",    "BMP180 давление", "DHT11 влажность", "",    "",    "BMP180 температура",  "DHT11 температура",    "RTC температура",      "ESP температура",      "Устройство"  };
  String  w_post[10]={ "",    "мм ртутного столба",     "%",     "",    "",    "градус Цельсия", "градус Цельсия",  "градус Цельсия",  "градус Цельсия",  "msg"     };
  String lastval[10]={ "",    "",       "",      "",    "",    "",        "",         "",         "",         ""        };

  // themes
  int count_themes=6;
  String  t_name[6]={ "nil",   "yel",       "red",     "gre",     "blu",     "wht"         };
  RGB       t_bg[6]={ {0,0,0}, {64, 64, 0}, {64,0,0},  {0,64,0},  {0,0,64},  {64,64,64}    };
  RGB   t_border[6]={ {0,0,0}, {128,128,0}, {128,0,0}, {0,128,0}, {0,0,128}, {128,128,128} };
  RGB     t_text[6]={ {0,0,0}, {192,192,0}, {192,0,0}, {0,192,0}, {0,0,192}, {192,192,192} };
  RGB     t_high[6]={ {0,0,0}, {255,255,0}, {255,0,0}, {0,255,0}, {0,0,255}, {255,255,255} };
  RGB   t_contra[6]={ {0,0,0}, {0,0,255  }, {0,255,0}, {255,0,0}, {255,0,0}, {255,0,0}     };
  
  UI();
  ~UI();
  
  String utf8rus(String source);
  
  void begin();

  void show(String widget_name,String value);
  void show(String widget_name,float value);

  int getWidgetKeyByName(String widgetname);
  int getThemeKeyByName(String themename);
  word rgb2word( RGB color);
};


UI::~UI(){}
UI::UI(){}
void UI::begin(){
  tft.cp437(true);
  tft.fillScreen(ILI9341_BLACK);
}

/* Recode russian fonts from UTF-8 to Windows-1251 */
String UI::utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}



void UI::show(String widget_name,float value){
  show(widget_name,String(value,0));
};


void UI::show(String widget_name,String value){
  int id=getWidgetKeyByName(widget_name);
  if(id>0){
    if (value.equals(lastval[id])){
      ; // todo blink .. or not
    }else{
      lastval[id]=value;

      String outval=utf8rus( value );
      String outcapt=utf8rus( w_capt[id] );
      String outpost=utf8rus( w_post[id] );
      
      int oth_textsize=1;
      int tid=getThemeKeyByName(w_theme[id]);
      
      int text_textsize=w_textsize[id];
      if (text_textsize==0){
          int max_textsize  = round( ( ( w_h[id] - 2 - 2 - 2 - 2 - ( oth_textsize * 8 * 2 ) )/8)  - 0.5 );
          text_textsize = round( ( ( w_w[id] - 2 ) / ( outval.length() * 6 ) )                 - 0.5 );
          if (text_textsize > max_textsize ) {
              text_textsize=max_textsize;
          }
          // todo: not working auto detect
      }


      int text_width=outval.length()*6*text_textsize;
      int text_height=8*text_textsize;      

      int post_width=outpost.length()*6*oth_textsize;
      int post_height=8*oth_textsize;     


      tft.fillRect(w_x[id]+1,w_y[id]+1,w_w[id]-2,w_h[id]-2,rgb2word(t_bg[tid]));
      tft.drawRect(w_x[id],w_y[id],w_w[id],w_h[id],rgb2word(t_border[tid]));
      
      tft.setTextColor(rgb2word(t_high[tid]));
      tft.setTextSize(text_textsize); 
      
      tft.setCursor( 1+w_x[id]+(w_w[id]/2)-text_width/2 , 1+w_y[id]+(w_h[id]/2)-text_height/2); 
      tft.print(outval);

      tft.setTextSize(oth_textsize);
      tft.setTextColor(rgb2word(t_text[tid]));
      
      tft.setCursor(w_x[id]+2,w_y[id]+2); 
      tft.print(outcapt); 

      tft.setCursor(w_x[id]+w_w[id]-2-post_width ,w_y[id]+w_h[id]-2-post_height); 
      tft.print(outpost);

    }
  }
}

int UI::getWidgetKeyByName(String widgetname){
  for(int i = 1; i<= count_widgets; i++ ){
     if (widgetname.equals(w_name[i])){
       return i;  
     }
  }
  return 0;   
}

int UI::getThemeKeyByName(String themename){
 for(int i = 1; i<= count_themes; i++ ){
     if (themename.equals(t_name[i])){
       return i;  
     }
  }
  return 0;    
}

word UI::rgb2word( RGB color){
    return ( ((color.r & 0xF8) << 8) | ((color.g & 0xFC) << 3) | (color.b >> 3) );
}

#endif
