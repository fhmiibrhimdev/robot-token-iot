const int sol1 = 7;
const int sol2 = 6;
const int sol3 = 5;

const int stp1 = 8;
const int stp2 = 10;
const int stp3 = 9;
const int stp4 = 11;

const int limitSw = 4;

#include <Stepper.h>
const int stepsPerRevolution = 64;
Stepper myStepper(stepsPerRevolution, stp1, stp2, stp3, stp4);

const int delayStp = 2;

const int ztoRow4 = 1550; //1 2 3
const int ztoRow3 = 1100; //4 5 6 
const int ztoRow2 = 600;  //7 8 9
const int ztoRow1 = 100;  //< 0 >

const int ztoRow[] = {0, ztoRow1, ztoRow2, ztoRow3, ztoRow4};
unsigned int rowPost;
unsigned int rowDest;
unsigned int stpToRow;

const String idCmd = "go";
const int numIdCmd = 2;
String inputString = "";         // a String to hold incoming data
unsigned int sumChar = 0;
bool stringComplete = false;  // whether the string is complete

void setup() {
  pinMode(stp1, OUTPUT);
  pinMode(stp2, OUTPUT);
  pinMode(stp3, OUTPUT);
  pinMode(stp4, OUTPUT);

  pinMode(sol1, OUTPUT);
  pinMode(sol2, OUTPUT);
  pinMode(sol3, OUTPUT);

  pinMode(limitSw, INPUT_PULLUP);

  stpResetPoint();
//  stpMoveUp(200);

  Serial.begin(115200);
  Serial.println("TOKENIZATION");
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  Serial.println(ztoRow[3]);
}

void loop() {
  
  if (stringComplete) {
    Serial.print(inputString);
    if (inputString.indexOf(idCmd) != -1) {
      Serial.println("MOVING");
      //Serial.println(sumChar);
      //Serial.println(inputString[sumChar-1]); // sumChar - 1 = last char
      for(unsigned int i = numIdCmd; i < sumChar; i++) {
//        sbuff = inputString[i];
//        gotoRow(sbuff.toInt());  
//        pressPin(sbuff.toInt());      
        gotoRow(inputString[i]);  
        pressPin(inputString[i]);      
      }
      Serial.println("DONE");
      Serial.println("GO TO RESET");
      stpResetPoint();
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
    sumChar = 0;
  }
}

void pressPin(unsigned int pinPress) {
  delay(100);
  if ((pinPress == '1') || (pinPress == '4') || (pinPress == '7') || (pinPress == '<')) {
    digitalWrite(sol1, HIGH);
  } else if ((pinPress == '2') || (pinPress == '5') || (pinPress == '8') || (pinPress == '0')) {
    digitalWrite(sol2, HIGH);
  } else if ((pinPress == '3') || (pinPress == '6') || (pinPress == '9') || (pinPress == '>')) {
    digitalWrite(sol3, HIGH);
  }
  delay(200);
  digitalWrite(sol1, LOW);
  digitalWrite(sol2, LOW);
  digitalWrite(sol3, LOW);
  delay(200);  
}

void gotoRow(unsigned int pinPress) {
  Serial.print("Pin to Press:"); Serial.println(char(pinPress));
  rowDest = getRowDest(pinPress);
//  Serial.print("Post Row:"); Serial.println(rowPost);
//  Serial.print("Dest Row:"); Serial.println(rowDest);
//  Serial.print("Post-Zero:"); Serial.println(ztoRow[rowPost]);
//  Serial.print("Dest-Zero:"); Serial.println(ztoRow[rowDest]);
//  Serial.print("Dest-Post:"); Serial.println(abs(ztoRow[rowPost]-ztoRow[rowDest]));
  stpToRow = abs(ztoRow[rowPost] - ztoRow[rowDest]);
//  Serial.print("Dest-Post:"); Serial.println(stpToRow);

  if(stpToRow != 0) {
    if(rowDest > rowPost) {
      stpMoveUp(stpToRow);
    } else {
      stpMoveDw(stpToRow);
    }
  }
  rowPost = rowDest;
}

unsigned int getRowDest(unsigned int rowBuff) {
  if ((rowBuff == '1') || (rowBuff == '2') || (rowBuff == '3')) {
    rowBuff = 4;
  } else if ((rowBuff == '4') || (rowBuff == '5') || (rowBuff == '6')) {
    rowBuff = 3;
  } else if ((rowBuff == '7') || (rowBuff == '8') || (rowBuff == '9')) {
    rowBuff = 2;
  } else if ((rowBuff == '<') || (rowBuff == '0') || (rowBuff == '>')) {
    rowBuff = 1;
  } else {
    rowBuff = 0;
  } 
  return rowBuff;
}

//==============================
//======STEPPER SUBPROGRAM======
//==============================
void stpResetPoint() {
  if(digitalRead(limitSw) != 0) {
    while(digitalRead(limitSw) != 0) {
      stpDw();
    }
    stpOff();
  }
  rowPost = 0;
}

void stpMoveUp(unsigned int nStep) {
  for (int i = 0; i < nStep; i++){
    stpUp();
  }
  stpOff();
}

void stpMoveDw(unsigned int nStep) {
  for (int i = 0; i < nStep; i++){
    stpDw();
  }
  stpOff();
}

void stpUp() {
  myStepper.step(-1);
  delay(delayStp);
}

void stpDw() {
  myStepper.step(1);
  delay(delayStp);
}

void stpOff() {
  digitalWrite(stp1, LOW);
  digitalWrite(stp2, LOW);
  digitalWrite(stp3, LOW);
  digitalWrite(stp4, LOW);
  delay(delayStp);
}
//END OF SUBPROGRAM

//==============================
//======SERIAL SUBPROGRAM======
//==============================
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    else {
      sumChar++;
    }
  }
}
//END OF SUBPROGRAM
