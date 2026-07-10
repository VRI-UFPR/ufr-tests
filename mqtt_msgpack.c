#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <msgpack.h>

#define BROKER_ADDRESS "localhost"
#define BROKER_PORT 1883
#define TOPIC "sensores/dados"

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

    printf("Conectado ao broker! Publicando dados...\n");

    // Publica o buffer binário do MessagePack
    for (int i=0; i<10000; i++) {
        msgpack_sbuffer_clear(&sbuffer);
        msgpack_pack_int(&pk, i);
        msgpack_pack_int(&pk, i*2);
        rc = mosquitto_publish(mosq, NULL, TOPIC, sbuffer.size, sbuffer.data, 0, false);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Erro ao publicar mensagem: %s\n", mosquitto_strerror(rc));
        } else {
            // printf("Mensagem publicada com sucesso!\n");
        }
    }

    // Aguarda e encerra a conexão
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    // Libera o buffer do MessagePack
    msgpack_sbuffer_destroy(&sbuffer);
    return 0;
}
