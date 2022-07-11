

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "heap.h"
#include "chunk.h"
#include <unistd.h>
#include <string.h>


#define MIN_UNITS 1024

#define ARRAY_SIZE 512

#define MIN_UNITS_IN_CHUNK 3



static chunkPtr memStart = NULL;

static chunkPtr memEnd = NULL;

static chunkPtr memArray[ARRAY_SIZE];



void insertInArray(chunkPtr ch){                    //insert the chunk of memory at the right place in the array of linked list
        chunkPtr arrPtr, prevChunk, q = NULL;
        

        size_t chSize = getChunkUnits(ch);
        size_t temp = chSize;

        if(chSize > ARRAY_SIZE - 1){
            arrPtr = memArray[ARRAY_SIZE - 1];
            temp = ARRAY_SIZE - 1;
        }
        else
            arrPtr = memArray[chSize];

        while(arrPtr != NULL && getChunkUnits(arrPtr) < chSize){
            
            q = arrPtr;
            arrPtr = getNextInList(ch);
        }

	
        setNextInList(ch, arrPtr);
        if(arrPtr){
            prevChunk = getPrevInList(arrPtr);
            setPrevInList(ch, prevChunk);
            if(!prevChunk)
                memArray[temp] = ch;

            setPrevInList(arrPtr,ch);
            if(q)
                setNextInList(q,ch);
        }
        else{
        	
            if(memArray[temp] == NULL){
            	
                memArray[temp] = ch;
                setPrevInList(ch,NULL);
            }

            else{
                setNextInList(q,ch);
                setPrevInList(ch,q);
            }
        }

        return;
}


/* This function is used to unite or coalesce the contiguous memory. It prevents fragmentation of free memory*/

chunkPtr coalesce(chunkPtr p, chunkPtr q){
	
	if(!p)
		return q;
	else if(!q)
		return p;

	size_t pSize = getChunkUnits(p);
	size_t qSize = getChunkUnits(q);

	size_t total = pSize + qSize;

	setChunkUnits(p ,total);
	return p;
}


int getMemIndex(size_t s){

	if(s >= ARRAY_SIZE)
		return ARRAY_SIZE-1;
	else
		return (int)(s);

}


/*When the memory is given to use for user , it is removed from the free list*/

void removeFromList(chunkPtr p){
	
	chunkPtr prevChunk = getPrevInList(p);
	chunkPtr nextChunk = getNextInList(p);
	
	if(prevChunk)
		setNextInList(prevChunk,nextChunk);
	else
		memArray[getMemIndex(getChunkUnits(p))] = nextChunk;

	if(nextChunk)
		setPrevInList(nextChunk,prevChunk);
		

	return;
			
}


void printHeap(){

	int i;
	chunkPtr ch;

	for(i = 0; i < ARRAY_SIZE; i++){
		ch = memArray[i];
		while(ch){
			printf("i = %d, units = %d\n", i, (int)getChunkUnits(ch));
			ch = getNextInList(ch);
		}
	}
	printf("\n");
	return;
}



/* This function is called to give a system call and request for more memory from the operating syatem*/

chunkPtr moreMemory(size_t reqUnits){   
					         //request for memory from operating syatem
    chunkPtr chunk;
    chunkPtr prevMem;
    chunkPtr newEnd;
    	
    if(reqUnits < MIN_UNITS)
        reqUnits = MIN_UNITS;

    newEnd = (chunkPtr)((char*)memEnd + reqUnits*chunkSize());
    if(newEnd < memEnd)
        return NULL;
    if(brk(newEnd) == -1)
        return NULL;

    chunk = memEnd;
    memEnd = newEnd;
    prevMem = getPrevMem(chunk,memStart);

    chunk->size = 0;
    
    setChunkUnits(chunk, reqUnits);
    chunkSetStatus(chunk, 0);

    if( prevMem && !(chunkStatus(prevMem))){
	    removeFromList(prevMem);
	    chunk = coalesce(prevMem,chunk);
    }

    insertInArray(chunk);

   
    assert(chunkStatus(chunk) == 0);


    return chunk;



}


/* This function is called when malloc finds a chunk which is greater than or equal to required size
modifies the chunk as required for the user and returns it*/

chunkPtr getChunk(chunkPtr p, size_t reqUnits){

	size_t pSize = getChunkUnits(p);

	size_t extra = pSize - reqUnits;
	
	chunkPtr extraChunk, tempChunk = p;
	removeFromList(p);

	if(pSize < reqUnits + MIN_UNITS_IN_CHUNK){

		chunkSetStatus(tempChunk,1);

		return tempChunk;
	}
	
	setChunkUnits(tempChunk, reqUnits);
	chunkSetStatus(tempChunk, 1);

	extraChunk = getNextMem(tempChunk, memEnd);
	setChunkUnits(extraChunk, extra);
	chunkSetStatus(extraChunk, 0);

	insertInArray(extraChunk);
       
       	return tempChunk; 	

}


/* malloc2 function : allocates required bytes to the user */

void* malloc2(size_t dsize){
	
	size_t uSize = chunkSize();
	size_t units ;
	
	int index;
	chunkPtr tempChunk;

	if(dsize == 0)
		return NULL;
	units = (dsize - 1)/uSize + 1;
	units += 2;

	//initialising the start and end of whole heap memory
	
	if(!memStart)
		memEnd = memStart = (chunkPtr)sbrk(0);
	
	
	for(index = getMemIndex(units); index < ARRAY_SIZE; index++){
		tempChunk = memArray[index];
		//printf(" %d ", index);

		while(tempChunk){
			if(getChunkUnits(tempChunk) >= index){
				
				tempChunk = getChunk(tempChunk, units);

				return (void *)(tempChunk + 1);
			}

			tempChunk = getNextInList(tempChunk);
		}
	}

	tempChunk = moreMemory(3);

	if(tempChunk == NULL){
		return NULL;

	}
	
	tempChunk = getChunk(tempChunk, units);
	//printf("\n%p no\n", memArray[15]);

	return (void *)(tempChunk + 1);

}


/* free2 function : frees the memory , changes status to free and attach the freed memory in linked list
			of free memory for further use*/

void free2(void *mem){
	
	if(!mem)
		return;

	chunkPtr tempChunk, prev, next;

	tempChunk = (chunkPtr)((char*)mem - chunkSize());

	chunkSetStatus(tempChunk, 0);

	prev = getPrevMem(tempChunk, memStart);

	if(prev && !(chunkStatus(prev))){
		removeFromList(prev);
		tempChunk = coalesce(prev, tempChunk);
	}
	
	next = getNextMem(tempChunk, memEnd);

	if(next && !(chunkStatus(next))){
		removeFromList(next);
		tempChunk = coalesce(tempChunk, next);
	}

	insertInArray(tempChunk);

	return;

}


/* Calloc2 function : call malloc and initialise each byte to 0*/
				

void* calloc2(size_t count, size_t siz){

	char* memory;
	size_t totalSiz = count*siz;
	
	memory = (char*)malloc2(totalSiz);
	
	if(memory){
		memory = memset(memory, 0, totalSiz);
	}

	return (void*)memory;
}
	


/*
Realloc2 function : Reallocates memory , spread or compress according to user needs
			If user wants to compress no changes in memory pointer , but if user wants to increase the memory size 
			changing the memory pointer and alloting new bytes , copying the prev data to newly alloted memory
			
			*/
			
			
void* realloc2(void *ptr, size_t reqSize){

	chunkPtr curr, new, temp;
	size_t currUnits, reqUnits, extraUnits;
	

	curr = (chunkPtr)ptr;
	curr = curr - 1;
	

	currUnits = getChunkUnits(curr);
	
	reqUnits = (reqSize - 1)/chunkSize() + 3;

	if(reqUnits < currUnits - 3){			//if user wants reallocated memory to be less than previous
		
		setChunkUnits(curr, reqUnits);
		
		temp = getNextMem(curr, memEnd);
		extraUnits = currUnits - reqUnits;

		setChunkUnits(temp, extraUnits);
		chunkSetStatus(temp, 1);

		free2(temp + 1);

		return curr;

	}

	if(reqUnits <= currUnits)
		return curr;

	new = (chunkPtr)malloc2(reqSize);         //calling malloc2 to allocate larger memory than previous


	if(new){
		new = (chunkPtr)strncpy((char*)new, (char*)curr, (currUnits-2) * chunkSize());

		free2(curr + 1);

	}
	

	return (void*)new;

}

		

