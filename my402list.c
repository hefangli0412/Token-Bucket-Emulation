#include "cs402.h"
#include "my402list.h"
#include <stdlib.h>

int  My402ListLength(My402List* L)
{
	return L->num_members;
}
int  My402ListEmpty(My402List* L)
{
	if(L->num_members==0)
		return TRUE;
	else
		return FALSE;
}
int  My402ListAppend(My402List* L, void* r)
{//Add obj after Last(). This function returns TRUE if the operation 
	//is performed successfully and returns FALSE otherwise.
	My402ListElem *s,*t;	
	s=(My402ListElem*)malloc(sizeof(My402ListElem));
	if (s != NULL)
	{
	s->obj = r;  	
	t=L->anchor.prev;
	L->anchor.prev=s;
	t->next=s;
	s->next=&(L->anchor);
	s->prev=t;
	(L->num_members)++;
	return TRUE;}
	else 
		return FALSE;
}
int  My402ListPrepend(My402List* L, void* r)
{
	My402ListElem *s,*t;	
	s=(My402ListElem*)malloc(sizeof(My402ListElem));
	if(s != NULL)
	{s->obj = r;  
	t=My402ListFirst(L);
	L->anchor.next=s;
	t->prev=s;
	s->prev=&(L->anchor);
	s->next=t;
	(L->num_members)++;
	return TRUE;}
	else 
		return FALSE;
}
void My402ListUnlink(My402List* L, My402ListElem* E)
{//Unlink and delete elem from the list. Please do not delete the object 
	//pointed to by elem and do not check if elem is on the list.
	E->prev->next=E->next;
	E->next->prev=E->prev;
	free(E);
	(L->num_members)--;
}

//ÓÐ´í£¡£¡
void My402ListUnlinkAll(My402List* L)
{//Unlink and delete all elements from the list and make the list empty. 
	//Please do not delete the objects pointed to be the list elements.
	My402ListElem* p=My402ListFirst(L);
	My402ListElem* s; 
	p->prev->next=NULL;
	do
	{ 
		s=p->next;
		p->prev=NULL;
		p->next=NULL;
		s->prev=NULL;
		free(p);
		p=s;
	}while(p->next!=NULL);
	L->num_members=0;
}

int  My402ListInsertAfter(My402List* L, void* r, My402ListElem* E)
{//Insert obj between elem and elem->next. If elem is NULL, then this is the
	//same as Append(). This function returns TRUE if the operation is performed 
	//successfully and returns FALSE otherwise. Please do not check if elem is on the list.
	My402ListElem *s,*m,*n;	
	s=(My402ListElem*)malloc(sizeof(My402ListElem));
	if(s != NULL)
	{
		s->obj = r;  
		m=E->next,n=E->next->prev; 
		E->next->prev=s;
		E->next=s;
		s->next=m;
		s->prev=n;
		(L->num_members)++;
		return TRUE;
	}
	else 
		return FALSE;
}

int  My402ListInsertBefore(My402List* L, void* r, My402ListElem* E)
{//Insert obj between elem and elem->prev. If elem is NULL, then this is the same
	//as Prepend(). This function returns TRUE if the operation is performed successfully
	//and returns FALSE otherwise. Please do not check if elem is on the list.
	My402ListElem* s,*m,*n;
	s=(My402ListElem*)malloc(sizeof(My402ListElem));
	if(s != NULL)
	{
		s->obj = r;  
		n=E->prev;
		m=E->prev->next; 
		E->prev->next=s;
		//s->next=E;
		E->prev=s;
		s->next=m;
		s->prev=n;
		(L->num_members)++;
		return TRUE;
	}
	else 
		return FALSE;
}
My402ListElem *My402ListFirst(My402List* L)
{//Returns the first list element or NULL if the list is empty.
	if(L->num_members==0)
		return NULL;
	else
		return L->anchor.next;
}
My402ListElem *My402ListLast(My402List* L)
{//Returns the last list element or NULL if the list is empty.
	if(L->num_members==0)
		return NULL;
	else
		return L->anchor.prev;
}
My402ListElem *My402ListNext(My402List* L, My402ListElem* E)
{//Returns elem->next or NULL if elem is the last item on the list. 
 //Please do not check if elem is on the list.
	My402ListElem* p=E->next;
	if(p!=&(L->anchor)) return p;
	else return NULL;
}

My402ListElem *My402ListPrev(My402List* L, My402ListElem* E)
{
	My402ListElem* p=E->prev;
	if(p!=&(L->anchor)) return p;
	else return NULL;
}

My402ListElem *My402ListFind(My402List* L, void* r)
{//Returns the list element elem such that elem->obj == obj. 
	//Returns NULL if no such element can be found.
	My402ListElem* p=L->anchor.next; 
	while(p!=&(L->anchor))
	{
		if(p->obj==r) return p;
		p=p->next;
	}
	return NULL;
}

int My402ListInit(My402List* L)
{//Initialize the list into an empty list. Returns TRUE if all is well
	//and returns FALSE if there is an error initializing the list.
		L->anchor.next = &(L->anchor);
		L->anchor.prev = &(L->anchor);
		L->num_members = 0;
		return TRUE;
}
