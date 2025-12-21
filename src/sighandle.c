#include "../include/sighandle.h"
#include "../include/common.h"
#include <readline/readline.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void handleSigInt(int sig)
{   
    if (gSignal == SIG_ATOMIC_MIN)
    {
        gSignal = sig;
        write(1, "^C", 2);
        return;
    }
    
    if (isatty(STDIN_FILENO))
    {
        gSignal = sig;

        char* temp = strjoin(rl_line_buffer, "^C");
        rl_replace_line(temp, 0);
        free(temp);
        rl_redisplay();
        write(1, "\n", 1);

        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
        
        return;
    }

    resetTerminal();
    rl_clear_history();
    exit(128 + sig);
}

void handleSigQuit(int sig)
{   
    if (isatty(STDIN_FILENO))
    {        
        rl_replace_line("", 0);
        rl_redisplay();
        return;
    }

    resetTerminal();
    rl_clear_history();
    exit(128 + sig);
}

void resetTerminal()
{
    static struct termios origTerm;
    static bool flag = false;

    if (!flag)
    {
        tcgetattr(ttyslot(), &origTerm);
        flag = true;
    }
    else
        tcsetattr(ttyslot(), TCSAFLUSH, &origTerm);
}

void disableCtrlPrint()
{
    struct termios newTerm;

    tcgetattr(ttyslot(), &newTerm);
    newTerm.c_lflag &= ~ECHOCTL;
    tcsetattr(ttyslot(), TCSANOW, &newTerm);
}