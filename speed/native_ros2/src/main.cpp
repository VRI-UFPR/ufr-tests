// ============================================================================
//  Header
// ============================================================================

#include <chrono>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"

using namespace std::chrono_literals;

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

int main(int argc, char ** argv)
{
  // 1. Inicializa o contexto do ROS 2
  rclcpp::init(argc, argv);

  // 2. Cria um nó genérico para gerenciar a publicação
  auto node = rclcpp::Node::make_shared("laser_scan_standalone_node");

  // 3. Cria o publicador no tópico "/scan"
  auto publisher = node->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);

  // 4. Cria um timer para publicar a 1 Hz (a cada 1 segundo)
  rclcpp::WallRate loop_rate(1s);

  // Variavel de exemplo para simular incrementos de tempo
  float count = 0.0;
  
  // Prepara a mensagem LaserScan
  auto scan_msg = std::make_shared<sensor_msgs::msg::LaserScan>();
  // Preenche os dados de leitura (exemplo simulando obstáculos)
  const size_t num_readings = 1000; // 6.28 / 0.1 aprox 63
  scan_msg->ranges.resize(num_readings);
  scan_msg->intensities.resize(num_readings);

  geometry_msgs::msg::TransformStamped tf_msg;



  auto tf_broadcaster = std::make_shared<tf2_ros::TransformBroadcaster>(node);

  // 5. Loop principal de execução
  for (int i=0; i<50 && rclcpp::ok(); i++) {
    time_begin();

    auto stamp = node->get_clock()->now();

    // --- Publicar TF (Base para o Laser) ---
    /*
    tf_msg.header.stamp = stamp;
    tf_msg.header.frame_id = "base_link";
    tf_msg.child_frame_id = "laser_frame";

    // Posição física do sensor na base do robô
    tf_msg.transform.translation.x = 0.2;
    tf_msg.transform.translation.y = 0.0;
    tf_msg.transform.translation.z = 0.1;
    
    // Rotação simples em radianos (usando 0 no exemplo)
    tf_msg.transform.rotation.x = 0.0;
    tf_msg.transform.rotation.y = 0.0;
    tf_msg.transform.rotation.z = 0.0;
    tf_msg.transform.rotation.w = 1.0;

    tf_broadcaster->sendTransform(tf_msg);
    */


    // Preenche o cabeçalho (Header)
    scan_msg->header.stamp = stamp;
    scan_msg->header.frame_id = "laser_frame";

    // Propriedades do Laser
    scan_msg->angle_min = 0.0;
    scan_msg->angle_max = 6.28; // 360 graus em radianos
    scan_msg->angle_increment = 0.1; // passos de angulo
    scan_msg->time_increment = 0.0;
    scan_msg->scan_time = 0.1;
    scan_msg->range_min = 0.1;
    scan_msg->range_max = 10.0;

  for(size_t i = 0; i < num_readings; ++i) {
    scan_msg->ranges[i] = 10.0; // distância simulada
    scan_msg->intensities[i] = 1.56078; // intensidade do retorno
  }

    // Publica a mensagem
    publisher->publish(*scan_msg);
    time_end();

    // Espera para enviar os dados
    usleep(500000);
  }

  // 6. Encerra o ROS 2 corretamente
  time_average();
  rclcpp::shutdown();
  return 0;
}