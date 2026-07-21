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
uint8_t thr = 255;
bool dir = 0;   //vorwärts
int actSpeed = 0;

bool commState[6][3] = {{1,0,0},
                      {1,1,0},
                      {0,1,0},
                      {0,1,1},
                      {0,0,1},
                      {1,0,1}};
bool hallAr[3] = {0,0,0};

void readHall();
uint8_t readHallState();
void coilOut(uint8_t state);

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
  //thr = analogRead(THR);
  dir = digitalRead(DIR);
  if(halState != 255){
    if(!dir){
      state = halState;
    }else{  
      if(halState - 2 >= 0){   //overflow
        state = halState - 2;
      }else{
        state = halState - 2 + 6;
      }
    }
  }

  coilOut(state);
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

void coilOut(uint8_t trgtState){
  analogWrite(A, (int)(commState[trgtState][0]) * thr);
  analogWrite(B, (int)(commState[trgtState][1]) * thr);
  analogWrite(C, (int)(commState[trgtState][2]) * thr);
}