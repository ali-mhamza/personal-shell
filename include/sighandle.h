#ifndef SIGHANDLE_H
#define SIGHANDLE_H

#include <signal.h>

#ifndef SIGINT
#define SIGINT  2
#endif

#ifndef SIGQUIT
#define SIGQUIT 3
#endif

extern volatile sig_atomic_t gSignal;

void handleSigInt(int sig);
void handleSigQuit(int sig);

// To not print the ^\ upon getting SIGQUIT (i.e., Ctrl-\).

void resetTerminal();
void disableCtrlPrint();

#endif
