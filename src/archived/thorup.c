#define NULL_ITEM 0
/* null value for item locator*/
/* contains keyType and int declarations */
typedef long *it_type;
/* PUBLIC: type locates item via lookup */
typedef long *pq_ptr;
typedef unsigned long long keyType;
/* PUBLIC: type points to prioroty queue *


/* Included for profiling
#include <prof.h> */
/* Standard things */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
/* To test or not to test the user */
#define usercheck 0
/* To test or not to test ourselves */
#define safety 0
/* Often changed parameters */
#define bottommax 35
/*hardware assumptions*/
#define w 32
#define realmax (1.78e308)
typedef unsigned short int ourchar;
#define keymax 3
#define bucketmax 65535
/* some standard defs */
#define true 1
#define false 0
#define NULL 0
#define boolean int
#define ERROR(msg) {printf("%s","ERROR: " #msg); \
printf("\n"); exit(1);}
/* Insert a list into another list. The list to be
inserted starts at pointer fi and ends at pointer
la. The list is inserted before the node pointed
to by p. Can of course also be used to insert
just one node. Then fi=la. */
#define INS_BEFORE(fi, la, p) { la->nxt = p;\
fi->pre = p->pre; \
p->pre = la; fi->pre->nxt = fi; }
/* Remove a node from a list. */
#define OUT(p) p->nxt->pre = p->pre; \
p->pre->nxt = p->nxt;
#define START_BUCKET(bucket, thenew) { \
bucket->pre = thenew; \
bucket->nxt = thenew; \
thenew->nxt = bucket; \
thenew->pre = bucket;}
/* Macros for the bit map telling which
buckets are used */
#define CLR(i,j) map_unset_bit(used[i], j);
#define MRK(i,j) map_set_bit(used[i],j) ;
#define USED(i,j) map_bit(used[i],j)
/* comparisons */
#define GREATER(x,y) x.real>y.real
#define GEQ(x,y) x.real>=y.real
#define LESS(x,y) x.real<y.real
#define LEQ(x,y) x.real<=y.real
/* Type defs. The "external" type of pointer
(our list pointer) is called "usrpt" */
typedef union {
ourchar chars[keymax + 1];
double real;
} keytype;
typedef struct list {
keytype key;
struct list *pre, *nxt;
int data;
} list;
typedef list *usrpt;
typedef struct {
unsigned int signbit : 1;
unsigned int exponentbias1023 : 11;
/* unsigned int mantissa : 52; */
} doublemask;
typedef union {
double as_double;
doublemask as_mask;
} doubleandmask;
int expdouble(double d)
{return ((((doublemask*) &d)->exponentbias1023)-1023) ;}
/* Takes a 32 bit word and finds the leftmost bit */
long unsigned int word_left_bit(long unsigned int x)
{ doubleandmask fm;
fm.as_double=(double) x;
return (1054-fm.as_mask.exponentbias1023) ;
}

static long unsigned int bit[32] = {
0x1 <<31, 0x1 <<30, 0x1 <<29, 0x1 <<28, 0x1 <<27,
0x1 <<26, 0x1 <<25, 0x1 <<24, 0x1 <<23, 0x1 <<22,
0x1 <<21, 0x1 <<20, 0x1 <<19, 0x1 <<18, 0x1 <<17,
0x1 <<16, 0x1 <<15, 0x1 <<14, 0x1 <<13, 0x1 <<12,
0x1 <<11, 0x1 <<10, 0x1 << 9, 0x1 << 8, 0x1 << 7,
0x1 << 6, 0x1 << 5, 0x1 << 4, 0x1 << 3, 0x1 << 2,
0x1 << 1, 0x1 << 0 };
#define word_set_bit(x,i) x|=bit[i]
#define word_unset_bit(x,i) x&=~bit[i]
#define word_get_bit(x,i) ((x&bit[i])>0)
typedef struct {
long unsigned int level1[2];
long unsigned int level2[64];
long unsigned int level3[2048];
unsigned char level4[65537];
} Searchmap;
Searchmap *map_alloc()
{ Searchmap *m;
m=calloc(1,sizeof(Searchmap ));
if (!m) {printf("not enough memory"); exit(1);}
m->level4[65536]=1;
return m;
}
#define map_bit(m,i) (m->level4[i])
/* Find first value >=i (i<=65536) assume that
there are no smaller values */
unsigned int map_min(Searchmap *m, unsigned int i)
{
#if safety
if (i>65536) {
printf("%i is too large to finger from\n", i);
exit(1);}
#endif
if (m->level4[i]>0) return i;
i=i>>5;
if (m->level3[i]==0) {
i=i>>5;
if (m->level2[i]==0) {
i=i>>5;
while (m->level1[i]==0) {
i++;
if (i>1) return 65536;
}
i=word_left_bit(m->level1[i ])+ (i<< 5);
}
i=word_left_bit(m->level2 [i]) +(i <<5) ;
}
i=word_left_bit(m->level3[i ])+( i<< 5);
return i;
}
void map_set_bit(Searchmap *m, unsigned int i)
{ unsigned int j;
#if safety
if (i>=65536) {
printf("%i is to large to set\n", i);
exit(1);}
#endif
if (m->level4[i]>0) return;
m->level4[i]=1;
j=i&0x1F; i=i>>5;
if (m->level3[i] > 0) {
word_set_bit(m->level3[i ],j );
return;}
word_set_bit(m->level3 [i], j);
j=i&0x1F; i=i>>5;
if (m->level2[i] > 0) {
word_set_bit(m->level2[i ],j );
return;}
word_set_bit(m->level2 [i], j);
j=i&0x1F; i=i>>5;
word_set_bit(m->level1 [i], j);
}
void map_unset_bit(Searchmap *m, unsigned int i)
{ int j;
#if safety
if (i>=65536) {
printf("%i is to large to unset\n", i);
exit(1);}
#endif
if (m->level4[i]==0) return;
m->level4[i]=0;
j=i&0x1F; i=i>>5;
word_unset_bit(m->level3[i ],j );
if (m->level3[i]>0) return;
j=i&0x1F; i=i>>5;
word_unset_bit(m->level2[i ],j );
if (m->level2[i]>0) return;
j=i&0x1F; i=i>>5;
word_unset_bit(m->level1[i ],j );
}
/* This should be put in one struct for each
PQ instance */
Searchmap *used[keymax + 1];
list buckets[keymax + 1][bucketmax+1];
long PQsize, depth;
usrpt nextfree, bottom, sentinel;
keytype triekey, minkey;
long n,r;
boolean mindeleted;
/************ Initialization ************/
usrpt usrpt_alloc()
{ usrpt freept;
if (r) {
nextfree++;
r--;
return nextfree;
}
freept=(usrpt) bottom->pre;
if (freept!=sentinel) {

    bottom->pre=freept->pre;
freept->pre->nxt=bottom;
return freept;
}
printf("You are trying to allocate more than");
printf(" requested %ld elements.",n);
exit(1);
}
void Init()
{
long i;
mindeleted = true;
nextfree=(usrpt) malloc((n+10)*sizeof(list ));
if (!nextfree) {
printf("not enough memory for list elements");
exit(1);}
nextfree--;
r=n+8;
PQsize = 0;
depth = -1;
for (i = 0; i <= keymax; i++)
used[i]=map_alloc();
bottom = usrpt_alloc();
minkey.real = 0.0;
bottom->key = minkey;
sentinel = usrpt_alloc();
sentinel->key.real = realmax;
bottom->nxt = sentinel;
bottom->pre = sentinel;
sentinel->nxt = bottom;
sentinel->pre = bottom;
}
/************ Updating ************************/
/* Search along leftmost path. It is assumed that
this path is contained in minkey.
We compare up to and including endpos. */
long FirstDiff(x, startpos, endpos)
keytype x;
long startpos, endpos;
{ long i;
i = startpos;
while (x.chars[i] == minkey.chars[i] && i <= endpos)
i++;
return i;
}
/* Insert a new key, stored in a list node.
The list node thenew need not to be cleared,
the next and pre pointers can be anything.
newkey contains the key in thenew, passed
as paameter for optimization reasons. from
tells the depth where we should start
scanning the trie path. For a new key, from
is 0,when a key is reinserted during bucket
expansion or decrease-key, from may be
larger.
The variable locdepth is just a copy of the
global variable depth, used for optimization
reasons. */
void Insert(thenew,newkey,from)
usrpt thenew; keytype newkey; int short from;
{ usrpt tmp; long i,d;
ourchar newchar;
long locdepth;
locdepth = depth;
/* Search along leftmost trie path */
d=FirstDiff(newkey, from, locdepth);
/* If not too deep, insert in a bucket */
if (d <= locdepth) {
newchar=newkey.chars[d];
tmp = &buckets[d][newchar];
if (USED(d,newchar)) {
INS_BEFORE(thenew, thenew, tmp);
return;
}
START_BUCKET(tmp, thenew);
MRK(d,newchar);
/* Check if new key is smaller
than current triekey */
if (d == locdepth
&& newchar < triekey.chars[d])
triekey.chars[d]=newchar;
return;
}
/* If too deep, try to insert in bottom list */
i = 0;
tmp = bottom->nxt;
while (i < bottommax &&
GREATER(newkey, tmp->key)) {
tmp = tmp->nxt;
i++;
}
if (i < bottommax) {
INS_BEFORE (thenew, thenew, tmp);
return;
}
/* We end up here if insertion in bottom
list turned out to be too expensive.
That is, the "head" part of the bottom
list bcomes too large. In order to
shorten the head part, triedepth is
increased; triekey is updated */
if (locdepth >= 0)
triekey.chars[locdepth]
= newkey.chars[locdepth] ;
do {
locdepth++;
triekey.chars[locdepth]
= newkey.chars[locdepth] ;
} while (newkey.chars[locdepth ]
== minkey.chars[locdepth]);
depth = locdepth;
newchar=newkey.chars[locdepth] ;

tmp = &buckets[locdepth][newchar];
START_BUCKET(tmp, thenew);
MRK(locdepth,newchar);
}
void ExpandBucket()
{ usrpt expand, tmp,next; long i;
keytype newkey,bottomnextkey; long newchar;
int short from;
newchar = triekey.chars[depth];
tmp = &buckets[depth][newchar];
expand=tmp->nxt;
CLR(depth,newchar); /* Mark unused bucket */
tmp->pre->nxt=NULL;
/* Put as much as possible directly
into bottom list */
for (;;) {
newkey=expand->key;
i = 0;
tmp = bottom->nxt;
while (i < bottommax &&
GREATER(newkey, tmp->key)) {
tmp = tmp->nxt;
i++;
}
if (i >= bottommax) break;
next=expand->nxt;
INS_BEFORE (expand, expand, tmp);
expand=next;
if (!expand) {
bottom=bottom->nxt;
minkey=bottom->key;
do {
newchar=minkey.chars[depth] +1;
newchar=map_min(used[depth] ,newchar);
if (newchar <= bucketmax) {
triekey.chars[depth]=newchar;
break;
}
triekey.chars[depth]=0;
depth--;
} while (depth>=0);
return;
}
}
/* If something remain in bucket,
scan rest of it and
replace bottom->nxt whenever relevant. */
bottomnextkey=bottom->nxt ->key;
/* Since the first key in bucket was constly
to insert, it need not to beconsidered.
We start with the next one. */
tmp=expand->nxt;
while (tmp) {
if (GREATER(tmp->key, bottomnextkey)) {
tmp=tmp->nxt;
} else {
next=tmp->nxt;
if (next)
next->pre=tmp->pre;
tmp->pre->nxt=next;
INS_BEFORE(tmp,tmp,bottom->nxt);
bottomnextkey=tmp->key ;
tmp=next;
}
}
bottom=bottom->nxt;
minkey=bottom->key;
/* Now bottom list is correct.
The first element in expand list was
hard to insert, so it can't be minimum.
It will form a new bucket. */
next=expand->nxt;
depth++;
from=depth;
triekey.chars[depth]=newkey .chars[ depth];
while (newkey.chars[depth]
== minkey.chars[depth]) {
depth++;
triekey.chars[depth]=newkey.chars[depth ];
}
newchar=newkey.chars[depth] ;
tmp = &buckets[depth][newchar] ;
START_BUCKET(tmp, expand);
MRK (depth,newchar); /* Mark used bucket */
expand=next;
/* Now the remainder is inserted in
the standard style */
while (expand) {
next=expand->nxt;
Insert(expand,expand->key, from );
expand=next;
}
}
/* Remove the min key. If the first key in bottom
list becomes too large, call Expandbucket
The "real" Deletemin statement is:
bottom = bottom->nxt;
This is either done in ExpandBucket or here. */
void DeleteMin()
{
#if usercheck
if (PQsize<0)
ERROR(Trying to delete from empty queue);
#endif
if (depth >= 0 && LESS(triekey,bottom->nxt-> key) )
ExpandBucket();
else {
bottom = bottom->nxt;
minkey = bottom->key;
}
}
/* Called in decrease key */
void ClearBucket (p, k2)
usrpt p;

keytype k2;
{ usrpt tmp;keytype k1; long d;
ourchar c1,c2; long ch;
/*old key only element in some bucket */
k1=p->key;
p->key=k2;
d=FirstDiff(k1,0,depth);
c1=k1.chars[d];
c2=k2.chars[d];
if (c2==c1) return;
CLR(d,c1);
if (d==depth && c1==triekey.chars[d]) {
if (c2>minkey.chars[d]) {
tmp = &buckets[d][c2];
START_BUCKET(tmp,p);
MRK(d,c2);
triekey.chars[d]=c2;
return;
}
do {
ch=map_min(used[depth],
triekey.chars[depth]+1);
if (ch <= bucketmax) {
triekey.chars[depth]=ch;
break;
}
triekey.chars[depth]=0;
depth--;
} while (depth>=0);
}
Insert(p,k2,d);
}
/********* DIMACS interface ************/
pq_ptr construct_pq (int maxitems)
{
n = maxitems+1;
Init();
return NULL;
}
it_type find_min (pq_ptr Q)
{
if (mindeleted) {
DeleteMin();
mindeleted = false;
}
return (it_type)bottom;
}
keyType del_min (pq_ptr Q)
{
PQsize--;
if (!mindeleted)
mindeleted = true;
else
DeleteMin();
return bottom->key.real;
}
it_type insert (pq_ptr Q,
keyType pr, int in)
{ usrpt p; keytype k;
PQsize++;
k.real = pr;
#if usercheck
if (LESS(k,minkey))
ERROR(Insertin gtoo small key);
#endif
p = usrpt_alloc();
PR_ASSIGN (p->key, k);
IN_ASSIGN (p->data, in);
Insert(p,k,0);
return (it_type)p;
}
void decrease_p (pq_ptr Q,
it_type item, keyType pr)
{ keytype k; usrpt p;
p = (usrpt)item;
k.real = pr;
#if usercheck
if (LESS(k,minkey))
ERROR(Decrease key);
#endif
if (p->nxt!=p->pre) {
p->key.real = pr;
OUT(p);
Insert(p,k,0);
return;
}
ClearBucket ((usrpt)item, k);
}
keyType prioval (pq_ptr Q, it_type item)
{
return ((usrpt)item)->key.real ;
}
int *infoval (pq_ptr Q, it_type item)
{
return &((usrpt)item)->data;
}
int size (pq_ptr Q)
{
return PQsize ;
}
void clear (pq_ptr Q)
{
ERROR(clear);
}
pq_ptr destruct_pq (pq_ptr Q)
{
return NULL;
}