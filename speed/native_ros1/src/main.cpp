#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2/LinearMath/Quaternion.h>
#include <cmath>


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





int main(int argc, char** argv) {
    // Initialize the ROS node
    ros::init(argc, argv, "laser_and_tf_publisher");
    ros::NodeHandle nh;

    // Create a publisher for the "scan" topic
    ros::Publisher scan_pub = nh.advertise<sensor_msgs::LaserScan>("scan", 50);

    // Create a TF broadcaster instance
    tf2_ros::TransformBroadcaster tf_broadcaster;

    // Set the loop execution rate to 40 Hz
    ros::Rate r(40.0);

    // Laser specification parameters
    double scan_frequency = 40.0;
    double num_readings = 1000;
    
    // Laser field of view: -120 to +120 degrees (in radians)
    double angle_min = -2.0 / 3.0 * M_PI; 
    double angle_max = 2.0 / 3.0 * M_PI;  
    double angle_increment = (angle_max - angle_min) / (num_readings - 1);
    
    double range_min = 0.1;   // 10 cm
    double range_max = 30.0;  // 30 meters

    unsigned int count = 0;

    for (int i=0; i<50; i++){ 
        ros::Time current_time = ros::Time::now();

        // ==========================================
        // 1. BROADCAST TF TRANSFORM (base_link -> laser_frame)
        // ==========================================
        /*
        geometry_msgs::TransformStamped odom_trans;
        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "base_link";   // Parent frame
        odom_trans.child_frame_id = "laser_frame"; // Child frame (matches laser scan frame_id)

        // Position offset: LiDAR is mounted 20cm forward and 10cm up on the robot
        odom_trans.transform.translation.x = 0.2;
        odom_trans.transform.translation.y = 0.0;
        odom_trans.transform.translation.z = 0.1;

        // Rotation: Assuming the LiDAR is pointed straight forward (0 roll, pitch, yaw)
        tf2::Quaternion q;
        q.setRPY(0, 0, 0); // Roll, Pitch, Yaw in radians
        odom_trans.transform.rotation.x = q.x();
        odom_trans.transform.rotation.y = q.y();
        odom_trans.transform.rotation.z = q.z();
        odom_trans.transform.rotation.w = q.w();

        // Send the transform to the TF tree
        tf_broadcaster.sendTransform(odom_trans);
        */

        // ==========================================
        // 2. POPULATE AND PUBLISH LASER SCAN
        // ==========================================
        time_begin();
        sensor_msgs::LaserScan scan;
        scan.header.stamp = current_time;
        scan.header.frame_id = "laser_frame"; // Frame must match the TF child_frame_id
        scan.angle_min = 1.0;
        scan.angle_max = 2.0;
        scan.angle_increment = 3.0;
        scan.time_increment = 4.0;
        scan.range_min = 5.0;
        scan.range_max = 6.0;

        // Populate mock distance data arrays
        scan.ranges.resize(num_readings);
        scan.intensities.resize(num_readings);
        for(size_t i = 0; i < num_readings; ++i) {
            scan.ranges[i] = 10.0; // distância simulada
            scan.intensities[i] = 1.56078; // intensidade do retorno
        }

        scan_pub.publish(scan);
       
        time_end();
 
        count++;
        r.sleep();
        usleep(500000);
    }
  
    time_average();  
    return 0;
}
