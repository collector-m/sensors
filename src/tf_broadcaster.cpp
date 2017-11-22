//
// Created by guoleiming on 17-8-7.
//

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char** argv){
  ros::init(argc, argv, "robot_tf_publisher");
  ros::NodeHandle n;

  ros::Rate r(100);

  tf::TransformBroadcaster
          broadcaster1,
          broadcaster2,
          broadcaster3,
          broadcaster4,
          broadcaster5;

  while(n.ok()){
    broadcaster1.sendTransform(
      tf::StampedTransform(
        tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2)),
        ros::Time::now(),"base_link", "base_rslidar"));

    broadcaster2.sendTransform(
            tf::StampedTransform(
                    tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2)),
                    ros::Time::now(),"base_link", "base_imu"));


    broadcaster3.sendTransform(
            tf::StampedTransform(
                    tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2)),
                    ros::Time::now(),"base_link", "base_gps"));

    broadcaster3.sendTransform(
            tf::StampedTransform(
                    tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2)),
                    ros::Time::now(),"base_link", "base_gps"));


    broadcaster3.sendTransform(
            tf::StampedTransform(
                    tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2)),
                    ros::Time::now(),"base_link", "base_gps"));
    r.sleep();
  }
}