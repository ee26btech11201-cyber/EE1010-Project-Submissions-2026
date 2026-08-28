
// ---------------- Shared 7447 Inputs ----------------
const int PIN_A = 0, PIN_B = 1, PIN_C = 2, PIN_D = 3;

// ---------------- Enable pins for multiplexing ----------------
const int EN[] = {4, 5, 6, 7, 8, 9};  // sec1, sec10, min1, min10, hr1, hr10

// ---------------- Buttons ----------------
const int PAUSE_BTN = 10;
const int ALARM_BTN = 13;
const int NEXT_BTN = 11;
const int INC_BTN = 12;

bool paused = false;
bool AlarmMode = false;

int lastPauseState = HIGH;
int lastAlarmState = HIGH;
int lastNextState = HIGH;
int lastIncState = HIGH;

int selectedDigit = 0;
unsigned long lastBlink = 0;
bool blinkOn = true;

//---------------- Alarm flashing state--------------------
bool alarmRinging = false;
bool alarmTimeLatched = false;
unsigned long lastAlarmFlash = 0;
bool alarmShowEight = false;

// ---------------- Current clock state ----------------
int W1 = 0, X1 = 0, Y1 = 0, Z1 = 0;
int W2 = 0, X2 = 0, Y2 = 0;
int W3 = 0, X3 = 0, Y3 = 0, Z3 = 0;
int W4 = 0, X4 = 0, Y4 = 0;
int W5 = 0, X5 = 1, Y5 = 1, Z5 = 0;
int W6 = 0, X6 = 0;

// ---------------- Alarm state ----------------
int L1 = 0, M1 = 0, N1 = 0, O1 = 0;
int L2 = 0, M2 = 0, N2 = 0;
int L3 = 0, M3 = 0, N3 = 0, O3 = 0;
int L4 = 1, M4 = 1, N4 = 0;
int L5 = 0, M5 = 1, N5 = 1, O5 = 0;
int L6 = 0, M6 = 0;

unsigned long lastSecUpdate = 0;

void showDigit(int A, int B, int C, int D, int ENpin);
bool timeMatchesAlarm();

//------------------Setup----------------------------
void setup() {
  pinMode(PIN_A, OUTPUT); pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT); pinMode(PIN_D, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(EN[i], OUTPUT);
    digitalWrite(EN[i], LOW);
  }
  pinMode(PAUSE_BTN, INPUT_PULLUP);
  pinMode(NEXT_BTN, INPUT_PULLUP);
  pinMode(INC_BTN, INPUT_PULLUP);
  pinMode(ALARM_BTN, INPUT_PULLUP);
}

//---------------------Increment Logic---------------------------
void incrementDigit(int d) {
  int A, B, C, D;
  switch (d) {
    case 0:
      A = !W1;
      B = (W1 && !X1 && !Z1) || (!W1 && X1);
      C = (!X1 && Y1) || (!W1 && Y1) || (W1 && X1 && !Y1);
      D = (!W1 && Z1) || (W1 && X1 && Y1);
      W1 = A; X1 = B; Y1 = C; Z1 = D;
      break;
    case 1:
      A = !W2;
      B = (W2 && !X2 && !Y2) || (!W2 && X2);
      C = (W2 && X2) || (!W2 && !X2 && Y2);
      W2 = A; X2 = B; Y2 = C;
      break;
    case 2:
      A = !W3;
      B = (W3 && !X3 && !Z3) || (!W3 && X3);
      C = (!X3 && Y3) || (!W3 && Y3) || (W3 && X3 && !Y3);
      D = (!W3 && Z3) || (W3 && X3 && Y3);
      W3 = A; X3 = B; Y3 = C; Z3 = D;
      break;
    case 3:
      A = !W4;
      B = (W4 && !X4 && !Y4) || (!W4 && X4);
      C = (W4 && X4) || (!W4 && !X4 && Y4);
      W4 = A; X4 = B; Y4 = C;
      break;
    case 4:
      if (X6 == 0) {
        A = !W5;
        B = (W5 && !X5 && !Z5) || (!W5 && X5);
        C = (!X5 && Y5) || (!W5 && Y5) || (W5 && X5 && !Y5);
        D = (!W5 && Z5) || (W5 && X5 && Y5);
        W5 = A; X5 = B; Y5 = C; Z5 = D;
      } else {
        A = !W5;
        B = (W5 && !X5) || (!W5 && X5);
        W5 = A; X5 = B; Y5 = 0; Z5 = 0;
      }
      break;
    case 5:
      if (W6 == 1 && X6 == 0 && (Y5 == 1 || Z5 == 1)){
		  break;    // Does not proceed if tried to enter invalid time
	  }
      A = !W6 && !X6;
      B = W6 && !X6;
      W6 = A; X6 = B;
      break;
  }
}

//-----------------Increment logic for Alarm----------------------------
void ALARM_incrementDigit(int d) {
  int A, B, C, D;
  switch (d) {
    case 0:
      A = !L1;
      B = (L1 && !M1 && !O1) || (!L1 && M1);
      C = (!M1 && N1) || (!L1 && N1) || (L1 && M1 && !N1);
      D = (!L1 && O1) || (L1 && M1 && N1);
      L1 = A; M1 = B; N1 = C; O1 = D;
      break;
    case 1:
      A = !L2;
      B = (L2 && !M2 && !N2) || (!L2 && M2);
      C = (L2 && M2) || (!L2 && !M2 && N2);
      L2 = A; M2 = B; N2 = C;
      break;
    case 2:
      A = !L3;
      B = (L3 && !M3 && !O3) || (!L3 && M3);
      C = (!M3 && N3) || (!L3 && N3) || (L3 && M3 && !N3);
      D = (!L3 && O3) || (L3 && M3 && N3);
      L3 = A; M3 = B; N3 = C; O3 = D;
      break;
    case 3:
      A = !L4;
      B = (L4 && !M4 && !N4) || (!L4 && M4);
      C = (L4 && M4) || (!L4 && !M4 && N4);
      L4 = A; M4 = B; N4 = C;
      break;
    case 4:
      if (M6 == 0) {
        A = !L5;
        B = (L5 && !M5 && !O5) || (!L5 && M5);
        C = (!M5 && N5) || (!L5 && N5) || (L5 && M5 && !N5);
        D = (!L5 && O5) || (L5 && M5 && N5);
        L5 = A; M5 = B; N5 = C; O5 = D;
      } else {
        A = !L5;
        B = (L5 && !M5) || (!L5 && M5);
        L5 = A; M5 = B; N5 = 0; O5 = 0;
      }
      break;
    case 5:
      if (L6 == 1 && M6 == 0 && (N5 == 1 || O5 == 1)){
		  break;    // Does not proceed if tried to enter invalid time
	  }
	A = !L6 && !M6;
      B = L6 && !M6;
      L6 = A; M6 = B;
      break;
  }
}

// ----------------Function for checking for alarm--------------------------
bool timeMatchesAlarm() {
  return W1 == L1 && X1 == M1 && Y1 == N1 && Z1 == O1 &&
         W2 == L2 && X2 == M2 && Y2 == N2 &&
         W3 == L3 && X3 == M3 && Y3 == N3 && Z3 == O3 &&
         W4 == L4 && X4 == M4 && Y4 == N4 &&
         W5 == L5 && X5 == M5 && Y5 == N5 && Z5 == O5 &&
         W6 == L6 && X6 == M6;
}
//******************************************************************************

//---------------------------------MAIN LOOP---------------------------------

//********************************************************************************
void loop() {
	//-------------------Pause Button----------------------
  int pauseState = digitalRead(PAUSE_BTN);
  if (pauseState == LOW && lastPauseState == HIGH) {
    paused = !paused;
    if (paused) selectedDigit = 0;
    delay(30);
  }
  lastPauseState = pauseState;

//----------------------Next Button-------------------------
  int nextState = digitalRead(NEXT_BTN);
  if ((paused || AlarmMode) && nextState == LOW && lastNextState == HIGH) {
    selectedDigit = (selectedDigit + 1) % 6;
    if (!blinkOn) {
      lastBlink = millis();
      blinkOn = true;
    }
    delay(30);
  }
  lastNextState = nextState;

//-----------------------Increment button---------------------
  int incState = digitalRead(INC_BTN);
  if (paused && incState == LOW && lastIncState == HIGH) {
    incrementDigit(selectedDigit);
    if (!blinkOn) {
      lastBlink = millis();
      blinkOn = true;
    }
    delay(30);
  } else if (AlarmMode && incState == LOW && lastIncState == HIGH) {
    ALARM_incrementDigit(selectedDigit);
    if (!blinkOn) {
      lastBlink = millis();
      blinkOn = true;
    }
    delay(30);
  }
  lastIncState = incState;

//-----------------------------Alarm Button-------------------------------
  int AlarmState = digitalRead(ALARM_BTN);
  if (alarmRinging && AlarmState == LOW && lastAlarmState == HIGH) {
    alarmRinging = false;
    delay(30);
  } else if (!paused && AlarmState == LOW && lastAlarmState == HIGH) {
    AlarmMode = !AlarmMode;
    if (AlarmMode) selectedDigit = 0;
    delay(30);
  }
  lastAlarmState = AlarmState;

  if ((paused || AlarmMode) && millis() - lastBlink >= 500) {
    blinkOn = !blinkOn;
    lastBlink = millis();
  }

  if (alarmRinging && millis() - lastAlarmFlash >= 500) {
    alarmShowEight = !alarmShowEight;
    lastAlarmFlash = millis();
  }

  if (alarmRinging) {
    // Display 000000 and 888888 alternately while the alarm is ringing
    for (int d = 0; d < 6; d++) {
      showDigit(0, 0, 0, alarmShowEight ? 1 : 0, EN[d]);
      delay(1);
    }
  } else if (!AlarmMode) {
    for (int d = 0; d < 6; d++) {
      bool showThis = !(paused && d == selectedDigit && !blinkOn);
      if (showThis) {
        switch (d) {
          case 0: showDigit(W1, X1, Y1, Z1, EN[0]); break;
          case 1: showDigit(W2, X2, Y2, 0, EN[1]); break;
          case 2: showDigit(W3, X3, Y3, Z3, EN[2]); break;
          case 3: showDigit(W4, X4, Y4, 0, EN[3]); break;
          case 4: showDigit(W5, X5, Y5, Z5, EN[4]); break;
          case 5: showDigit(W6, X6, 0, 0, EN[5]); break;
        }
        delay(1);
      }
    }
  } else {
    for (int d = 0; d < 6; d++) {
      bool showThis = !(d == selectedDigit && !blinkOn);
      if (showThis) {
        switch (d) {
          case 0: showDigit(L1, M1, N1, O1, EN[0]); break;
          case 1: showDigit(L2, M2, N2, 0, EN[1]); break;
          case 2: showDigit(L3, M3, N3, O3, EN[2]); break;
          case 3: showDigit(L4, M4, N4, 0, EN[3]); break;
          case 4: showDigit(L5, M5, N5, O5, EN[4]); break;
          case 5: showDigit(L6, M6, 0, 0, EN[5]); break;
        }
        delay(1);
      }
    }
  }

//---------------------- Time Update ------------------------------
  if (!paused && millis() - lastSecUpdate >= 1000) {
    lastSecUpdate = millis();

    int A = !W1;
    int B = (W1 && !X1 && !Z1) || (!W1 && X1);
    int C = (!X1 && Y1) || (!W1 && Y1) || (W1 && X1 && !Y1);
    int D = (!W1 && Z1) || (W1 && X1 && Y1);
    W1 = A; X1 = B; Y1 = C; Z1 = D;

    if ((W1 | X1 | Y1 | Z1) == 0) {
      A = !W2;
      B = (W2 && !X2 && !Y2) || (!W2 && X2);
      C = (W2 && X2) || (!W2 && !X2 && Y2);
      W2 = A; X2 = B; Y2 = C;
    }

    if ((W1 | X1 | Y1 | Z1 | W2 | X2 | Y2) == 0) {
      A = !W3;
      B = (W3 && !X3 && !Z3) || (!W3 && X3);
      C = (!X3 && Y3) || (!W3 && Y3) || (W3 && X3 && !Y3);
      D = (!W3 && Z3) || (W3 && X3 && Y3);
      W3 = A; X3 = B; Y3 = C; Z3 = D;

      if ((W3 | X3 | Y3 | Z3) == 0) {
        A = !W4;
        B = (W4 && !X4 && !Y4) || (!W4 && X4);
        C = (W4 && X4) || (!W4 && !X4 && Y4);
        W4 = A; X4 = B; Y4 = C;
      }

      if ((W3 | X3 | Y3 | Z3 | W4 | X4 | Y4) == 0) {
        if (X6 == 0) {
          A = !W5;
          B = (W5 && !X5 && !Z5) || (!W5 && X5);
          C = (!X5 && Y5) || (!W5 && Y5) || (W5 && X5 && !Y5);
          D = (!W5 && Z5) || (W5 && X5 && Y5);
          W5 = A; X5 = B; Y5 = C; Z5 = D;
        } else {
          A = !W5;
          B = (W5 && !X5) || (!W5 && X5);
          W5 = A; X5 = B; Y5 = 0; Z5 = 0;
        }

        if ((W5 | X5 | Y5 | Z5) == 0) {
          A = !W6 && !X6;
          B = W6 && !X6;
          W6 = A; X6 = B;
        }
      }
    }
  }

//-----------------Enables Alarm Display-------------------
  bool alarmMatches = timeMatchesAlarm();
  if (!alarmMatches) {
    alarmTimeLatched = false;
  } else if (!paused && !AlarmMode && !alarmTimeLatched) {
    alarmRinging = true;
    alarmTimeLatched = true;
    lastAlarmFlash = millis();
    alarmShowEight = false;
  }
}

void showDigit(int A, int B, int C, int D, int ENpin) {
  digitalWrite(PIN_A, A); digitalWrite(PIN_B, B);
  digitalWrite(PIN_C, C); digitalWrite(PIN_D, D);
  for (int i = 0; i < 6; i++) digitalWrite(EN[i], LOW);
  digitalWrite(ENpin, HIGH);
}
