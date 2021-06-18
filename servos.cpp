#include <Servo.h>
#include "debug.h"
#include "servos.h"

/* identify Servo GPIOs */
const int AileG = 16; //CH1 Right Horiz can't use 2
const int ElevG = 13; //CH2 Right Vert ok
const int RuddG = 12; //CH3 Left Horiz ok
const int ThroG = 14; //CH4 Left Vert ok

/* FlySky type pulse width values */
/* Web page servo values are 0 to 200 */
const int MaxS = 2000;
const int CntS = 1500;
const int MinS = 1000;

uint8_t ch_pos[4] = { 100, 100, 100, 100 };
uint8_t last_pos[4] = { 100, 100, 100, 100 };

Servo AileS;  //ch1
Servo ElevS;  //ch2
Servo RuddS;  //ch3
Servo ThroS;  //ch4

Servo *ChS[4] = { &AileS, &ElevS, &RuddS, &ThroS };

#define DBGPRINT(a) dbgPrintf a;

void doServo(int val, Servo *serv, int which);

void servo_init(void)
{
    // setup Servos
  DBGPRINT(("Initialise Servos\n"));
  AileS.attach(AileG);
  ElevS.attach(ElevG);
  RuddS.attach(RuddG);
  ThroS.attach(ThroG);
  doServo(100, &AileS, 0);
  doServo(100, &ElevS, 1);
  doServo(100, &RuddS, 2);
  doServo(100, &ThroS, 3);
  DBGPRINT(("\n"));
}

void servo_process(void)
{
  int x = 0;

  for (x = 0; x < 4; x++)
  {
    if (ch_pos[x] != last_pos[x])
    {
      doServo(ch_pos[x], ChS[x], x);
      last_pos[x] = ch_pos[x];
    }
  }
  
}

void doServo(int val, Servo *serv, int which)
{
  int uS;
  int msk = 2^which;
  
  // val = 0 to 200 (100 = centre)
  //val += RcConfig.adj[which];
  /*
  if ((RcConfig.flags & (2^which)) != 0)
    val = 200-val;
  */
  if (val < 0)
    val = 0;
  else
  {
    if (val > 200)
      val = 200;
  }
  uS = (val * 5) + MinS;
  DBGPRINT((" :%d_%d_%d_%d\n",which,val,msk,uS));
  //DBGPRINT((which));
  //DBGPRINT(("_"));
  //DBGPRINT((RcConfig.adj[which]));
  //DBGPRINT(("_"));
  //DBGPRINT((val));
  //DBGPRINT(("_"));
  //DBGPRINT((msk));
  //DBGPRINT(("_"));
  //DBGPRINT((uS));
  serv->writeMicroseconds(uS);
}
