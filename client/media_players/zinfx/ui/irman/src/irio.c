/* irio.c v0.3 (c) 1998 Tom Wheeley <tomw@tsys.demon.co.uk>     */
/* this code is placed under the LGPL, see www.gnu.org for info */

/*
 * irio.c, Irman infrared controller interface
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "ir.h"

#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif

/* wrapper for ir_close_port() for use by atexit() */
static void ir_close_port_on_exit(void) { (void) ir_close_port(); }

static int portfd=0;
static int portflags=0;
static int oldflags=0;
static struct termios oldterm;
static struct termios portterm;

/*
 * Note regarding terminal settings.
 *
 * These work on my system.  I am quite confident they will work on other
 * systems.  The termios setup code is originally from another program
 * designed to talk to a serial device (casio diary) written by someone who I
 * can't remember but I presume they knew what they were doing.
 *
 * More information on Unix serial port programming can be obtained from
 *   http://www.easysw.com/~mike/serial/index.html
 *
 */

int ir_open_port(char *filename)
{
  int parnum = 0;
#if 0
  int hand = TIOCM_DTR | TIOCM_RTS;	/* I'm sure this *should* be used... */
#endif
  int baudrate=B9600;

  /* get a file descriptor */
  if ((portfd=open(filename, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
    return -1;
  }

  /* check to see that the file is a terminal */ 
  if (!isatty(portfd)) {
    return -1;
  }

  /* get port attributes, store in oldterm */
  if (tcgetattr(portfd, &oldterm) < 0) {
    return -1;
  }

  /* get port flags, save in oldflags */
  if ((oldflags = fcntl(portfd, F_GETFL)) < 0) {
    return -1;
  }

  /* now we have read the old attributes for the port, we can restore them
   * upon exit. if we had done this bfore, and exited beore reading in the
   * old attributes, we would have overwritten the old settings with zeros.  
   *
   * this way, if we do exit before we get here, we simply rely on the OS closing
   * the port for us, which is fine as we haven't changed anything yet.
   */

  atexit(ir_close_port_on_exit);

  /* copy old attrs into new structure */
  portterm = oldterm;
  portflags = oldflags;

  /* remove old parity setting, then set our own (ie none) */
  portterm.c_cflag &= ~(PARENB | PARODD);
  portterm.c_cflag |= parnum;

  /* set the char size to 8 bits, enable receiver, and disable modem control */
  portterm.c_cflag &= ~CSIZE;
  portterm.c_cflag |= CREAD | CS8 | CLOCAL;

  /* set the input and output baud rate */
  cfsetispeed(&portterm, baudrate);
  cfsetospeed(&portterm, baudrate);

  /* set non-canonical mode (we don't want any fancy terminal processing!) */
  portterm.c_lflag = 0;

  /* Ignore breaks and make terminal raw and dumb. */
  portterm.c_iflag = 0;
  portterm.c_iflag |= IGNBRK;
  portterm.c_oflag &= ~OPOST;

  /* now clean the serial line and activate the new settings */
  tcflush(portfd, TCIOFLUSH);
  if (tcsetattr(portfd, TCSANOW, &portterm) < 0) {
    return -1;
  }  

  /* set non-blocking */
  if (fcntl(portfd, F_SETFL, (portflags |= O_NONBLOCK)) < 0) {
    return -1;
  }

  /* wait a little while for everything to settle through */
  ir_usleep(IR_POWER_ON_LATENCY);
  
  return 0;
}


/* close the port, restoring old settings */

int ir_close_port(void)
{
  int retval = 0;
  
  if (!portfd) {	/* already closed */
    errno = EBADF;
    return -1;
  }
  
  /* restore old settings */
  if (tcsetattr(portfd, TCSADRAIN, &oldterm) < 0) {
    retval = -1;
  }

  if (fcntl(portfd, F_SETFL, oldflags) < 0) {
    retval = -1;
  }
  
  close(portfd);
  portfd=0;
  
  return retval;
}


/* write a character.  nothing interesting happens here */

int ir_write_char(unsigned char data)
{
  if (write (portfd, &data, 1) != 1)
    return -1;
  else {
#ifdef DEBUG_COMM
    printf("{%02x}", data);fflush(stdout);
#endif
    return 0; 
  }
}


/* read a character, with a timeout.
 * timeout < 0	-  block indefinitely
 * timeout = 0  -  return immediately
 * timeout > 0  -  timeout after `timeout' microseconds
 *                 use the nice macros in irio.h to define sec, msec, usec
 */ 

int ir_read_char(long timeout)
{
  unsigned char rdchar;
  int ok;
  fd_set rdfds;
  struct timeval tv;
  
  FD_ZERO(&rdfds);
  FD_SET(portfd, &rdfds);
  
  /* block until something to read or timeout occurs.  select() is damn cool */
  
  if (timeout < 0) {
    ok = select(portfd + 1, &rdfds, NULL, NULL, NULL);
  } else {
    tv.tv_sec=timeout / 1000000;
    tv.tv_usec=(timeout % 1000000);
    ok = select(portfd + 1, &rdfds, NULL, NULL, &tv);
  }
  
  if (ok > 0) {
    ok = read(portfd, &rdchar, 1);
    if (ok == 0) {
      return EOF;
    }
#ifdef COMM_DEBUG
    printf("[%02x]", rdchar);fflush(stdout);
#endif
    return rdchar;
  } else if (ok < 0) {
    return EOF-1;
  } else {
    errno = ETIMEDOUT;
    return EOF-1;
  }
   
  return 0;
} 


/* just about the only function where we don't care about errors! */

void ir_clear_buffer(void)
{
  while (ir_read_char(IR_GARBAGE_TIMEOUT) >= 0)
    ;
}


/* some systems have it, some systems don't.  This just makes life easier,
 * hence I have left this function visible (also for irfunc.c)
 */

void ir_usleep(unsigned long usec)
{
  struct timeval tv;

  tv.tv_sec=usec / 1000000;
  tv.tv_usec=(usec % 1000000);
  (void) select(0, NULL, NULL, NULL, &tv);
}


/* end of irio.c */
