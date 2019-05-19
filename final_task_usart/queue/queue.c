#include <queue.h>

TQueue* QPush(TQueue* queue, packet_t* p)
{
   TNode* node = malloc(sizeof(TNode));
   node->ppacket = p;
   node->next = NULL;
 
   if (queue->head && queue->tail)
   {
      queue->tail->next = node;
      queue->tail = node;
   }
   else
   {
      queue->head = queue->tail = node;
   }
 
   return queue;
}

packet_t* QPop(TQueue* queue)
{
   packet_t* p = NULL;
 
   if (queue->head)
   {
      p = queue->head->ppacket;
      queue->head = queue->head->next;
   }
 
   return p;
}

void QClear(TQueue* queue)
{
    while (queue->head)
    {
        QPop(queue);
    }
 
    queue->head = queue->tail = NULL;
}
