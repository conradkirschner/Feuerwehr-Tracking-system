#define PIN_INPUT_V 34    //Pin 34

/*ESP-Einstellungen*/
const int waitTime = 1;     /* Wartezeit bis der Zyklus sich wiederholt*/
const int sleepTime = 1000;	/* Zeit zwischen den Messungen und Senden des Spannungsergebnisse*/

/*Messbereiche*/
int zustand = 0;                  /*|0=Wartend|1=Messung laeuft|2=Ende|-1=Ueberspannung|-2=Unterspannung|*/
int counter = 0;                  /*Zaehler fuer den Wert*/
const int messAnzahl = 3000;        /*Anzahl von Messungen */
const int moeglicheErg = 4095;
int messung[moeglicheErg] = {0};     /*Array mit den Messungen: |Value|Anzahl|*/
int spannungsTabelle[11] = {0};        /*Spannungstabelle: Index+20 ist die Spannung und der Wert ist der ideal-Messwert*/
bool messungErfolgreich = false;        /*Wenn Messung erfolgreich wird dieser Wert True gesetzt, nach dem Senden der Information wieder aus false*/
const float spannungsabfall = 20.0;    /*Spannung die ueber die Schaltung abfaellt*/
float ergebnisSpannung = 0.0;       /*Hier wird die berechnete Spannung abgelegt*/

void setup() {
  Serial.begin(9600); //Baudrate fuer die Ausgabe der Konsole
  pinMode(PIN_INPUT_V, INPUT);  //Deklarierung zum Eingang

  vTabEinrichtung();	//Werte der Spannungsmessung wird eingerichtet
}

/**
 * Daten der Spannungswerte werden eingerichtet
 * Der Index steht für die Einerstelle der Spannung
 * Example: Index= 3 => 3V + Grundspassung(20V)
 * Example2:   
 * ...spannungsTabelle[3] = 2559;
 * spannungsTabelle[4] = 2679;...
 * Wird ein Wert gemessen(2439) ergibt dies eine Spannung zwischen >22V und <23V, da es über den Indexwert 2432 und unter 2559 liegt. 
 * Des weiteren wird, danach über eine lineare Funktion ein Prozentwert mit dem Differenzwert berechnet um die 2Kommastellen der Spannung zu berechnen.
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
  spannungsTabelle[9] = 3331;     //Schaetzwerte, da Testmessung war nur bis 28V moeglich
  spannungsTabelle[10] = 3474;    //Schaetzwerte, da Testmessung war nur bis 28V moeglich
  }

/**
 * Messwert wird hinzugefuegt
 * Funktion erstellt Messungen und zaehlt den counter hoch
 * Wenn eine Anzahl vom Counter erreicht wurde wird false zurueck gegeben. Dies dient zur Signaisierung, dass ein Messzyklus abgeschlossen wurde.
 */
bool phaseMessung(){  
  if(counter < messAnzahl){
    int rawValue = analogRead(PIN_INPUT_V);	//Spannungsmessung erfolgt hier
    //Serial.println("Messung(" + String(counter) + "/" + String(messAnzahl) + ") von rawValue=" + String(rawValue));
    messung[rawValue] = messung[rawValue] + 1;
    counter++;
    return true;
  }
  else{
    counter = 0;
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
 * Als Ergebnis werden die Nachkommazahl der Spannungsergebnis zuruech gegegen
 */
float prozentWert(int value, int valueStart, int valueEnd){
  int range = 0;
  float erg = 0;  
  if(valueEnd > value && value >= valueStart){ 
      range = valueEnd - valueStart;
      erg = ((float)value - (float)valueStart) / (float)range;
      //Serial.println("Ergebnis vom prozentWert():" + String(valueEnd) + "-" + String(valueStart) + "=" + String(range) + "=> das sind 100%;");
      //Serial.println("float_erg:" + String(erg) + "=((float)value - valueStart):" + String((float)value - valueStart) + " / (float)range:" + String((float)range) + ";");
      return erg;
  }
  return -1.0;
}

/**
 * Wandelt den am häufigsten erhaltenen Messwert in einen Spannungswert um
 */
float ergebnisBerechnung(){
  int rohWert = 0;		// Wert der am ESP-Eingang gemessen wurde
  int index = 0;		// index = Einerstelle des Spannungswertes
  int indexwert = 0;	// Wert die der ESP bei einer bestimmten Spannung misst
  float spannung = 0.0; // Ergebnis der Spannungsberechnung
  float kommaWert = 0.0;// Nachkommazahl der Spannungsmessung
  
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
  int fehlerWarteZeit = 5000;
  switch(zustand){
    case 0:     //Programm wartet auf Input
		delay(sleepTime);
		zustand = 1;	//Wechsel zum Messzustand
      break;
    case 1:     //Messungsprozess wird ausgefuehrt
      if(!phaseMessung()){
        Serial.println("Messung wurden erfolgreich abgeschlossen.");
		spannung = 0.0;	//Reset des alten Spannungswertes
        spannung = ergebnisBerechnung();
		messungReset();
        Serial.println("Ergebnis der Spannung= " + String(spannung) + "V.");           
		zustand = 2;
        messungErfolgreich = true;     
        }
      break;
    case 2: 	//Senden der Messung
      if(messungErfolgreich){	//Neues Ergebnis wird verschickt
        Serial.println("Senden der Spannung");
		  
        }
		else{	//Es existiert noch kein neues Ergebnis
		Serial.println("Senden einer Fehlermeldung");
		
		}
		zustand = 0;
      break; 
  case -1:  //Fehlermeldung, Messung unterbrochen. Ueberhoehte Spannung ermittelt. 
		Serial.print("FEHLER(-1): Spannung ist zu hoch. ");
		Serial.println("ESP könnte bei eine überhohten Spannung schaden nehmen.");
		delay(fehlerWarteZeit);
		zustand = 0;
      break;
  case -2:  //Fehlermeldung, Messung unterbrochen. Spannung zu niedrig. 
  		Serial.println("FEHLER(-2): Spannung ist zu niedrig. Messung zu ungenau.");		
		delay(fehlerWarteZeit);
		zustand = 0;		
      break;
  }  
}


void loop() {
  
  prozess();
  
  delay(waitTime);
}