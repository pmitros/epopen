/* EpOpen.c -- Extended popen command 

Returns streams for read AND write instead of just one or the other.

Tested under Debian 1.3.1r6, libc5.4.33, Linux 2.0.33

    Copyright (C) 1998, 2000 Piotr F. Mitros

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

/* Extended popen -- open a command for read and write */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int epopen(FILE**fin, FILE**fout, const char *command)
{
  int inpipe[2], outpipe[2];
  int pid;

  if(pipe(inpipe)) {
    perror ("Error creating inpipe");
    return -1;
  }

  if(pipe(outpipe)) {
    perror ("Error creating outpipe");
    return -1;
  }

  /* Exit, since a failed fork can leave the system in a fscked up
     state */
  if((pid = fork()) < 0)
    {
      perror("Error forking");
      exit(1);
    }
  
  /* The child process executes the command */
  if(pid==0)
    {
      if (dup2(outpipe[1], 1)==-1)
      	perror("Error duplicating outpipes");
      if (dup2(inpipe[0], 0)==-1)
      	perror("Error duplicating inpipes");
      close(outpipe[0]);
      close(inpipe[1]);
      execl("/bin/sh", "sh", "-c", command, 0);     
      perror("Error during exec");
      _exit(127);
    }
  
  close(inpipe[0]);
  close(outpipe[1]);
  *fin=fdopen(inpipe[1], "w");
  *fout=fdopen(outpipe[0], "r");
  
  return 1;
}

void main()
{
  FILE *fin, *fout;
  int i;
  char ch;
  
  epopen(&fout, &fin, "grep 5");
  for(i=0; i<20; i++)
    fprintf(fout, "%d\n", i);
  fputc(EOF, fout);
  fclose(fout);
  while (!feof(fin))
    {
      printf ("%c",fgetc(fin));
    }
}


