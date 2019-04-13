#include <Arduino_FreeRTOS.h>
#include <Servo.h>

//hcsr04 pins/constants
#define PIN_HCSR04_TRIG 26
#define PIN_HCSR04_ECHO 22

#define RANGE_HCSR04_MAX 30
#define RANGE_HCSR04_MIN 3
#define RANGE_HCSR04_NO_ECHO -1

//servo pins/constants
#define PIN_SERVO 52
#define SERVO_MAX 90
#define SERVO_MIN 0

Servo SERVO;

bool isTargetSpotted;

int lDuration;
int lDistance;

int rotate;
int rotateInc;


void vTaskHCSR04(void *pvParameters);
void vTaskServo(void *pvParamethers);
void vTaskSerial(void *pvParamethers);

void setup() {
  
  Serial.begin(9600);
  
  pinMode(PIN_HCSR04_ECHO, INPUT);
  pinMode(PIN_HCSR04_TRIG, OUTPUT);

  SERVO.attach(PIN_SERVO);

  isTargetSpotted = false;

  //Create task for HCSR04
  //Priority: 1
  xTaskCreate(
    vTaskHCSR04,
    (const portCHAR *) "Task - HCSR04",
    300,
    NULL,
    1,
    NULL
  );
  //Create task for Servo
  //Priority: 2
  xTaskCreate(
    vTaskServo,
    (const portCHAR *) "Task - SERVO",
    300,
    NULL,
    2,
    NULL
  );
  
  //Create task for Serial
  //Priority: 3
  xTaskCreate(
    vTaskSerial,
    (const portCHAR *) "Task - SERIAL",
    100,
    NULL,
    3,
    NULL
  );
  
  vTaskStartScheduler();

}

void loop() {}

void vTaskHCSR04(void *pvParameters){
  (void) pvParameters;
  const char *pcTaskName = "Task: ";

  lDuration = 0;
  lDistance = 0;

  for(;;){


    Serial.println(lDistance);
    
    digitalWrite(PIN_HCSR04_TRIG, LOW);
    vTaskDelay(2 / portTICK_PERIOD_MS);
  
    digitalWrite(PIN_HCSR04_TRIG, HIGH);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    digitalWrite(PIN_HCSR04_TRIG, LOW);

    lDuration = pulseIn(PIN_HCSR04_ECHO, HIGH);
    lDistance = lDuration / 29.1 / 2;

    if(lDistance > RANGE_HCSR04_MAX){
      lDistance = RANGE_HCSR04_NO_ECHO;
      isTargetSpotted = false;
    } else {
      lDistance = constrain(lDistance, RANGE_HCSR04_MIN, RANGE_HCSR04_MAX);
      isTargetSpotted = (lDistance < RANGE_HCSR04_MAX && lDistance > RANGE_HCSR04_MIN);
     }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void vTaskServo(void *pvParameters){
  (void) pvParameters;
  const char *pcTaskName = "Task: ";

  rotate = 1;
  rotateInc = 1;

  for(;;){
           
      if (!isTargetSpotted) {
        
        SERVO.write(rotate);        

        if (rotate == SERVO_MAX){
           rotateInc = -rotateInc;
        }
        if (rotate == SERVO_MIN){
          rotateInc = -rotateInc;
        }
        rotate += rotateInc;
        vTaskDelay(20 / portTICK_PERIOD_MS);
      }
      else {
        rotate += 5;
        SERVO.write(rotate);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        rotate -= 5;
        SERVO.write(rotate);
        vTaskDelay(20 / portTICK_PERIOD_MS);
      }
      
    }
}

void vTaskSerial(void *pvParameters){
  (void) pvParameters;
  const char *pcTaskName = "Task: ";

  for(;;){

    Serial.print(rotate);
    Serial.print(":");
    Serial.println(lDistance);
    vTaskDelay(20 / portTICK_PERIOD_MS);

  }
}
