#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"
#include "my402list.h"

int My402ListInit(My402List *list1)
{
 list1= malloc(sizeof(struct tagMy402List));
list1->num_members=0;
list1->anchor.prev=&(list1->anchor);
list1->anchor.next=&(list1->anchor);
list1->anchor.obj=NULL;
if (!list1)
return 0;
else return 1;
}

int  My402ListLength(My402List *list1)
{
return list1->num_members;
}

int  My402ListEmpty(My402List *list1)
{
if(list1->num_members==0)
return 1;
else return 0;
}

My402ListElem *My402ListFirst(My402List *list1)
{
if(list1->num_members==0) return NULL;
else
return list1->anchor.next;
}

My402ListElem *My402ListLast(My402List *list1)
{
if(list1->num_members==0) return NULL;
else
return list1->anchor.prev;
}

My402ListElem *My402ListNext(My402List *list1, My402ListElem *elem)
{
if(elem->next==&(list1->anchor))
return NULL;
else return elem->next;
}

My402ListElem *My402ListPrev(My402List *list1, My402ListElem *elem)
{
if(elem->prev==&(list1->anchor))
return NULL;
else return elem->prev;
}

int  My402ListAppend(My402List *list1, void *obj1)
{
My402ListElem *last;
My402ListElem *elem= malloc(sizeof(struct tagMy402List));

last=My402ListLast(list1);
if(last==NULL)
last=&(list1->anchor);

elem->obj=obj1;

elem->prev=(last);
elem->next=&(list1->anchor);
last->next=elem;
list1->anchor.prev=(elem);

list1->num_members=list1->num_members+1;
if(!elem)return 0;
else return 1;
}

int  My402ListPrepend(My402List *list1, void *obj1)
{
My402ListElem *first;
My402ListElem *elem= malloc(sizeof(struct tagMy402List));

first=My402ListFirst(list1);
if(first==NULL)
first=&(list1->anchor);

elem->obj=obj1;

elem->prev=&(list1->anchor);
elem->next=(first);
first->prev=elem;
list1->anchor.next=elem;
list1->num_members=list1->num_members+1;
if(!elem)return 0;
else return 1;
}

void My402ListUnlink(My402List *list1, My402ListElem *elem)
{
if( My402ListLength(list1) == 0)
return;
elem->prev->next= elem->next;
elem->next->prev= elem->prev;
list1->num_members=(list1->num_members)-1;
free(elem);
}


void My402ListUnlinkAll(My402List *list1)
{
My402ListElem *elem=NULL;My402ListElem *temp=NULL;
for (elem=My402ListFirst(list1); elem != NULL; ) {
temp=elem;
elem=My402ListNext(list1, elem);
free(temp);
}
list1->anchor.next = list1->anchor.prev = &(list1->anchor);
list1->num_members = 0;
}

int  My402ListInsertAfter(My402List *list1, void *obj, My402ListElem *elem)
{
My402ListElem *elem1= malloc(sizeof(struct tagMy402List));
//elem1->obj=obj;
if(elem==&(list1->anchor))
return My402ListPrepend(list1,obj);
else 
if(elem==My402ListLast(list1))
return My402ListAppend(list1,obj);
else
{
elem1->obj=obj;
elem1->prev=elem;
elem1->next=(elem->next);
elem->next->prev=elem1;
elem->next=elem1;}
list1->num_members=list1->num_members+1;
if(!(elem1)) return 0;
else return 1;
}

int  My402ListInsertBefore(My402List *list1, void *obj, My402ListElem *elem)
{
My402ListElem *elem1= malloc(sizeof(struct tagMy402List));
//elem1->obj=obj;
if(elem==&(list1->anchor))
return My402ListAppend(list1,obj);
else 
if(elem==My402ListFirst(list1))
return My402ListPrepend(list1,obj);
else
{
elem1->obj=obj;
elem1->prev=elem->prev;
elem1->next=elem;
elem->prev->next=elem1;
elem->prev=elem1;}
list1->num_members=list1->num_members+1;
if(!(elem1)) return 0;
else return 1;
}

My402ListElem *My402ListFind(My402List *list1, void *obj)
{
My402ListElem *elem=NULL;
for (elem=My402ListFirst(list1);elem != NULL;elem=My402ListNext(list1, elem)) {
if(elem->obj==obj)return elem;}
return NULL;
}
