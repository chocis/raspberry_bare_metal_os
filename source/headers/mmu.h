#ifndef MMU_H
#define MMU_H


/* PERFORMANCE Bemchmark
*   MMU takes a little extra commands to convert virtual to physycal addresses,
*   thats why I created performance benchmark and got these results:
*
*   Without MMU:
*   Start time: 7028320,  End time: 9671536,  Time spent: 2643216
*   Start time: 11030612,  End time: 13672790,  Time spent: 2642178
*
*   With MMU:
*   Start time: 7028925,  End time: 9684855,  Time spent: 2655930
*   Start time: 7028876,  End time: 9684745,  Time spent: 2655869
*
*   Average performance decrease: 0.5%. Looks like around 13ms lost in 2655ms

    USED THIS CODE FOR BENCHMARKING:

    mmu_init();
    volatile u32 start_time = getTimerCurrentValue();

    volatile u32 testVariable;
    u32 i;

    for(i=0; i< 10000000; i++){
        testVariable = POINTVAL_(i);
        dummy();
    }
    volatile u32 end_time = getTimerCurrentValue();

    DEBUG("Start time: %d,  End time: %d,  Time spent: %d ", start_time, end_time, end_time-start_time);

*/



void mmu_init(void);

#endif /* MMU_H */
