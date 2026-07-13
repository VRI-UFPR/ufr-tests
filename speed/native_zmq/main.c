// ============================================================================
//  Header
// ============================================================================

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <msgpack.h>
#include <pthread.h>

// ============================================================================
//  Medição do Tempo
// ============================================================================

int count = 0;
int64_t sum = 0;
struct timespec start, end;

static
void time_begin() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

static
void time_end() {
    clock_gettime(CLOCK_MONOTONIC, &end);
    int64_t elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + 
            (end.tv_nsec - start.tv_nsec);
    sum += elapsed_ns;
    count += 1;
    printf("Time: %ld nanoseconds\n", elapsed_ns);
}

static
void time_average() {
    double average = sum / (double)count;
    printf("Average Time: %.2f nanoseconds\n", average);
}

// ============================================================================
//  Main
// ============================================================================

int main(void) {
    // 1. Create context and publisher socket
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    
    // 2. Inicializa o buffer do MessagePack
    msgpack_sbuffer sbuffer;
    msgpack_sbuffer_init(&sbuffer);

    // 3. Inicializa o empacotador
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuffer, msgpack_sbuffer_write);

    // 4. Bind to a port
    int rc = zmq_bind(publisher, "tcp://*:5000");
    if (rc != 0) {
        printf("Error binding socket\n");
        return 1;
    }

    // Publica o buffer binário do MessagePack
    float ranges[1000];
    float intensities[1000];
    for (int i=0; i<1000; i++){
        ranges[i] = 10.0;
        intensities[i] = 1.56078;
    }


    static int a = 0;

    const char *payload = "Breaking: Local team wins championship!";

    for (int i=0; i<50; i++) {
        time_begin();
        msgpack_sbuffer_clear(&sbuffer);
        msgpack_pack_float(&pk, 1.0);
        msgpack_pack_float(&pk, 2.0);
        msgpack_pack_float(&pk, 3.0);
        msgpack_pack_float(&pk, 4.0);
        msgpack_pack_float(&pk, 5.0);
        msgpack_pack_float(&pk, 6.0);
        msgpack_pack_float(&pk, 7.0);

        msgpack_pack_array(&pk, 1000);
        for (int i=0; i<1000; i++) {
            msgpack_pack_float(&pk, ranges[i]);
        }

        msgpack_pack_array(&pk, 1000);
        for (int i=0; i<1000; i++) {
            msgpack_pack_float(&pk, intensities[i]);
        }

        // Send the body/payload frame
        zmq_send(publisher, sbuffer.data, sbuffer.size, 0);
        time_end();

        usleep(500000);
    }

    time_average();

    // Clean up (unreachable in infinite loop)
    zmq_close(publisher);
    zmq_ctx_destroy(context);
    return 0;
}