/* irfunc.c v0.3 (c) 1998 Tom Wheeley <tomw@tsys.demon.co.uk>   */
/* this code is placed under the LGPL, see www.gnu.org for info */

/*
 * ircmd.c, Infra red command interface
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifndef __IR
#define __IR 1
#endif

#include "ir.h"

/* hashtable (array of pointers to ht_entry_t) */
static ir_htentry_t **ir_ht = NULL;

/* internal hash table functions - self explanatory */
static unsigned ir_hash_code(unsigned char *code, unsigned max);
static int ir_hash_add(unsigned char *code, int cmd);
static int ir_hash_match(unsigned char *code);
static int ir_hash_remove(unsigned char *code);
static void ir_hash_free(void);


/* checks the command is not one of the reserved ones, then adds it to the
 * hashtable associated with the given ir code */
 
int ir_register_command(unsigned char *code, int command)
{
  if (command <= 0) {
    errno = EINVAL;
    return -1;
  }
  return ir_hash_add(code, command);
}


/* removes command associated with code from the hashtable.
 * should we also enable removal indexed by command? - bit fiddly really,
 * but could actually be more useful.
 */

int ir_remove_command(unsigned char *code)
{
  return ir_hash_remove(code);
}


/* blocking wait for a code, then return a match if we find one, else
 * return IR_CMD_UNKNOWN for an unknown/spurious code, or IR_CMD_ERROR
 */

int ir_get_command(void)
{
  unsigned char *code;

  code = ir_get_code();
  if (code) {
    return ir_hash_match(code);
  } else {
    return IR_CMD_ERROR;
  }
}


/* polling wait for a command.  returns IR_CMD_UNKNOWN if nothing to 
 * be read from the port
 */

int ir_poll_command(void)
{
  unsigned char *code;
  
  code = ir_poll_code();
  if (code) {
    return ir_hash_match(code);
  } else {
    if (errno == ETIMEDOUT) {
      return IR_CMD_UNKNOWN;	/* the more harmless of errors */
    } else {
      return IR_CMD_ERROR;
    }
  }
}


void ir_free_commands(void)
{
  ir_hash_free();
}

/* all the internal hash table crap (separate chaining) */

static unsigned ir_hash_code(unsigned char *code, unsigned max)
{
 unsigned hashval;
 int i;
 
 hashval = 0;
 for (i = 0; i < IR_CODE_LEN; i++) {
   hashval = code[i] * 7 + hashval * 15;
 }
 return hashval % max;
}

static int ir_hash_add(unsigned char *code, int cmd)
{
  unsigned hashval;
  ir_htentry_t *node;
  
  if (!ir_ht) {
    ir_ht = calloc(IR_HT_SIZE, sizeof (ir_htentry_t *));
    if (!ir_ht) {
      errno = ENOMEM;
      return -1;
    }
  }
  
  hashval = ir_hash_code(code, IR_HT_SIZE);
  for (node=ir_ht[hashval]; node; node=node->next) {
    if (!memcmp(code, node->code, IR_CODE_LEN)) {
      errno = EBUSY;		/* code already in use */ 
      return - node->cmd;	/* so we know what has it */
    }
  }

  node = malloc(sizeof (ir_htentry_t));
  if (!node) {
    errno = ENOMEM;
    return -1;
  }
  
  memcpy(node->code, code, IR_CODE_LEN);
  node->cmd = cmd;
  node->next = ir_ht[hashval];
  ir_ht[hashval] = node;

  return 0;
}

static int ir_hash_match(unsigned char *code)
{
  ir_htentry_t *node;
  unsigned hashval;

  if (!ir_ht) {
    return IR_CMD_UNKNOWN;
  }
  
  hashval = ir_hash_code(code, IR_HT_SIZE);
  for (node = ir_ht[hashval]; node; node = node->next) {
    if (!memcmp(code, node->code, IR_CODE_LEN)) {
      return node->cmd;
    }
  }
    
  return IR_CMD_UNKNOWN;
}

static int ir_hash_remove(unsigned char *code)
{
  ir_htentry_t **pn;
  ir_htentry_t *dead;
  unsigned hashval;
  
  if (!ir_ht) {
    return -1;
  }
  
  hashval = ir_hash_code(code, IR_HT_SIZE);
  for (pn = &(ir_ht[hashval]); *pn; pn = &((*pn)->next)) {
    if (!memcmp(code, (*pn)->code, IR_CODE_LEN)) {
      dead = (*pn);
      (*pn) = dead->next;
      free(dead);
      return 0;
    }
  }
  return -1;
}

static void ir_hash_free(void)
{
  int i;
  ir_htentry_t *node;
  ir_htentry_t *dead;
  
  if (!ir_ht)
    return;
    
  for (i=0; i<IR_HT_SIZE; i++) {
    for (node = ir_ht[i]; node; ) {
      dead = node;
      node = dead->next;
      free(dead);
    }
  }
  
  free(ir_ht);
  ir_ht = NULL;
}


/* end of ircmd.c */
