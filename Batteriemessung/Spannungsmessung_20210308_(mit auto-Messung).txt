#define PIN_INPUT_V 34    //Pin 34

/*ESP-Einstellungen*/
const int waitTime = 1;     /*Wartezeit bis der Zyklus sich wiederholt*/

/*Led-Einrichtung*/
const int led1_gruen = 12;  /* Pinnummer des Ausgangs*/
bool led1_gruen_On = false;       /* Aktueller Zustand des Ausgangs*/
const int led2_rot = 14;
bool led2_rot_On = false;
const int led3_gelb = 27;
bool led3_gelb_On = false;

/*Button-Einrichtung*/
const int button1 = 26;     /* Pinnummer des Eingangs*/
bool button1_press = false; /* Aktueller Zustand des Buttons*/
bool button1_Old = false;   /* Alter Zustand des Buttons*/
const int button2 = 25;
bool button2_press = false;
bool button2_Old = false;
const int button3 = 33;
bool button3_press = false;
bool button3_Old = false;
bool button_veraenderung = false;  /* Wert der eine Veraenderung der Buttons registriert*/

/*Messbereiche*/
int zustand = 0;                  /*|0=Wartend|1=Messung laeuft|2=Ende|*/
int counter = 0;                  /*Zaehler fuer den Wert*/
const int messAnzahl = 3000;        /*Anzahl von Messungen */
const int moeglicheErg = 4095;
int messung[moeglicheErg] = {0};     /*Array mit den Messungen: |Value|Anzahl|*/
int spannungsTabelle[11] = {0};        /*Spannungstabelle: Index+20 ist die Spannung und der Wert ist der ideal-Messwert*/
bool messungBeendet = false;        /*Sobald Messungen vorgenommen wurde wird dieser Wert auf true gesetzt*/
const float spannungsabfall = 20.0;    /*Spannung die ueber die Schaltung abfaellt*/
float ergebnisSpannung = 0.0;       /*Hier wird die berechnete Spannung abgelegt*/

void setup() {
  Serial.begin(9600); //Baudrate fuer die Ausgabe der Konsole
  pinMode(PIN_INPUT_V, INPUT);  //Deklarierung zum Eingang
  /*LEDs werden Regstriert*/
  pinMode(led1_gruen, OUTPUT);
  pinMode(led2_rot, OUTPUT);
  pinMode(led3_gelb, OUTPUT);
  /*Buttons werden Registriert*/
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  vTabEinrichtung();
}

/**
 * Daten der Spannungswerte werden eingerichtet
 */
void vTabEinrichtung(){
  spannungsTabelle[0] = 2192;
  spannungsTabelle[1] = 2311;
  spannungsTabelle[2] = 2432;
  spannungsTabelle[3] = 2559;
  spannungsTabelle[4] = 2679;
  spannungsTabelle[5] = 2800;
  spannungsTabelle[6] = 2928;
  spannungsTabelle[7] = 3071;
  spannungsTabelle[8] = 3216;  
  spannungsTabelle[9] = 3331;     //Schaetzwerte, da Testmessung nur bis 28V moeglich
  spannungsTabelle[10] = 3474;    //Schaetzwerte, da Testmessung nur bis 28V moeglich
  }

/*Funktion registriert die Button eingaben*/
void butUpdate(){
  if(digitalRead(button1) == HIGH){
    button1_press = true;
  }
  else{
    button1_press = false;
    }
  if(digitalRead(button2) == HIGH){
    button2_press = true;
  }
  else{
    button2_press = false;
    }  
  if(digitalRead(button3) == HIGH){
    button3_press = true;
  }
  else{
    button3_press = false;
    }
    if(!button1_Old && !button2_Old && !button3_Old){ /*Ueberprueft, ob der Zustand zuvor ohne einen Druckpunkt bei den Button vorhanden ist*/
      if(button1_Old != button1_press || button2_Old != button2_press || button3_Old != button3_press){   /*Ueberprueft, ob ein Button betätigt wird vorhanden ist*/
        button_veraenderung = true;
      }
    }
  else{
    button_veraenderung = false;
    }
   button1_Old = button1_press;
   button2_Old = button2_press;
   button3_Old = button3_press;
  }

/*Reset der Button-Zustaender fuer den neuen Zyklus*/
void butReset(){
  button1_press = false;
  button2_press = false;
  button3_press = false;
  button_veraenderung = false;
  }

/*Funktion schaltet die LEDs*/
void ledUpdate(){
  if(led1_gruen_On){
    digitalWrite(led1_gruen, HIGH);
  }
  else{
    digitalWrite(led1_gruen, LOW);      
  }
  if(led2_rot_On){
    digitalWrite(led2_rot, HIGH);
  }
  else{
    digitalWrite(led2_rot, LOW);      
  }
  if(led3_gelb_On){
    digitalWrite(led3_gelb, HIGH);
  }
  else{
    digitalWrite(led3_gelb, LOW);      
  }  
}

/*Bereich für die Testmessungen*/
/**
 * Messwert wird hinzugefuegt
 */
bool phaseMessung(){  
  //Serial.println("Messung");
  //Serial.println("Messung: Counter(" + String(counter) + ")" );
  if(counter < messAnzahl){
    int rawValue = analogRead(PIN_INPUT_V);
    //Serial.println("Messung(" + String(counter) + "/" + String(messAnzahl) + ") von rawValue=" + String(rawValue));
    messung[rawValue] = messung[rawValue] + 1;
    counter++;
    return true;
  }
  else{
    counter = 0;
    //memset(messung, 0, sizeof(messung));  //reset des Arrays  //Funktioniert nicht richtig
    return false;
    }
  }

  /**
 * Löscht die Eintraege vom Array Messung
 */
void messungReset(){
  for(int i = 0; i < moeglicheErg; i++){
      messung[i] = 0;
    }
  }

/**
 * Ermittelt, welcher Wert am häufigsten gemessen wurde
 */
int bestMessWert_OLD(){
  int maxAnzahl = 0;
  int index = 0;
  for(int i = 0; i < moeglicheErg; i++){
    if(maxAnzahl < messung[i]){
      maxAnzahl = messung[i];
      index = i;      
      //Serial.println("newMaxWert:" + String(i) + " mit der Anzahl von:" + String(maxAnzahl) + ".");
    }
  }
  return index;
}

/**
 * Summiert alle Gemessenen Werte und berechnet dann den Querschnitt Werte
 */
int bestMessWert(){
  int summe = 0;
  int wert = 0;
  for(int i = 0; i < moeglicheErg; i++){
    if(messung[i] > 0){
        summe = summe + (messung[i] * i);
    }
  }
  wert = summe / messAnzahl;
  Serial.println("Bester Wert:" + String(wert));
  return wert;
}

/**
 * Methode berechnet eine Range von valueStart und valueEnd und sucht in diesem Bereich den Value
 * Als Ergebnis wird das Prozent-Ergebnis von Value in der Range zurueck gegeben
 */
float prozentWert(int value, int valueStart, int valueEnd){
  int range = 0;
  float erg = 0;  
  if(valueEnd > value && value >= valueStart){ 
      range = valueEnd - valueStart;
      erg = ((float)value - (float)valueStart) / (float)range;
      Serial.println("Ergebnis vom prozentWert():" + String(valueEnd) + "-" + String(valueStart) + "=" + String(range) + "=> das sind 100%;");
      Serial.println("float_erg:" + String(erg) + "=((float)value - valueStart):" + String((float)value - valueStart) + " / (float)range:" + String((float)range) + ";");
      return erg;
  }
  return -1.0;
}

/**
 * Wandelt den am häufigsten erhaltenen Wert in eine Spannung um
 */
float ergebnisBerechnung(){
  int rohWert = 0;
  int index = 0;
  int indexwert = 0;
  float spannung = 0.0;
  float kommaWert = 0.0;
  
  rohWert = bestMessWert();
  indexwert = spannungsTabelle[index];    // niederigster zulaessiger Wert
  //Serial.println("Bester Wert rohWert=" + String(rohWert) + "; indexWert=" + String(indexwert));
  //Serial.println("Anzahl von Spannungswerten=" + String((sizeof(spannungsTabelle)/sizeof(int))));
  if(rohWert >= indexwert && rohWert <= spannungsTabelle[(sizeof(spannungsTabelle)/sizeof(int))-1]){          // rawValue ist unter 20V oder ueber den maximalWert
    while(index < (sizeof(spannungsTabelle)/sizeof(int))){
      index = index + 1;
      //Serial.println("sizeof(spannungsTabelle)=" + String((sizeof(spannungsTabelle)/sizeof(int))));
      if(index >= (sizeof(spannungsTabelle)/sizeof(int))){  //Wenn der gemessene Wert den maximal Wert der Spannungstabelle uebersteigt
        Serial.println("Fehler beim Finden des Spannungswertes.");
        //break; 
        return -1.0; 
      }
      else{
        indexwert = spannungsTabelle[index];
        if(rohWert <= indexwert){   //sobald der Wert aus der Tabelle groesser ist als der rawValue stoppt die Suche
          //Serial.println("Value(" + String(rawValue) + "); kleinerer Spannungswert(" + String(index-1) + ")='" + String(spannungsTabelle[index-1]) + "'; höherer Spannungswert(" + String(index) + ")='" + String(spannungsTabelle[index]) + "';");
          kommaWert = prozentWert(rohWert, spannungsTabelle[index-1], spannungsTabelle[index]);   //Erwarteter Wert ist 0 bis 1
          //Serial.println("Prozentwert=" + String(spannung) + " index=" + String(index-1));
          if(kommaWert >= 0.0 && kommaWert <= 1.0){
            //Serial.println("Berechung: Grundwert=" + String(spannungsabfall) + "V + " + String(index-1) + "V + " + String(kommaWert) + "V" );
            spannung = spannungsabfall + (index-1) + kommaWert ;    // Zehner + Einer + Kommawert => Spannungswert
            ergebnisSpannung = spannung;
            return spannung;
          }
          break;
        }
      }
    }
  }
  else{
    if(rohWert < indexwert){
            Serial.println("Spannung ist kleiner als die Mindestspannung von " + String(spannungsabfall) + "V. Messung nicht mögliche.");
      zustand = -2;
      return (spannungsabfall * -1);
    }
        if(rohWert > indexwert){
            Serial.println("ACHTUNG! Spannung ist zu hoch. Maximalspanung von " + String(spannungsabfall + (sizeof(spannungsTabelle)/sizeof(int))) + "V ist überschritten. Messung nicht möglich.");
      zustand = -1;
      return (spannungsabfall + ((sizeof(spannungsTabelle)/sizeof(int)) - 1));
    }
  }
  Serial.println("Fehler beim Finden des Spannungswertes.");
  return -1.0;
}

/**
 * Methode wandelt Float-Zahlen in ein String/Char-Kette um
 */
char *floatToString(float zahl){
  static char ausgabe[50];
  sprintf(ausgabe, "%2.5f", zahl);
  return ausgabe;
}

/**
 * Methode startet Funktionen je nachdem in welchem Zustand das Programm ist
 */
void prozess(){
  float spannung = 0;
  switch(zustand){
    case 0:     //Programm wartet auf Input
      led1_gruen_On= true; 
      led2_rot_On = false;
      led3_gelb_On = false;   
      break;
    case 1:     //Testmessungen werden vorgenommen
      led1_gruen_On= false; 
      led2_rot_On = false;
      led3_gelb_On = true;    
      if(!phaseMessung()){
        zustand = 0;
        messungBeendet = true;
        Serial.println("Messung wurden abgeschlossen.");
        spannung = ergebnisBerechnung();
        Serial.println("Ergebnis der Spannung= " + String(spannung) + "V.");        
        }
      break;
    case 2: 
      led1_gruen_On= true; 
      led2_rot_On = false;
      led3_gelb_On = true;
      if(!phaseMessung()){
        spannung = ergebnisBerechnung();       
        messungReset();
        Serial.println("Ergebnis der Spannung= " + String(spannung) + "V.");
        }
      break; 
  case -1:  //Fehlermeldung, Messung unterbrochen. Ueberhoehte Spannung ermittelt. 
      led1_gruen_On= false; 
      led2_rot_On = true;
      led3_gelb_On = false;
      break;
  case -2:  //Fehlermeldung, Messung unterbrochen. Spannung zu niedrig. 
      led1_gruen_On= false; 
      led2_rot_On = true;
      led3_gelb_On = false;
      break;
  }  
}


 /**
 * Input von den Button veraendern den Zustand
 */
void buttonSteuerung(){
  if(button_veraenderung){
      switch(zustand){
        case 0:         
          if(button1_press){
            //Serial.println("Button1(Z0) wurde gedrückt.");
            Serial.println("Messung wird gestartet. Bitte warten...");
            messungReset();
            zustand = 1;
          }
          if(button2_press){
            //Serial.println("Button2(Z0) wurde gedrückt.");
            if(messungBeendet){
              float spannung = ergebnisBerechnung();
              Serial.println("Ergebnis der Spannung= " + String(spannung) + "V.");
            }
            else{
              Serial.println("Es wurde noch keine Messung vorgenommen.");  
            }
          }          
          if(button3_press){
           //Serial.println("Button3(Z0) wurde gedrückt.");
            Serial.println("Dauerhafte überprüfung startet.");                     
            messungReset();
            zustand = 2;
          }
          break;
        case 1:
          if(button1_press){
            Serial.println("Button1(Z1) wurde gedrückt.");    
          }
          if(button2_press){
            Serial.println("Button2(Z1) wurde gedrückt.");
          }          
          if(button3_press){
            Serial.println("Button3(Z1) wurde gedrückt.");
          }
          break;
        case 2:
          if(button1_press){       
            Serial.println("Button1(Z2) wurde gedrückt.");
          }
          if(button2_press){
            Serial.println("Button2(Z2) wurde gedrückt.");
          }          
          if(button3_press){
            Serial.println("Button3(Z2) wurde gedrückt.");
            Serial.println("Dauerhafte überprüfung beendet.");
            zustand = 0;
          }
        break; 
        case -1:  //Ueberhohte Spannung ermittelt
          if(button1_press){
            Serial.println("Button1(Z-1) wurde gedrückt.");
            Serial.println("Zustand wird zurueckgesetzt"); 
            zustand = 0;      
          }
          if(button2_press){
            Serial.println("Button2(Z-1) wurde gedrückt.");
          }          
          if(button3_press){
            Serial.println("Button3(Z-1) wurde gedrückt.");
          }
        break;    
        case -2: //Niedrige Spannung ermittelt
          if(button1_press){       
            Serial.println("Button1(Z-2) wurde gedrückt.");
            Serial.println("Zustand wird zurueckgesetzt"); 
            zustand = 0;    
          }
          if(button2_press){
            Serial.println("Button2(Z-2) wurde gedrückt.");
          }          
          if(button3_press){
            Serial.println("Button3(Z-2) wurde gedrückt.");
          }
        break; 
      }
    }    
}


void loop() {
  butUpdate();
  buttonSteuerung();
  ledUpdate();
  
  prozess();

  butReset();
  delay(waitTime);
}