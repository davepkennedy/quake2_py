//------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// "q_shosx.c" - shared functions for the plug-ins.
//
// Written by:	awe				            [mailto:awe@fruitz-of-dojo.de].
//		        �2001-2006 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
// Quake II� is copyrighted by id software	[http://www.idsoftware.com].
//
// Version History:
// v1.0.0:   Initial release.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma mark =Includes=

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <ctype.h>

#include "glob.h"
#include "qcommon.h"

#pragma mark -

//------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma mark =Variables=

byte *			membase;
int				maxhunksize;
int				curhunksize;
int				curtime;

static char		gSysFindBase[MAX_OSPATH];
static char		gSysFindPath[MAX_OSPATH];
static char		gSysFindPattern[MAX_OSPATH];
static	DIR	*	gSysFindDir;

#pragma mark -

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void strlwr (char *theString)
{
    if (theString != NULL)
    {
		while (*theString != 0x00)
		{
			*(theString) = tolower(*theString);
			theString++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void *	Hunk_Begin (int theMaxSize)
{
    maxhunksize	= theMaxSize + sizeof (int);
    curhunksize	= 0;
    membase		= malloc (maxhunksize);
	
    if (membase == NULL)
    {
        Sys_Error ("unable to virtual allocate %d bytes", theMaxSize);
    }
    
    *((int *) membase) = curhunksize;

    return (membase + sizeof(int));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void *	Hunk_Alloc (int theSize)
{
    unsigned char	*myMemory;

    theSize = (theSize + 31) & ~31;
	
    if (curhunksize + theSize > maxhunksize)
    {
        Sys_Error ("Hunk_Alloc overflow");
    }
    
	myMemory	= membase + sizeof(int) + curhunksize;
    curhunksize	+= theSize;
    
    return (myMemory);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void	Hunk_Free (void *theBuffer)
{
    unsigned char *	myMemory;

    if (theBuffer != NULL)
    {
        myMemory = ((unsigned char *) theBuffer) - sizeof (int);
        free (myMemory);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

int	Hunk_End (void)
{
    return (curhunksize);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

int	Sys_Milliseconds (void)
{
    struct timeval	myTimeValue;
    struct timezone	myTimeZone;
    static int		myStartSeconds;

    gettimeofday (&myTimeValue, &myTimeZone);
    
    if (!myStartSeconds)
    {
        myStartSeconds = myTimeValue.tv_sec;
        return (myTimeValue.tv_usec / 1000);
    }

    curtime = (myTimeValue.tv_sec - myStartSeconds) * 1000 + myTimeValue.tv_usec / 1000;
    
    return (curtime);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void	Sys_Mkdir (char *thePath)
{
    if (mkdir (thePath, 0777) == -1)
	{
		if (errno != EEXIST)
		{
			Sys_Error ("\"mkdir %s\" failed, reason: \"%s\".", thePath, strerror(errno));
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

static qboolean Sys_CompareAttributes (char *thePath, char *theName, unsigned theMustHave, unsigned theCantHave)
{
    struct stat		myStat;
    char			myFileName[MAX_OSPATH];

    // . and .. never match
    if (strcmp (theName, ".") == 0 || strcmp (theName, "..") == 0)
    {
        return false;
    }

    snprintf(myFileName, MAX_OSPATH, "%s/%s", thePath, theName);
    
    if (stat (myFileName, &myStat) == -1)
    {
        return (false);
    }
    
    if ((myStat.st_mode & S_IFDIR) && (theCantHave & SFF_SUBDIR))
    {
        return (false);
    }

    if ((theMustHave & SFF_SUBDIR) && !(myStat.st_mode & S_IFDIR))
    {
        return (false);
    }

    return (true);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

char	*Sys_FindFirst (char *thePath, unsigned theMustHave, unsigned theCantHave)
{
    struct dirent *	myDirEnt;
    char *			myPointer;

    if (gSysFindDir != NULL)
    {
        Sys_Error ("Sys_BeginFind without close");
    }

    strcpy (gSysFindBase, thePath);

    if ((myPointer = strrchr (gSysFindBase, '/')) != NULL)
    {
        *myPointer = 0;
        strcpy (gSysFindPattern, myPointer + 1);
    }
    else
    {
        strcpy (gSysFindPattern, "*");
    }

    if (strcmp (gSysFindPattern, "*.*") == 0)
    {
        strcpy (gSysFindPattern, "*");
    }
    
    if ((gSysFindDir = opendir (gSysFindBase)) == NULL)
    {
        return (NULL);
    }
    while ((myDirEnt = readdir (gSysFindDir)) != NULL)
    {
        if (!*gSysFindPattern || glob_match(gSysFindPattern, myDirEnt->d_name))
        {
            if (Sys_CompareAttributes (gSysFindBase, myDirEnt->d_name, theMustHave, theCantHave))
            {
                snprintf (gSysFindPath, MAX_OSPATH, "%s/%s", gSysFindBase, myDirEnt->d_name);
                return (gSysFindPath);
            }
        }
    }
    
    return (NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

char *	Sys_FindNext (unsigned theMustHave, unsigned theCantHave)
{
    struct dirent 	*myDirEnt;

    // just security:
    if (gSysFindDir == NULL)
    {
        return (NULL);
    }
    
    // find next...
    while ((myDirEnt = readdir (gSysFindDir)) != NULL)
    {
        if (!*gSysFindPattern || glob_match (gSysFindPattern, myDirEnt->d_name))
        {
            if (Sys_CompareAttributes (gSysFindBase, myDirEnt->d_name, theMustHave, theCantHave))
            {
                snprintf (gSysFindPath, MAX_OSPATH, "%s/%s", gSysFindBase, myDirEnt->d_name);
                return (gSysFindPath);
            }
        }
    }
    
    return (NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void	Sys_FindClose (void)
{
    if (gSysFindDir != NULL)
    {
        closedir (gSysFindDir);
    }
	
    gSysFindDir = NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
