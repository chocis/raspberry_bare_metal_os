#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "types.h"

/* TODO
* For now these values are global. Should implement using struct.
* Maybe use MASKED buffer which gives much better permormance.
* It must use uffer size of power 2, and then can extra values be masked out by simple &.
* That allows to use many features without if statements. See details:
* http://stackoverflow.com/questions/9718116/improving-c-circular-buffer-efficiency
*/


//remember that max element count is size-1, so that indexes do not overlap and start from beginning
#define RING_BUFFER_SIZE 10

//STATIC makes these variables visible only in this file
static volatile char ringCurrentlyInBuffer[RING_BUFFER_SIZE]; //TODO - is this a good idea - to hold two buffers in memory?
static volatile char ringBuffer[RING_BUFFER_SIZE];
volatile u32 ringHeadIndex;
volatile u32 ringTailIndex;
volatile u32 ringSize;
volatile u32 ringCount;

u32 ringInit();
u32 ringAddCharToBuffer(char newChar);
u32 ringGetNextIndex(u32 index);
volatile char* ringGetCurrentBufferString();
void removeFirstBufferElements(u32 count);

#endif /* RINGBUFFER_H_ */
