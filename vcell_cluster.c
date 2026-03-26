#include "vcell_engine.h"
#include <math.h>

// This function simulates the SPU Worker (The "Muscle")
void* spu_worker_thread(void* arg) {
    VirtualSPE* spu = (VirtualSPE*)arg;
    
    while(1) {
        if(spu->status == 1) { // If PPU gave us a task
            printf("[SPU %d] Received Task. Processing on Local Store...\n", spu->spu_id);
            
            // Simulating heavy PS3 Math (Matrix calculation / Physics)
            float sum = 0;
            for(int i = 0; i < 1024; i++) {
                sum += sqrtf(spu->workload_data[i] * i);
                usleep(100); // Simulate high-latency Cell memory access
            }
            
            spu->result = sum;
            spu->status = 2; // Mark as Done
            printf("[SPU %d] Task Complete. Result: %.2f\n", spu->spu_id, spu->result);
        }
        usleep(1000); 
    }
    return NULL;
}

int main() {
    PS3Node my_ps3;
    pthread_t threads[NUM_VIRTUAL_SPUS];

    printf("=== BOOTING VIRTUAL PS3 CLUSTER (vCell) ===\n");

    // 1. Initialize the Virtual Hardware
    for(int i = 0; i < NUM_VIRTUAL_SPUS; i++) {
        my_ps3.spus[i].spu_id = i;
        my_ps3.spus[i].status = 0;
        pthread_create(&threads[i], NULL, spu_worker_thread, &my_ps3.spus[i]);
    }

    // 2. The PPU (Main Thread) distributes a massive task
    printf("[PPU] Breaking down supercomputer workload...\n");
    for(int i = 0; i < NUM_VIRTUAL_SPUS; i++) {
        for(int j = 0; j < 1024; j++) {
            my_ps3.spus[i].workload_data[j] = (float)(rand() % 100);
        }
        my_ps3.spus[i].status = 1; // Wake up the SPE
    }

    // 3. PPU Waits for the Cluster to finish
    int completed = 0;
    while(completed < NUM_VIRTUAL_SPUS) {
        completed = 0;
        for(int i = 0; i < NUM_VIRTUAL_SPUS; i++) {
            if(my_ps3.spus[i].status == 2) completed++;
        }
    }

    // 4. Aggregate Results
    float final_calculation = 0;
    for(int i = 0; i < NUM_VIRTUAL_SPUS; i++) {
        final_calculation += my_ps3.spus[i].result;
    }

    printf("\n=== SUPERCOMPUTER OUTPUT ===\n");
    printf("Final Cluster Result: %.4f\n", final_calculation);
    printf("All Virtual SPEs returned to IDLE.\n");

    return 0;
}
