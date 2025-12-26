#include "../include/exec.h"
#include "../include/common.h"
#include "../include/error.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// We can optimize executable searching (not just directory
// searching) further by using 'execve probing'. Essentially,
// you call execve with the path you currently have. If it
// succeeds, nothing more is to be done. If it fails, we check
// the errno value.
// - If it's ENOENT, we try the next directory (if searching the
// path )
// - If it's EACCES, the file isn't executable.
// - If it's EISDIR, the "file" is a directory.
// The kernel handles all the work for us and will do these
// checks much faster.
// Sticking with this approach for the moment since it's
// somewhat simpler (and it's fast enough).

static char* checkDirContent(char* dirPath, char* filePath)
{
    char stackPathBuf[1024];
    // dirPath/entryName[\0]
    size_t needed = strlen(dirPath) + 1 + strlen(filePath) + 1;
    char* fullPath = stackPathBuf;
    if (needed > sizeof(stackPathBuf))
        fullPath = calloc(needed, sizeof(char));
    strcpy(fullPath, dirPath);
    strcat(fullPath, "/");
    strcat(fullPath, filePath);

    if (access(fullPath, F_OK) == 0) // Check if file exists.
    {
        struct stat statInfo;
        stat(fullPath, &statInfo);
        if (S_ISREG(statInfo.st_mode)) // Check that it's a regular file.
        {
            if (fullPath == stackPathBuf)
                return strdup(fullPath);
            else
                return fullPath;
        }
    }

    if (fullPath != stackPathBuf)
        free(fullPath);
    return NULL;
}

static char* searchPath(sConfig* conf, char* path)
{
    char* envPath = expandEnv(conf, "PATH", NULL, NULL);
    char** path_dirs = split(envPath, ':');
    free(envPath);
    for (int i = 0; path_dirs[i] != NULL; i++)
    {
        char* ret = checkDirContent(path_dirs[i], path);
        if (ret != NULL)
        {
            for (int j = 0; path_dirs[j] != NULL; j++)
                free(path_dirs[j]);
            free(path_dirs);
            return ret;
        }
    }
    for (int i = 0; path_dirs[i] != NULL; i++)
        free(path_dirs[i]);
    free(path_dirs);
    return NULL;
}

static char* getExecFile(sConfig* conf, char* path)
{
    if (strchr(path, '/') != NULL)
    {
        if (access(path, F_OK) == 0)
            return path;
        else
            return NULL; // runExec will report an error properly.
    }
    else
        return searchPath(conf, path);
}

// Checks the executable (potentially) file and prepares
// our environment variable array.
// Returns the array on complete success.
// Otherwise returns NULL.
static char** checkExecFile(sConfig* conf, char* command, char* file)
{
    if (command == NULL)
    {
        setConfigExitCode(conf, NOT_FOUND);
        reportError("Command Error", "Command '%s' not found.", file);
        return NULL;
    }

    struct stat statInfo;
    stat(command, &statInfo);

    if ((access(command, X_OK) == -1) || (!S_ISREG(statInfo.st_mode)))
    {
        setConfigExitCode(conf, NOT_EXEC);
        reportError("Command Error",
            "Command '%s' cannot be executed (permission denied).", command);
        return NULL;
    }

    char** tempEnvp = formExecEnv(conf->env);
    if (tempEnvp == NULL)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Internal Error", "Failed memory allocation.");
        return NULL;
    }

    return tempEnvp;
}

void runExec(char* path, char** args, sConfig* conf)
{
    // Safety guard.
    if ((path == NULL) || path[0] == '\0')
        return;
    
    char* command = getExecFile(conf, path);
    char** tempEnvp = checkExecFile(conf, command, path);
    if (tempEnvp == NULL)
        return;

    // execve must receive a pathname that contains a /.
    // Examples:
    // 1. /bin/ls (absolute path). DONE.
    // 2. ./run (relative path). DONE.
    // 3. dir/run (relative to subdirectory). DONE.

    int ret = execve(command, args, tempEnvp);
    if (ret == -1)
    {
        if (errno == ENOEXEC) // File should be treated as a script, not an executable.
        {
            char* tempArgv[] = {"./minishell", command, NULL};
            int ret = execve("./minishell", tempArgv, tempEnvp);
            if (ret == -1)
                exit(NOT_EXEC);
            exit(conf->exitCode);
        }
        else
        {
            setConfigExitCode(conf, GEN_ERROR);
            reportError("Command Failure", "%s.", strerror(errno));
        }
    }
    if (command != path) // To avoid double free-ing path later when freeing tokens.
        free(command);
    for (size_t i = 0; tempEnvp[i] != NULL; i++)
        free(tempEnvp[i]);
    free(tempEnvp);
}