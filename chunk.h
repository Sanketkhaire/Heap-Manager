#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>
#include <unistd.h>

typedef struct chunk{
    size_t size;
    struct chunk* neighbr;
}chunk;

typedef chunk* chunkPtr;

size_t chunkSize();
int chunkStatus(chunkPtr c);
void chunkSetStatus(chunkPtr ch,int stat);
size_t getChunkUnits(chunkPtr ch);
void setChunkUnits(chunkPtr ch,size_t units);
chunkPtr getNextInList(chunkPtr ch);
chunkPtr getPrevInList(chunkPtr ch);
void setNextInList(chunkPtr ch, chunkPtr next);
void setPrevInList(chunkPtr ch, chunkPtr prev);
chunkPtr getPrevMem(chunkPtr ch, chunkPtr start);
chunkPtr getNextMem(chunkPtr ch, chunkPtr End);

#endif // CHUNK_H




