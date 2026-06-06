#define ENCODER_RA 34
#define ENCODER_RB 35
#define ENCODER_LA 33
#define ENCODER_LB 32

volatile long encoderCount1 = 0;
volatile long encoderCount2 = 0;


// --- Interrupts ---
void IRAM_ATTR encoderISR1() {
  int b = digitalRead(ENCODER_RB);
  encoderCount1 += (b == HIGH) ? 1 : -1;
}

void IRAM_ATTR encoderISR2() {
  int c = digitalRead(ENCODER_LB);
  encoderCount2 += (c == HIGH) ? 1 : -1;
}

void setup() {
  Serial.begin(115200);

  // --- Motor setup ---
  // --- Encoder setup ---
  pinMode(ENCODER_RA, INPUT_PULLUP);
  pinMode(ENCODER_RB, INPUT_PULLUP);
  pinMode(ENCODER_LA, INPUT_PULLUP);
  pinMode(ENCODER_LB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_RA), encoderISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_LA), encoderISR2, RISING);

  encoderCount1 = 0;
  encoderCount2 = 0;
}

void loop() {
  // Read encoders safely
  long enc1 = encoderCount1;
  long enc2 = encoderCount2;
  Serial.print("Encoder_Right: "); Serial.print(enc1);
  Serial.print("  |Encoder_Left: "); Serial.println(enc2);

  delay(10);
}
