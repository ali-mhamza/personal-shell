#include "../include/exec.h"
#include "../include/common.h"
#include "../include/error.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static char* checkDirContent(char* dirPath, char* filePath)
{
    DIR* dir = opendir(dirPath);
    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            char* temp = strjoin(dirPath, "/");
            char* fullPath = strjoin(temp, entry->d_name);
            free(temp);

            struct stat statInfo;
            stat(fullPath, &statInfo);
            if (S_ISREG(statInfo.st_mode))
            {
                if (strlen(filePath) != strlen(entry->d_name))
                {
                    free(fullPath);
                    continue;
                }
                if (!strcmp(entry->d_name, filePath))
                {
                    closedir(dir);
                    return fullPath;
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

static char* searchCurrentDir(sConfig* conf, char* path)
{
    if (strlen(conf->cwd) != 0)
        return checkDirContent(conf->cwd, path);
    return NULL;
}

static char* getExecFile(sConfig* conf, char* path)
{
    if (path == NULL)
        return NULL;
    if (path[0] == '/')
        return path;
    else if (strchr(path, '/') != NULL)
        return path;
    else if (strlen(path) > 2)
    {
        if (!strncmp(path, "./", 2) || !strncmp(path, "../", 3))
            return path;
    }

    char* file = searchCurrentDir(conf, path);
    if (file != NULL)
        return (file);
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
        reportError(conf, NOT_FOUND,
            "Command Error", "Command '%s' not found.", path);
        return;
    }
    else if (access(command, X_OK) == -1)
    {
        reportError(conf, NOT_EXEC,
            "Command Error", "Command '%s' cannot be executed.", command);
        return;
    }

    size_t envpSize;
    char** tempEnvp = formConfEnv(conf->env, &envpSize);
    if (tempEnvp == NULL)
    {
        reportError(conf, GEN_ERROR,
            "Internal Error", "Failed memory allocation.");
        return;
    }

    pid_t id = fork();
    if (id == 0)
    {
        int ret = execve(command, args, tempEnvp);
        if (ret == -1)
            reportError(conf, GEN_ERROR,
                "Command Failure", "%s.", strerror(errno));
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