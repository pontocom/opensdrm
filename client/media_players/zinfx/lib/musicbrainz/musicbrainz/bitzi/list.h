/* (PD) 2001 The Bitzi Corporation
 * Please see file COPYING or http://bitzi.com/publicdomain 
 * for more info.
 *
 * $Id: list.h,v 1.1 2004/10/13 16:16:09 linha2 Exp $
 */
/*------------------------------------------------------------------------- */

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _List
{
    int    numItems;
    int    numAllocated;
    void **list;
} List;

List *create_list(void);
int   num_items_in_list(List *list);
void  add_to_list(List *list, void *node);
void *return_item(List *list, int index);
void  delete_list(List *list);

#ifdef __cplusplus
}
#endif

#endif
