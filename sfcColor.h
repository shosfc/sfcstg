#ifndef SFCCOLOR
#define SFCCOLOR
void sfcClearS(void);
void sfcClearL(void);
void sfcMoveTo(int, int);
void sfcHome(void);
void sfcTrace(int);
void sfcTraceStep(char*);
void sfcPoint(int, int, int);
void sfcString(int, int, int, char*);
void sfcInit(int, int);
#endif
