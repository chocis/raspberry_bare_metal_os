/*
 * ringBuffer.c
 *
 * Created: 2012.10.08. 20:59:35
 *  Author: chocis
 *
 *	Used for manupilating ring buffer array.
 */

#include "ringBuffer.h"


void removeFirstBufferElements(u32 count){
    u32 i;
	for(i=0; i< count; i++){
		ringTailIndex = ringGetNextIndex(ringTailIndex);
		ringCount--;
	}
}

u32 ringInit()
{
	ringHeadIndex = 0;
	ringTailIndex = 0;
	ringSize = 0;
	ringCount = 0;
	u32 success = 0;
	if (RING_BUFFER_SIZE >= 2)
	{
		ringSize = RING_BUFFER_SIZE;
		success = 1;
	}
	return success;
}


u32 ringGetNextIndex(u32 index){

	if(index == ringSize-1){ //if index is already at
		return 0; //next head index should start from 0
	}
	else{
		return index+1; //else return next probable index
	}
}

u32 ringAddCharToBuffer(char newChar){

	u32 success = 0;
	u32 nextProbableHeadIndex = ringGetNextIndex(ringHeadIndex);


	if(!(nextProbableHeadIndex == ringTailIndex)){ //add char only if head will not overflow tail

		ringBuffer[ringHeadIndex] = newChar;  //add new char to buffer
		ringHeadIndex = nextProbableHeadIndex; //now its safe to assign index
		ringCount++; //increase char count size inside buffer
		success = 1;
	}

	return success;
}

volatile char* ringGetCurrentBufferString(){


	if(ringHeadIndex == ringTailIndex){  //there are no elements in buffer
		ringCurrentlyInBuffer[0] = '\0';
	}
	else{
		u32 currentTailIndexTemp = ringTailIndex;
		u32 counter = 0;

		while (currentTailIndexTemp != ringHeadIndex)
		{
			ringCurrentlyInBuffer[counter] = ringBuffer[currentTailIndexTemp]; //take char from buffer
			counter++;
			currentTailIndexTemp = ringGetNextIndex(currentTailIndexTemp);
		}

		ringCurrentlyInBuffer[counter] = '\0';


	}
	return ringCurrentlyInBuffer;
}


