#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

static struct termios attrsav;

void sfcStopReal(void) {
  if(tcsetattr(0, TCSANOW, &attrsav) < 0) {
    fprintf(stderr, "failed to change term. attr.\n"); exit(-1); }
}

void sfcStopRealEnd(int sig) {
  sfcStopReal();
  if(sig < 0) exit(-1); else exit(0);
}

void sfcStartReal(void) {
  signal(SIGINT, &sfcStopRealEnd);
  if(tcgetattr(0, &attrsav) < 0) {
    fprintf(stderr, "failed to get term. attr.\n"); sfcStopReal(); exit(-1); }
  struct termios term_attr = attrsav;
  term_attr.c_lflag &= ~(ICANON|ECHO);
  term_attr.c_cc[VMIN] = 0;
  term_attr.c_cc[VTIME] = 0;
  if(tcsetattr(0, TCSANOW, &term_attr) < 0) {
    fprintf(stderr, "failed to change term. attr.\n");
    sfcStopReal(); exit(-1); }
//   if(fcntl(0, F_SETFL, O_NONBLOCK) == -1) {
//     fprintf(stderr, "failed to fcntl.\n"); sfcStopReal(); exit(-1); }
}

char sfcInKey(void) {
  char c;
  if (read(0, &c, 1) == 1) return c;
  else return (char)-1;
}
