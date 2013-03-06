/* irio.h v0.3 (c) 1998 Tom Wheeley <tomw@tsys.demon.co.uk>     */
/* this code is placed under the LGPL, see www.gnu.org for info */

#ifndef IR_H
#define IR_H

/* number of bytes sent back from the IR interface */

#define IR_CODE_LEN			6

/* timing details */

#if defined(solaris)
#undef SEC
#endif
#define SEC	* 1000000
#define MSEC	* 1000
#define USEC

/* Assuming LONG_MAX to be 2,000,000,000, we have a maximum timeout of
 * approx 2,000s, ie over half an hour.  Plenty!
 */

/* time we allow the port to sort itself out with */ 
#define IR_POWER_ON_LATENCY		(10 MSEC)
/* gap between sending 'I' and 'R' */
#define IR_HANDSHAKE_GAP		(500 USEC)

/* successive initial garbage characters should not be more than this apart */
#define IR_GARBAGE_TIMEOUT		(50 MSEC)
/* letters 'O' and 'K' should arrive within this */
#define IR_HANDSHAKE_TIMEOUT		(2 SEC)
/* successive bytes of an ir code should arrive within this time limit */
#define IR_POLL_TIMEOUT			(1 MSEC)

/* timeout for blocking IO */
#define IR_BLOCKING			(-1)

/* return from ir_get_command() on error */
#define IR_CMD_ERROR			-1
#define IR_CMD_UNKNOWN			0

/* size of hash table in ircmd.c.  must be prime */
#define IR_HT_SIZE		271

/* high level: ircmd.c */
int ir_register_command(unsigned char *code, int command);
int ir_remove_command(unsigned char *code);
int ir_get_command(void);
int ir_poll_command(void);
void ir_free_commands(void);

/* mid level: irfunc.c */
int ir_init(char *filename);
int ir_finish(void);
unsigned char *ir_get_code(void);
unsigned char *ir_poll_code(void);
char *ir_code_to_text(unsigned char *code);
unsigned char *ir_text_to_code(char *text);
void ir_set_enabled(int val);

/* low level: irio.c */
int ir_open_port(char *filename);
int ir_close_port(void);
int ir_write_char(unsigned char data);
int ir_read_char(long timeout);
void ir_clear_buffer(void);
void ir_usleep(unsigned long usec);

/* purely internal stuff */

#ifdef __IR

typedef struct ir_htentry_s {
  unsigned char code[IR_CODE_LEN];
  int cmd;
  struct ir_htentry_s *next;
} ir_htentry_t;

#endif /* __IR */


#endif /* IR_H */

/* end of ir.h */
