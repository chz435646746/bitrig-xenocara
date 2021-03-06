/*
 * *************************************************************************
 * This module is all original code
 * by Rob Nation
 * Copyright 1993, Robert Nation
 *     You may use this code for any purpose, as long as the original
 *     copyright remains in the source code and all documentation
 *
 * Changed 09/24/98 by Dan Espen:
 * - remove logic that processed and saved module configuration commands.
 * Its now in "modconf.c".
 * *************************************************************************
 */
#include "config.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"
#include "module.h"

extern Boolean debugging;

char *fvwm_file = NULL;

int numfilesread = 0;

static int last_read_failed=0;

static const char *read_system_rc_cmd="Read system"FVWMRC;


extern void StartupStuff(void);

/*
 * func to do actual read/piperead work
 * Arg 1 is file name to read.
 * Arg 2 (optional) "Quiet" to suppress message on missing file.
 */
static void ReadSubFunc(XEvent *eventp,Window junk,FvwmWindow *tmp_win,
                        unsigned long context, char *action,int* Module,
                        int piperead)
{
  char *filename= NULL,*Home, *home_file, *ofilename = NULL;
  char *option;                         /* optional arg to read */
  char *rest,*tline,line[1024];
  FILE *fd;
  int thisfileno;
  char missing_quiet;                   /* missing file msg control */
  char *cmdname;
  size_t len;

  /* domivogt (30-Dec-1998: I tried using conditional evaluation instead
   * of the cmdname variable ( piperead?"PipeRead":"Read" ), but gcc seems
   * to treat this expression as a pointer to a character pointer, not just
   * as a character pointer, but it doesn't complain either. Or perhaps
   * insure++ gets this wrong? */
  if (piperead)
    cmdname = "PipeRead";
  else
    cmdname = "Read";

  thisfileno = numfilesread;
  numfilesread++;

/*  fvwm_msg(INFO,cmdname,"action == '%s'",action); */

  rest = GetNextToken(action,&ofilename); /* read file name arg */
  if(ofilename == NULL)
  {
    fvwm_msg(ERR, cmdname,"missing parameter");
    last_read_failed = 1;
    return;
  }
  missing_quiet='n';                    /* init */
  rest = GetNextToken(rest,&option);    /* read optional arg */
  if (option != NULL) {                 /* if there is a second arg */
    if (strncasecmp(option,"Quiet",5)==0) { /* is the arg "quiet"? */
      missing_quiet='y';                /* no missing file message wanted */
    } /* end quiet arg */
    free(option);                       /* arg not needed after this */
  } /* end there is a second arg */

  filename = ofilename;
/*  fvwm_msg(INFO, cmdname,"trying '%s'",filename); */

  if (piperead)
    fd = popen(filename,"r");
  else if (ofilename[0] != '/')
  {
    /* find the home directory to look in */
    Home = getenv("HOME");
    if (Home != NULL)
    {
      len = strlen(Home) + strlen(ofilename) + 3;
      home_file = safemalloc(len);
      strlcpy(home_file,Home,len);
      strlcat(home_file,"/",len);
      strlcat(home_file,ofilename,len);
      filename = home_file;
      fd = fopen(filename,"r");
    }
    else
    {
      fd = 0;
    }
    if (fd == 0)
    {
      if((filename != NULL)&&(filename!= ofilename))
	free(filename);
      /* find the home directory to look in */
      Home = FVWM_CONFIGDIR;
      len = strlen(Home) + strlen(ofilename) + 3;
      home_file = safemalloc(len);
      strlcpy(home_file,Home,len);
      strlcat(home_file,"/",len);
      strlcat(home_file,ofilename,len);
      filename = home_file;
      fd = fopen(filename,"r");
    }
  }
  else
  {
    /* open file with absolute path */
    fd = fopen(filename,"r");
  }

  if(fd == NULL)
  {
    if (missing_quiet == 'n') {         /* if quiet option not on */
      if (piperead)
	fvwm_msg(ERR, cmdname, "command '%s' not run", ofilename);
      else
	fvwm_msg(ERR, cmdname,
		 "file '%s' not found in $HOME or "FVWM_CONFIGDIR, ofilename);
    } /* end quiet option not on */
    if((ofilename != filename)&&(filename != NULL))
    {
      free(filename);
    }
    if(ofilename != NULL)
    {
      free(ofilename);
    }
    last_read_failed = 1;
    return;
  }
  if((ofilename != NULL)&&(filename!= ofilename))
    free(ofilename);
  fcntl(fileno(fd), F_SETFD, 1);
  if (!piperead)
  {
    if(fvwm_file != NULL)
      free(fvwm_file);
    fvwm_file = filename;
  }
  else
  {
    if (filename)
      free(filename);
  }

  tline = fgets(line,(sizeof line)-1,fd);
  while(tline)
  {
    int l;
    while(tline && (l = strlen(line)) < sizeof(line) && l >= 2 &&
          line[l-2]=='\\' && line[l-1]=='\n')
    {
      tline = fgets(line+l-2,sizeof(line)-l+1,fd);
    }
    tline=line;
    while(isspace(*tline))
      tline++;
    if (debugging)
    {
      fvwm_msg(DBG,"ReadSubFunc","about to exec: '%s'",tline);
    }
    ExecuteFunction(tline,tmp_win,eventp,context,*Module);
    tline = fgets(line,(sizeof line)-1,fd);
  }

  if (piperead)
    pclose(fd);
  else
    fclose(fd);
  last_read_failed = 0;
}

void ReadFile(XEvent *eventp,Window junk,FvwmWindow *tmp_win,
              unsigned long context, char *action,int* Module)
{
  int this_read = numfilesread;

  if (debugging)
  {
    fvwm_msg(DBG,"ReadFile","about to attempt '%s'",action);
  }

  ReadSubFunc(eventp,junk,tmp_win,context,action,Module,0);

  if (last_read_failed && this_read == 0)
  {
    fvwm_msg(INFO,"Read","trying to read system rc file");
    ExecuteFunction((char *)read_system_rc_cmd,NULL,&Event,C_ROOT,-1);
  }

  if (this_read == 0)
  {
    if (debugging)
    {
      fvwm_msg(DBG,"ReadFile","about to call startup functions");
    }
    StartupStuff();
  }
}

void PipeRead(XEvent *eventp,Window junk,FvwmWindow *tmp_win,
              unsigned long context, char *action,int* Module)
{
  int this_read = numfilesread;

  if (debugging)
  {
    fvwm_msg(DBG,"PipeRead","about to attempt '%s'",action);
  }

  ReadSubFunc(eventp,junk,tmp_win,context,action,Module,1);

  if (last_read_failed && this_read == 0)
  {
    fvwm_msg(INFO,"PipeRead","trying to read system rc file");
    ExecuteFunction((char *)read_system_rc_cmd,NULL,&Event,C_ROOT,-1);
  }

  if (this_read == 0)
  {
    if (debugging)
    {
      fvwm_msg(DBG,"PipeRead","about to call startup functions");
    }
    StartupStuff();
  }
}

