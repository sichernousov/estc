
#ifndef QUEUE_H
#define QUEUE_H

typedef struct node_t
{
   packet_t* ppacket;
   struct node_t* next;
}  TNode;
 
typedef struct queue_t
{
   TNode* head;
   TNode* tail;
}  TQueue;

TQueue* QPush(TQueue* queue, packet_t* p);
packet_t* QPop(TQueue* queue);
void QClear(TQueue* queue);
 
#endif
