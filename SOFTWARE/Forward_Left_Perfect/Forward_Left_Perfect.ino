#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===================== OLED =====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===================== MOTOR PINS =====================
#define MOTOR_R_IN1 27
#define MOTOR_R_IN2 14
#define MOTOR_R_PWM 13

#define MOTOR_L_IN1 25
#define MOTOR_L_IN2 26
#define MOTOR_L_PWM 12

// ===================== ENCODERS =====================
#define ENCODER_RA 34
#define ENCODER_RB 35
#define ENCODER_LA 33
#define ENCODER_LB 32

volatile long encoderCount1 = 0;
volatile long encoderCount2 = 0;

// ===================== ROBOT =====================
#define WHEEL_DIAMETER_CM 3.4
#define ENCODER_PPR 12
#define GEAR_RATIO 30

#define TICKS_PER_REV (ENCODER_PPR * GEAR_RATIO)
#define CM_PER_TICK ((PI * WHEEL_DIAMETER_CM) / TICKS_PER_REV)
#define TICKS_PER_CM (TICKS_PER_REV / (PI * WHEEL_DIAMETER_CM))

// ===================== ENCODER ISR =====================
void IRAM_ATTR encoderISR1() {
  int b = digitalRead(ENCODER_RB);
  encoderCount1 += (b == HIGH) ? 1 : -1;
}

void IRAM_ATTR encoderISR2() {
  int b = digitalRead(ENCODER_LB);
  encoderCount2 += (b == HIGH) ? 1 : -1;
}

// ===================== MOTOR FUNCTIONS =====================
void motorsForward(int pwm) {
  digitalWrite(MOTOR_R_IN1, HIGH);
  digitalWrite(MOTOR_R_IN2, LOW);

  digitalWrite(MOTOR_L_IN1, HIGH);
  digitalWrite(MOTOR_L_IN2, LOW);

  ledcWrite(MOTOR_R_PWM, pwm);
  ledcWrite(MOTOR_L_PWM, pwm * 0.9);
}

void motorsStop() {
  digitalWrite(MOTOR_R_IN1, LOW);
  digitalWrite(MOTOR_R_IN2, LOW);

  digitalWrite(MOTOR_L_IN1, LOW);
  digitalWrite(MOTOR_L_IN2, LOW);

  ledcWrite(MOTOR_R_PWM, 0);
  ledcWrite(MOTOR_L_PWM, 0);
}

void turnLeft(int pwm) {
  // Right wheel forward
  digitalWrite(MOTOR_R_IN1, HIGH);
  digitalWrite(MOTOR_R_IN2, LOW);

  // Left wheel backward
  digitalWrite(MOTOR_L_IN1, LOW);
  digitalWrite(MOTOR_L_IN2, HIGH);

  ledcWrite(MOTOR_R_PWM, pwm);
  ledcWrite(MOTOR_L_PWM, pwm);
}

// ===================== DRIVE DISTANCE =====================
void driveDistance(float distanceCM, int pwm) {

  long targetTicks = distanceCM * TICKS_PER_CM;

  noInterrupts();
  encoderCount1 = 0;
  encoderCount2 = 0;
  interrupts();

  motorsForward(pwm);

  while (true) {

    long rightTicks = encoderCount1;
    long leftTicks  = encoderCount2;

    long avgTicks = (abs(rightTicks) + abs(leftTicks)) / 2;

    Serial.print("R=");
    Serial.print(rightTicks);
    Serial.print(" L=");
    Serial.print(leftTicks);
    Serial.print(" Target=");
    Serial.println(targetTicks);

    if (avgTicks >= targetTicks) {
      break;
    }

    delay(5);
  }

  motorsStop();
}

// ===================== TURN LEFT 90° =====================
void turnLeft90(int pwm) {

  noInterrupts();
  encoderCount1 = 0;
  encoderCount2 = 0;
  interrupts();

  long targetTicks = 220;

  turnLeft(pwm);

  while (true) {

    long r = abs(encoderCount1);
    long l = abs(encoderCount2);

    long avgTicks = (r + l) / 2;

    Serial.print("TURN=");
    Serial.println(avgTicks);

    if (avgTicks >= targetTicks) {
      break;
    }

    delay(5);
  }

  motorsStop();
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.clearDisplay();
  display.display();

  // Motor pins
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);

  ledcAttach(MOTOR_R_PWM, 5000, 8);
  ledcAttach(MOTOR_L_PWM, 5000, 8);

  // Encoder pins
  pinMode(ENCODER_RA, INPUT_PULLUP);
  pinMode(ENCODER_RB, INPUT_PULLUP);
  pinMode(ENCODER_LA, INPUT_PULLUP);
  pinMode(ENCODER_LB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_RA), encoderISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_LA), encoderISR2, RISING);

  delay(1000);

  // Drive forward 30 cm
  driveDistance(30.0, 180);

  delay(500);

  // Turn left 90°
  turnLeft90(150);

  // Show message on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 28);
  display.println("HALA MADRID");
  display.display();

  Serial.println("Mission complete");
}

// ===================== LOOP =====================
void loop() {
}