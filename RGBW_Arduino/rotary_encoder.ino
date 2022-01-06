int firstRot = 0;
int firstBut = 0;
int clickArray[3]={-1,-1,-1};
#define ARR_ROT0  0
#define ARR_ROT1  1
#define ARR_BUT  2

void ICACHE_RAM_ATTR handleISR_Rotor() {
  int t = millis();
  int dT = t-firstRot;
  if (dT > 100) {
    firstRot=t;
    dT=0;
  }
  int rA = digitalRead(ROT_PIN_OUTPUT_A);
  int rB = digitalRead(ROT_PIN_OUTPUT_B);

  if ( (rA==1) && (clickArray[ARR_ROT0]==-1) ) {
    clickArray[0]=dT;
  }
  if ( (rB==1) && (clickArray[1]==-1) ) {
    clickArray[1]=dT;
  }

  if ( (rA==0) && (rB==0) ) {
    // Done moving
    //debug("Completed movements : " 
    //  + String(clickArray[ARR_ROT0]) + " " 
    //  + String(clickArray[ARR_ROT1]) + " "
    //  + String(dT) + " "
    //);
    if (clickArray[ARR_ROT0] < clickArray[ARR_ROT1]) {
      callbackMovement(+1,dT);
    }
    if (clickArray[ARR_ROT0] > clickArray[ARR_ROT1]) {
      callbackMovement(-1,dT);
    }
    clickArray[ARR_ROT0]=-1;
    clickArray[ARR_ROT1]=-1;
  }
  
  //debug("handleISR_Rotor " + String(rA) + String(rB)+ " " + String(dT));
}

void ICACHE_RAM_ATTR handleISR_Button() {
  int t = millis();
  int dT = t-firstBut;
  if (dT > 1000) {
    firstBut=t;
    dT=0;
  }
  int r = digitalRead(ROT_PIN_PUSH_BUT);

  if ( (r==1) && (clickArray[ARR_BUT]==-1) ) {
    clickArray[ARR_BUT]=0;
    firstBut=t;
  }

  if ( (r==0) && (clickArray[ARR_BUT]>-1) ) {
    callbackButton(dT);
    clickArray[2]=-1;
  }

  //debug("handleISR_Button" + String(r) + " " + String(dT));
}

void setup_encoder() {
  pinMode(ROT_PIN_OUTPUT_A, INPUT);
  pinMode(ROT_PIN_OUTPUT_B, INPUT);
  pinMode(ROT_PIN_PUSH_BUT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_OUTPUT_A), handleISR_Rotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_OUTPUT_B), handleISR_Rotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_PUSH_BUT), handleISR_Button, CHANGE);
}
