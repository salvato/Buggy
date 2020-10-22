/*
#include <stdio.h>

#include <pigpio.h>


//   Rotary encoder connections:

//   Encoder A      - gpio 18   (pin P1-12)
//   Encoder B      - gpio 7    (pin P1-26)
//   Encoder Common - Pi ground (pin P1-20)


#define ENCODER_A 18
#define ENCODER_B  7

static volatile int encoderPos;

// forward declaration

void encoderPulse(int gpio, int lev, uint32_t tick);

int main(int argc, char * argv[])
{
   int pos=0;

   if (gpioInitialise()<0) return 1;

   gpioSetMode(ENCODER_A, PI_INPUT);
   gpioSetMode(ENCODER_B, PI_INPUT);

   // pull up is needed as encoder common is grounded

   gpioSetPullUpDown(ENCODER_A, PI_PUD_UP);
   gpioSetPullUpDown(ENCODER_B, PI_PUD_UP);

   encoderPos = pos;

   // monitor encoder level changes

   gpioSetAlertFunc(ENCODER_A, encoderPulse);
   gpioSetAlertFunc(ENCODER_B, encoderPulse);

   while (1)
   {
      if (pos != encoderPos)
      {
         pos = encoderPos;
         printf("pos=%d\ ", pos);
      }
      gpioDelay(20000); // check pos 50 times per second
   }

   gpioTerminate();
}

void encoderPulse(int gpio, int level, uint32_t tick)
{
//

//             +---------+         +---------+      0
//             |         |         |         |
//   A         |         |         |         |
//             |         |         |         |
//   +---------+         +---------+         +----- 1

//       +---------+         +---------+            0
//       |         |         |         |
//   B   |         |         |         |
//       |         |         |         |
//   ----+         +---------+         +---------+  1

//

   static int levA=0, levB=0, lastGpio = -1;

   if (gpio == ENCODER_A) levA = level; else levB = level;

   if (gpio != lastGpio) // debounce
   {
      lastGpio = gpio;

      if ((gpio == ENCODER_A) && (level == 0))
      {
         if (!levB) ++encoderPos;
      }
      else if ((gpio == ENCODER_B) && (level == 1))
      {
         if (levA) --encoderPos;
      }
   }
}
*/


#include "rpmmeter.h"
#include <sys/time.h>
#include <QDebug>
#include <QTimer>


static callbackData* pUserData;


//typedef void (*CBFuncEx_t)
//   (int pi, unsigned user_gpio, unsigned level, uint32_t tick, void *userdata);
CBFuncEx_t
statusChanged(int      handle,
              unsigned user_gpio,
              unsigned level,
              uint32_t currentTick,
              void*    pData)
{
    // currentTick is the number of microseconds since boot.
    // WARNING: this wraps around from 4294967295 to 0 roughly every 72 minutes
    Q_UNUSED(handle)
    Q_UNUSED(level)
    Q_UNUSED(pData)

    pUserData->transitionCounter[user_gpio]++;
    pUserData->lastTick[user_gpio] = currentTick;
    pUserData->totalTick[user_gpio]++;
    return nullptr;
}


RPMmeter::RPMmeter(uint gpioPin, int gpioHandle, QObject *parent)
    : QObject(parent)
    , inputPin(gpioPin)
    , gpioHostHandle(gpioHandle)
{
    if(gpioHostHandle < 0) {
        perror("GPIO not Initialized");
        exit(EXIT_FAILURE);
    }
    int iStatus = set_mode(gpioHostHandle, inputPin, PI_INPUT);
    if(iStatus) {
        perror("Unable to set rpm meter as input");
        exit(EXIT_FAILURE);
    }
    iStatus = set_pull_up_down(gpioHostHandle, inputPin, PI_PUD_DOWN);
    if(iStatus) {
        perror("Unable to set rpm meter as input");
        exit(EXIT_FAILURE);
    }
    pUserData = new callbackData;
    pUserData->transitionCounter[inputPin] = 0;
    pUserData->tick0[inputPin] = 0;
    pUserData->lastTick[inputPin] = 0;

    iStatus = callback_ex(gpioHostHandle,
                          inputPin,
                          EITHER_EDGE,
                          reinterpret_cast<CBFuncEx_t>(statusChanged),
                          reinterpret_cast<void *>(pUserData));

    if(iStatus==pigif_duplicate_callback) {
        perror("Duplicate Callback");
        exit(EXIT_FAILURE);
    }
    if(iStatus==pigif_bad_malloc) {
        perror("Bad malloc");
        exit(EXIT_FAILURE);
    }
    if(iStatus==pigif_bad_callback) {
        perror("Bad Callback");
        exit(EXIT_FAILURE);
    }
    encoderTicksPerTurn = 38.0;
}


double
RPMmeter::currentSpeed() { // In giri/s
    dt = (pUserData->lastTick[inputPin]-pUserData->tick0[inputPin])*1.0e-6;
    if(dt == 0.0) return 0.0;
    speed = pUserData->transitionCounter[inputPin] /
            (encoderTicksPerTurn*dt);
    //qDebug() << inputPin << pUserData->transitionCounter[inputPin];
    pUserData->transitionCounter[inputPin] = 0;
    pUserData->tick0[inputPin] = pUserData->lastTick[inputPin];
    return speed;
}


void
RPMmeter::resetDistance() {
    pUserData->totalTick[inputPin] = 0;
}


double
RPMmeter::traveledDistance() {
    return double(pUserData->transitionCounter[inputPin]) / encoderTicksPerTurn;
}
