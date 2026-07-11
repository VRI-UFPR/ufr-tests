#include <ufr.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct timespec start, end;

void time_begin() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void time_end() {
    clock_gettime(CLOCK_MONOTONIC, &end);
    int64_t elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + 
            (end.tv_nsec - start.tv_nsec);

    printf("Time: %ld nanoseconds\n", elapsed_ns);
}


int main() {
    ufr_stdout("@new ros2 @coder ros2:laserscan @topic /scan @log 0");

    sleep(1);
    float ranges[1100];
    for (int i=0; i<20; i++) {
        time_begin();
        /*ufr_printf("time_increment:%f scan_time:%f range_min:%f range_max:%f", 1.0, 1.0, 1.0, 20.0);
        ufr_printf("angle_min:%f angle_max:%f angle_increment:%f", 0.0, 15.0, 0.25);
        for (int j=0; j<1100; j++){
            ranges[j] = 2.0;
        }
        ufr_printf("ranges: %af\n", 1100, ranges);*/

        ufr_printf("%f %f %f", 0.0, 15.0, 0.25);
        ufr_printf("%f %f %f %f", 1.0, 1.0, 1.0, 20.0);
        ufr_printf("%af\n", 1100, ranges);

        time_end();
        // ufr_printf("intensities=%a:0:?\n", range_size);
        // ufr_put_af32(link, range_ptr, range_size);
        // ufr_send(link);
        // printf("dormindo\n");
        sleep(1);
    }

    // ufr_close(fd);
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