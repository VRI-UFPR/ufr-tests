// ============================================================================
//  Header
// ============================================================================

#include <msgpack/pack.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <msgpack.h>
#include <pthread.h>
#include <math.h>

#define BROKER_ADDRESS "127.0.0.1"
#define BROKER_PORT 1883
#define TOPIC "/scan"

// ============================================================================
//  Medição do Tempo
// ============================================================================

int count = 0;
int64_t sum = 0;
struct timespec start, end;

double times[100];

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
    times[count] = (double) elapsed_ns;
    count += 1;
    printf("Time: %ld nanoseconds\n", elapsed_ns);
}

static
void time_average() {
    double media = sum / (double)count;
    // printf("Average Time: %.2f nanoseconds\n", media);

    double variancia_soma = 0.0;
    for (int i = 0; i < count; i++) {
        const double val = (times[i] - media);
        variancia_soma += val * val;
    }
    double desvio_padrao = sqrt(variancia_soma / count);

    // Exibição dos resultados
    printf("\nMédia: %.2f\n", media);
    printf("Desvio Padrão: %.2f\n", desvio_padrao);
}

// ============================================================================
//  Main
// ============================================================================

int main() {
    // 1. Inicializa o buffer do MessagePack
    msgpack_sbuffer sbuffer;
    msgpack_sbuffer_init(&sbuffer);

    // 2. Inicializa o empacotador
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuffer, msgpack_sbuffer_write);
   
    // 3. Inicializa a biblioteca Mosquitto
    mosquitto_lib_init();

    // Cria uma nova instância de cliente
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Erro ao criar instância do Mosquitto\n");
        return 1;
    }

    // Conecta ao broker MQTT
    int rc = mosquitto_connect(mosq, BROKER_ADDRESS, BROKER_PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Erro ao conectar ao broker: %s\n", mosquitto_strerror(rc));
        return 1;
    }
    sleep(1);

    printf("Conectado ao broker! Publicando dados...\n");

    // Publica o buffer binário do MessagePack
    float ranges[1000];
    float intensities[1000];
    for (int i=0; i<1000; i++){
        ranges[i] = 10.0;
        intensities[i] = 1.56078;
    }

    for (int i=0; i<50; i++) {
        // Envia o pacote
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

        rc = mosquitto_publish(mosq, NULL, TOPIC, sbuffer.size, sbuffer.data, 0, false);
        time_end();

        // printf("%ld\n", sbuffer.size);

        // Verifica se enviou
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Erro ao publicar mensagem: %s\n", mosquitto_strerror(rc));
        }

        // Necessario um delay, senão ele envia apenas uma única mensagem
        usleep(500000);
    }

    time_average();

    // Aguarda e encerra a conexão
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    // Libera o buffer do MessagePack
    msgpack_sbuffer_destroy(&sbuffer);
    return 0;
}
