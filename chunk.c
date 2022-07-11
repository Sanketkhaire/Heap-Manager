
#include "chunk.h"
#include <stddef.h>

size_t chunkSize(){

    return sizeof(chunk);

}

int chunkStatus(chunkPtr c){

    return c->size & 1U;

}

void chunkSetStatus(chunkPtr ch,int stat){

    ch->size &= ~1U;
    ch->size |= stat;
    return;
}

size_t getChunkUnits(chunkPtr ch){

    return (ch->size)>>1;
}

void setChunkUnits(chunkPtr ch,size_t units){

    ch->size &= 1U;
    ch->size |= units<<1U;

    (ch+units-1)->size = units;
    return;
}

chunkPtr getNextInList(chunkPtr ch){

    return ch->neighbr;

}

chunkPtr getPrevInList(chunkPtr ch){

    return (ch + getChunkUnits(ch) - 1)->neighbr;

}

void setNextInList(chunkPtr ch, chunkPtr next){

    ch->neighbr = next;
    return ;
}

void setPrevInList(chunkPtr ch, chunkPtr prev){

    (ch + getChunkUnits(ch) - 1)->neighbr = prev;
    return;
}

chunkPtr getPrevMem(chunkPtr ch, chunkPtr start){

    chunkPtr prevTemp;

    size_t temp = chunkSize(ch);

    if(ch == start)
        return NULL;

    prevTemp = (chunkPtr)((char*)ch - temp);

    prevTemp = ch - prevTemp->size;
    return prevTemp;

}

chunkPtr getNextMem(chunkPtr ch, chunkPtr End){
	
	chunkPtr nextTemp;

	size_t temp = getChunkUnits(ch);
	
	nextTemp = ch + temp;
	if(nextTemp == End)
		return NULL;

	return nextTemp;
}
	

