#define PIN_INPUT_V 34    //Pin 34

/*ESP-Einstellungen*/
const int waitTime = 1;     /*Wartezeit bis der Zyklus sich wiederholt*/

/*Led-Einrichtung*/
const int led1_gruen = 12;  /* Pinnummer des Ausgangs*/
bool led1_On = false;       /* Aktueller Zustand des Ausgangs*/
const int led2_rot = 14;
bool led2_On = false;

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
int spannungsTabelle[9] = {0};        /*Spannungstabelle: Index+20 ist die Spannung und der Wert ist der ideal-Messwert*/
bool messungBeendet = false;        /*Sobald Messungen vorgenommen wurde wird dieser Wert auf true gesetzt*/
float ergebnisSpannung = 0.0;       /*Hier wird die berechnete Spannung abgelegt*/

void setup() {
  Serial.begin(9600); //Baudrate fuer die Ausgabe der Konsole
  pinMode(PIN_INPUT_V, INPUT);  //Deklarierung zum Eingang?
  /*LEDs werden Regstriert*/
  pinMode(led1_gruen, OUTPUT);
  pinMode(led2_rot, OUTPUT);
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
  if(led1_On){
    digitalWrite(led1_gruen, HIGH);
  }
  else{
    digitalWrite(led1_gruen, LOW);      
  }
  if(led2_On){
    digitalWrite(led2_rot, HIGH);
  }
  else{
    digitalWrite(led2_rot, LOW);      
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
    //Serial.println("Messung von rawValue=" + String(rawValue) + " bei " + String(aktuelleSpannung) + "V");
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
      erg = ((float)value - valueStart) / (float)range;
      //Serial.println("Ergebnis vom prozentWert():" + String(valueEnd) + "-" + String(valueStart) + "=" + String(range) + "=> das sind 100%;");
      //Serial.println("float_erg:" + String(erg) + "=((float)value - valueStart):" + String((float)value - valueStart) + " / (float)range:" + String((float)range) + ";");
      return erg;
  }
  return -1.0;
}

/**
 * Wandelt den am häufigsten erhaltenen Wert in eine Spannung um
 */
float ergebnisBerechnung(){
  int rawValue = 0;
  int index = 0;
  int indexwert = 0;
  float spannung = 0.0;
  
  rawValue = bestMessWert();
  //Serial.println("bester Wert rawValue:" + String(rawValue));
  indexwert = spannungsTabelle[index];
  if(rawValue > indexwert){   //rawValue ist unter 20V
    while(true){    
      index = index + 1;
      indexwert = spannungsTabelle[index];
      if(rawValue < indexwert){   //sobald der Wert aus der Tabelle groesser ist als der rawValue stoppt die Suche
        //Serial.println("Value(" + String(rawValue) + "); kleinerer Spannungswert(" + String(index-1) + ")='" + String(spannungsTabelle[index-1]) + "'; höherer Spannungswert(" + String(index) + ")='" + String(spannungsTabelle[index]) + "';");
        spannung = prozentWert(rawValue, spannungsTabelle[index-1], spannungsTabelle[index]);   //Erwarteter Wert ist 0 bis 1
        //Serial.println("Prozentwert=" + String(spannung) + " index=" + String(index-1));
        if(spannung >= 0 && spannung <= 1){        
          spannung = spannung + 20 + (index-1);    // Da der Index die Spannung im ganzzahligen Bereich darstellt, wird dieser Bereich hinzugefügt.
          ergebnisSpannung = spannung;
          return spannung;
        }
        break;
      }
      if((index +1) >= sizeof(spannungsTabelle)){
        Serial.println("Fehler beim Finden des Spannungswertes.");
        break; 
        return -1.0; 
      }
    }
  }
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
      led1_On= true; 
      led2_On = true; 
      break;
    case 1:     //Testmessungen werden vorgenommen
      led1_On= false; led2_On = true; 
      if(!phaseMessung()){
        zustand = 0;
        messungBeendet = true;
        Serial.println("Messung wurden abgeschlossen.");
        spannung = ergebnisBerechnung();
        Serial.println("Ergebnis der Spannung= " + String(spannung) + "V.");        
        }
      break;
    case 2: 
      led1_On= true; 
      led2_On = false; 
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
            //erial.println("Button1(Z0) wurde gedrückt.");
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
