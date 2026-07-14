// ============================================================================
//  Header
// ============================================================================

#include <ufr.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

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
    ufr_stdout("@new mqtt @coder msgpack @topic /scan @log 0");
    // ufr_stdout("@new zmq @coder msgpack @log 0");
    // ufr_stdout("@new ros2 @coder ros2:laserscan @topic /scan @log 0");
    
    sleep(1);

    float ranges[1000];
    float intensities[1000];
    for (int i=0; i<1000; i++){
        ranges[i] = 10.0;
        intensities[i] = 1.56078;
    }

    for (int i=0; i<50; i++) {
        time_begin();
        ufr_printf("time_increment=%f scan_time=%f", 1.0, 2.0);
        ufr_printf("range_min=%f range_max=%f", 3.0, 4.0);
        ufr_printf("angle_min=%f angle_max=%f angle_increment=%f", 5.0, 6.0, 7.0);
        ufr_printf("ranges=%a:f:1000", ranges);
        ufr_printf("intensities=%a:f:1000\n", intensities);
        time_end();

        // Necessario um delay, senão ele envia apenas uma única mensagem ao inves de 50
        usleep(500000);
    }

    time_average();
    return 0;
}

/*

Aprendizagem do UFR
 - instalação usando pacote DEB (requisitos mosquitto, msgpack, ...)
 - ufr_publisher: enviar dados para um servidor web (x,y)
 - ufr_subscriber: receber dados de um servidor web (odometria das rodas) e calcular o X e Y
   - pode-se usar diferentes configurações (tamanho do robo, qtde de rodas, robotino 3 (3 rodas))
 - ufr_server: servidor web single thread
 - ufr_client: requisição ao servidor web

 - ufr_publisher_extra:
   - enviar dados simples com um vetor
   - enviar dados simples com uma imagem


navegação básica
 - odometria x, y e th
 - mover do ponto a outro ponto
 - distancia euclidiana

navegação seguir parede
 - lidar para medir a distancia da parede
 - achar a parede mais proxima
 - ir até a parede
 - seguir a parede

navegação desviar obstaculos
 - precisa de uma navegação razoavel funcionando
 - ideal que conseguisse detectar o obstaculo antes de bater

*/
