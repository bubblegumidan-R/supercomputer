#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_VIRTUAL_SPUS 8          // The PS3 had 8 SPEs (7 available)
#define LOCAL_STORE_SIZE 262144    // 256KB "Local Store" per SPU

typedef struct {
    int spu_id;
    unsigned char local_store[LOCAL_STORE_SIZE]; // Sandboxed memory
    float workload_data[1024];                  // Task data
    float result;                               // Output
    int status;                                 // 0: Idle, 1: Processing, 2: Done
} VirtualSPE;

typedef struct {
    VirtualSPE spus[NUM_VIRTUAL_SPUS];
    int cluster_active;
} PS3Node;
