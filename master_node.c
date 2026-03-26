#include <stdio.h>
#include <libspe2.h>
#include <pthread.h>

extern spe_program_handle_t spu_worker_handle;

#define NUM_SPUS 6 // Most PS3s have 6 available SPEs for Linux

void *run_spu(void *arg) {
    spe_context_ptr_t ctx = *(spe_context_ptr_t *)arg;
    unsigned int entry = SPE_DEFAULT_ENTRY;
    spe_stop_info_t stop_info;

    // Execute the worker code on the SPU
    spe_context_run(ctx, &entry, 0, NULL, NULL, &stop_info);
    pthread_exit(NULL);
}

int main() {
    spe_context_ptr_t contexts[NUM_SPUS];
    pthread_t threads[NUM_SPUS];

    printf("PS3 Supercomputer Node Initializing...\n");
    printf("Detecting %d Synergistic Processing Units...\n", NUM_SPUS);

    for(int i = 0; i < NUM_SPUS; i++) {
        // Create context for each SPU
        contexts[i] = spe_context_create(0, NULL);
        spe_program_load(contexts[i], &spu_worker_handle);
        
        // Start thread to manage the SPU
        pthread_create(&threads[i], NULL, run_spu, &contexts[i]);
        printf("SPU Worker %d started.\n", i);
    }

    for(int i = 0; i < NUM_SPUS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Cluster Task Complete.\n");
    return 0;
}
