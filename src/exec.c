#include "../include/exec.h"
#include "../include/common.h"
#include "../include/error.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static char* checkDirContent(char* dirPath, char* filePath)
{
    DIR* dir = opendir(dirPath);
    if (dir == NULL)
        return NULL;
    size_t fileLen = strlen(filePath);

    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (fileLen != strlen(entry->d_name))
                continue;

            if (!strcmp(entry->d_name, filePath))
            {
                char stackPathBuf[1024];
                // dirPath/entryName[\0]
                size_t needed = strlen(dirPath) + 1 + strlen(entry->d_name) + 1;
                char* fullPath = stackPathBuf;
                if (needed > sizeof(stackPathBuf))
                    fullPath = calloc(needed, sizeof(char));
                strcpy(fullPath, dirPath);
                strcat(fullPath, "/");
                strcat(fullPath, entry->d_name);

                struct stat statInfo;
                stat(fullPath, &statInfo);
                if (S_ISREG(statInfo.st_mode))
                {
                    closedir(dir);
                    if (fullPath == stackPathBuf)
                        return strdup(fullPath);
                    else
                        return fullPath;
                }
                else
                {
                    if (fullPath != stackPathBuf)
                        free(fullPath);
                }
            }
        }
    }

    closedir(dir);
    return (NULL);
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

void runExec(char* path, char** args, sConfig* conf)
{
    char* command = getExecFile(conf, path);
    // execve must receive a pathname that contains a /.
    // Examples:
    // 1. /bin/ls (absolute path). DONE.
    // 2. ./run (relative path). DONE.
    // 3. dir/run (relative to subdirectory). DONE.

    if (command == NULL)
    {
        setConfigExitCode(conf, NOT_FOUND);
        reportError("Command Error", "Command '%s' not found.", path);
        return;
    }
    else if (access(command, X_OK) == -1)
    {
        setConfigExitCode(conf, NOT_EXEC);
        reportError("Command Error",
            "Command '%s' cannot be executed (permission denied).", command);
        return;
    }

    size_t envpSize;
    char** tempEnvp = formExecEnv(conf->env, &envpSize);
    if (tempEnvp == NULL)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Internal Error", "Failed memory allocation.");
        return;
    }

    pid_t id = fork();
    if (id == 0)
    {
        int ret = execve(command, args, tempEnvp);
        if (ret == -1)
        {
            setConfigExitCode(conf, GEN_ERROR);
            reportError("Command Failure", "%s.", strerror(errno));
        }
        exit(0); // Temporary.
    }
    else
    {
        int status;
        waitpid(id, &status, 0);
        if (command != path) // To avoid double free-ing path later when freeing tokens.
            free(command);
        for (size_t i = 0; i < envpSize; i++)
            free(tempEnvp[i]);
        free(tempEnvp);
        conf->exitCode = WEXITSTATUS(status);
    }
}