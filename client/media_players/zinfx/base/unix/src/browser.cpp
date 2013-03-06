/* (PD) 2001 The Bitzi Corporation
 * Please see file COPYING or http://bitzi.com/publicdomain 
 * for more info.
 *
 * $Id: browser.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "browser.h"

void launch(const char *url, const char *command);
void launch_using_envvar(const char *url);
bool is_netscape_running(void);

void launch_browser(const char* url, BrowserEnum browser)
{
    char         command[1024];
    char        *browser_env;

    browser_env = getenv("BROWSER");
    if (browser_env && strlen(browser_env) > 0)
        return launch_using_envvar(url);

    switch(browser)
    {
        case eBrowserNetscape:
           if (is_netscape_running())
                strcpy(command, "netscape -raise -remote "
                                "\"openURL(%s)\""); 
           else
                strcpy(command, "netscape %s");

           break;
        case eBrowserMozilla:
           strcpy(command, "mozilla file://%s");
           break;
        case eBrowserKonqueror:
           strcpy(command, "konqueror file://%s");
           break;
        case eBrowserOpera:
           strcpy(command, "opera file://%s");
           break;
        case eBrowserLynx:
           strcpy(command, "lynx file://%s");
           break;
    }

    launch(url, command);

    printf("Cannot launch browser.\n");
    _exit(0);
}

void launch_using_envvar(const char *url)
{
    char *browser, *token;
    int   ret = 0;

    browser = strdup(getenv("BROWSER"));
    token = strtok(browser, ":");
    while(*token)
    {
        launch(url, token);
        token = strtok(NULL, ":");
    }
    free(browser);

    printf("Cannot launch browser.\n");
    _exit(0);
}

void launch(const char *url, const char *browser)
{
    char *command, token[1024], *args[255];
    int   ret, offset, used, argCount = 0;

    command = (char *)malloc(strlen(browser) + strlen(url) + 20);
    sprintf(command, browser, url);

    offset = 0;
    for(;;)
    {
       used = 0;
       ret = sscanf(command + offset, "\"%[^\"]\" %n", token, &used);
       if (ret > 0 || used > 0)
       {
           offset += used;
       }
       if (ret == 0)
       {
           used = 0;
           ret = sscanf(command + offset, "%[^ \t\n\r] %n", token, &used);
           if (ret > 0 || used > 0)
               offset += used;
       } 

       if (ret == 1)
       {
           args[argCount++] = strdup(token);
       }
       else
           break;
    }
    free(command);
    args[argCount] = NULL;
    execvp(args[0], args);
}

bool is_netscape_running(void)
{
    struct stat  sb;
    char        *home, lockfile[1024];

    home = getenv("HOME");
    if (!home) 
        return false;

    sprintf(lockfile,"%.200s/.netscape/lock",home);
    return (lstat(lockfile, &sb) != -1);
}
