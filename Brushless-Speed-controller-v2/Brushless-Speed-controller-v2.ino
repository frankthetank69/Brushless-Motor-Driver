#define HA 2    //bl
#define HB 3   //gr
#define HC 4   //ge

#define A 9     //ge
#define B 10     //gr
#define C 11     //bl

#define INHA 6
#define INHB 7
#define INHC 5

#define THR 0
#define DIR 5

uint8_t state = 0;
uint8_t halState = 0;
uint8_t lastHalState = 0;
uint8_t thr = 0;
bool dir = 0;   //vorwärts
float actSpeed = 0;
float phi_el = 0;
double t_ms = 0;
float dt_ms = 0;


bool commState[6][3] = {{1,0,0},
                      {1,1,0},
                      {0,1,0},
                      {0,1,1},
                      {0,0,1},
                      {1,0,1}};
bool hallAr[3] = {0,0,0};

void readHall();
uint8_t readHallState();
uint8_t commutation(uint8_t i_halState, bool i_dir);
void coilOut(uint8_t state);
float calcPhiEl(uint8_t i_halState, uint8_t i_lastHalState, float i_phiEl);

void setup() {
  pinMode(HA, INPUT_PULLUP);
  pinMode(HB, INPUT_PULLUP);
  pinMode(HC, INPUT_PULLUP);

  pinMode(THR, INPUT);

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);

  pinMode(INHA, OUTPUT);
  pinMode(INHB, OUTPUT);
  pinMode(INHC, OUTPUT);

  digitalWrite(INHA, LOW);
  digitalWrite(INHB, LOW);
  digitalWrite(INHC, LOW);
  delay(1000);
  digitalWrite(INHA, HIGH);
  digitalWrite(INHB, HIGH);
  digitalWrite(INHC, HIGH);

  pinMode(DIR, INPUT_PULLUP);
  Serial.begin(250000);
}

void loop() {
  readHall();
  halState = readHallState();
  //phi_el = calcPhiEl(halState, lastHalState, phi_el);
  //Serial.println(phi_el);

  thr = analogRead(THR)/4;
  dir = !digitalRead(DIR);
  
  state = commutation(halState, dir);
  coilOut(state);
  t_ms = millis();
  lastHalState = halState;
} 

void readHall(){
  hallAr[0] = !digitalRead(HA);   //negiert weil pullup
  hallAr[1] = !digitalRead(HB);
  hallAr[2] = !digitalRead(HC);
}

uint8_t readHallState(){
  for(uint8_t i = 0; i < 6; i++){
    bool stateFound = true;
    for(uint8_t j = 0; j < 3; j++){
      if(commState[i][j] != hallAr[j]){
        stateFound = false;
      }
    }
    if(stateFound){
      return i;
    }
  }
  return 255;   //kein gültiger state gefunden
}

uint8_t commutation(uint8_t i_halState, bool i_dir){
  uint8_t o_state = 0;
  if(halState != 255){
    if(!i_dir){
      if(halState + 1 < 6){   //overflow
        o_state = halState + 1;
      }else{
        o_state = halState + 1 - 6;
      }
    }else{  
      if(halState - 2 >= 0){   //underflow
        o_state = halState - 2;
      }else{
        o_state = halState - 2 + 6;
      }
    }
  }
  return o_state;
}

float calcPhiEl(uint8_t i_halState, uint8_t i_lastHalState, float i_phiEl){
  float o_phiEl = i_phiEl;
  if(i_halState != i_lastHalState){   //state changed
    if(i_halState > i_lastHalState){  
      o_phiEl += (float)1/6;
    }else{
      o_phiEl -= (float)1/6;
    }
  }
  return o_phiEl;
}

void coilOut(uint8_t trgtState){
  analogWrite(A, (int)(commState[trgtState][0]) * thr);
  analogWrite(B, (int)(commState[trgtState][1]) * thr);
  analogWrite(C, (int)(commState[trgtState][2]) * thr);
}