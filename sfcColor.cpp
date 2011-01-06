#include <iostream>
#include <unistd.h>
using namespace std;

int xsize, ysize, tracemode;

void sfcClearS(void) { cout << "\033[2J" << flush; }  /* clear screen */

void sfcClearL(void) { cout << "\033[0K" << flush; }   /* clear line */

void sfcMoveTo(int x, int y) { 
  cout << "\033[" << y+1 << ";" << x*2+1 << "H" << flush;
}

void sfcHome(void) {  sfcMoveTo(0, ysize+1); }

void sfcTrace(int mode) {
  tracemode = mode;
}

void sfcTraceStep(char *message) {
  char dummyinput[1];
  sfcMoveTo(0, ysize);
  cout << message << flush;
  if (tracemode == 1) {
    sleep(1);
  } else {
    cout << " [Press Return]" << flush;
    cin.getline(dummyinput, 1);
  }
  sfcMoveTo(0, ysize);
  sfcClearL();
}

void sfcPoint(int x, int y, int c) {
  char * dot = (char *)"¢£";
  if ((x < xsize) && (y < ysize) && (c < 8) &&
      (x >= 0)    && (y >= 0)    && (c >= 0)) {
    switch (c) {
    case 0: cout << "\033[0;30;47m"; break;             /* black   */
    case 1: cout << "\033[0;34;47m"; break;             /* blue    */
    case 2: cout << "\033[0;32;47m"; break;             /* green   */
    case 3: cout << "\033[0;36;47m"; break;             /* cyan    */
    case 4: cout << "\033[0;31;47m"; break;             /* red     */
    case 5: cout << "\033[0;35;47m"; break;             /* magenta */
    case 6: cout << "\033[1;33;47m"; break;             /* yellow  */
    case 7: cout << "\033[0;30;47m"; dot = (char *)"¢¢"; break; /* white   */
    }
    sfcMoveTo(x, y); cout << dot;
    cout << "\033[0m" << flush;
  }
  if (tracemode > 0) {
    char *buf;
    asprintf(&buf, "sfcPoint(%d, %d, %d)", x, y, c);
    sfcTraceStep(buf);
    free(buf);
  }
  sfcMoveTo(0, ysize+1);
}

void sfcString(int x, int y, int c, char *str) {
  if ((x < xsize) && (y < ysize) && (c < 8) &&
      (x >= 0)    && (y >= 0)    && (c >= 0)) {
    switch (c) {
    case 0: cout << "\033[0;30;47m"; break;             /* black   */
    case 1: cout << "\033[0;34;47m"; break;             /* blue    */
    case 2: cout << "\033[0;32;47m"; break;             /* green   */
    case 3: cout << "\033[0;36;47m"; break;             /* cyan    */
    case 4: cout << "\033[0;31;47m"; break;             /* red     */
    case 5: cout << "\033[0;35;47m"; break;             /* magenta */
    case 6: cout << "\033[1;33;47m"; break;             /* yellow  */
    case 7: cout << "\033[0;37;40m"; break;             /* white   */
    }
    sfcMoveTo(x, y); cout << str;
    cout << "\033[0m" << flush;
  }
  if (tracemode > 0) {
    char *buf;
    asprintf(&buf, "sfcString(%d, %d, %d, \"%s\")", x, y, c, str);
    sfcTraceStep(buf);
    free(buf);
  }
  sfcMoveTo(0, ysize+1);
}

void sfcInit(int x = 10, int y = 10) {
  xsize = x; ysize = y;
  sfcClearS();
  for   (x=0; x<xsize; x++) {
    for (y=0; y<ysize; y++) {
      sfcPoint(x,y,7);
    }
  }
  if (tracemode > 0) {
    sfcTraceStep((char *)"Screen Initialized");
  }
  sfcMoveTo(0, ysize+1);
}
