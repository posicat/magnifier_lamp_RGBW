int lastRot = 0;
int firstBut = 0;
int clickArray[2]={0,-1};
#define ARR_ROT  0
#define ARR_BUT  1

void ICACHE_RAM_ATTR handleISR_Rotor() {
  int t = millis();
  int rA = digitalRead(ROT_PIN_OUTPUT_A);
  int rB = digitalRead(ROT_PIN_OUTPUT_B);
  int dT = t-lastRot;

  //debug("handleISR_Rotor " + String(rA) + String(rB)+ " " + String(dT));

  if (dT < 10) { // Too fast a click, probably noise
    return;
  }
  if (dT > 400) { // Tool long between clicks, must be a new set of clicks.
    lastRot=t;
    dT=0;
  }

  if ( (rA==1) ) { // Start of a pulse
    if ( (rB==0) ) { // Direction indicated by second pin
      clickArray[ARR_ROT]=-1;
    }else{
      clickArray[ARR_ROT]=+1;
    }
    
  }else{ // Done moving
    callbackMovement(clickArray[ARR_ROT],dT);
    clickArray[ARR_ROT]=0;
    lastRot=millis();
  }
}

void ICACHE_RAM_ATTR handleISR_Button() {
  int t = millis();
  int r = digitalRead(ROT_PIN_PUSH_BUT);
  int dT = t-firstBut;

  debug("handleISR_Button" + String(r) + " " + String(dT));

  if (dT < 60) { // Too fast a click, probably noise
    return;
  }
  if ( r==1 ) { // Start of click
    firstBut=t;
    dT=0;
    clickArray[ARR_ROT]=1;
  } else { // End of click
    if (clickArray[ARR_ROT] == 1) { // Make sure we only do this once
      callbackButton(dT);
      clickArray[ARR_ROT]=0;
    }
  }
}

void setup_encoder() {
  pinMode(ROT_PIN_OUTPUT_A, INPUT);
  pinMode(ROT_PIN_OUTPUT_B, INPUT);
  pinMode(ROT_PIN_PUSH_BUT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_OUTPUT_A), handleISR_Rotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_PUSH_BUT), handleISR_Button, CHANGE);
}
