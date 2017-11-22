#include <sensor.h>
#include <iostream>
#include <sstream>
//#include <sensors/encoders.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>

using std::cout;
using std::endl;

namespace robosense
{
namespace sensor
{

class WhlEncoder : public Sensor
{
public:
  WhlEncoder();

  void analysis();

  ros::Publisher pub_lfront_;
  ros::Publisher pub_rfront_;
  ros::Publisher pub_lrear_;
  ros::Publisher pub_rrear_;
  ros::Publisher pub_mean_;
  double diameter_;

private:
  nav_msgs::Odometry msg_;
  std::vector<std::string> speeds_vec_;
  std::stringstream stream_;
};

WhlEncoder::WhlEncoder()
{
  head_ = "$WHEEL";
  diameter_ = 0.57;
}

void WhlEncoder::analysis()
{
  std::string wheel_msg;
  serial_.readline(wheel_msg);
  cout << "wheel msg line     " << wheel_msg << endl;
  speeds_vec_ = split(wheel_msg, ",");
  int ang_vec;
  if(speeds_vec_.size() >= 5)
  {
    if(head_ == speeds_vec_[0])
    {
      float speed_mean = 0;
      stream_.str(speeds_vec_[1]);
      stream_ >> ang_vec;   // 编码器输出 转/分钟
      stream_.clear();
      //msg_.lfront = temp*0.01;
      msg_.twist.twist.linear.x = ang_vec * 0.01 * M_PI * diameter_ / 60.0f;
      msg_.header.frame_id = "0";
      msg_.header.stamp = ros::Time::now();
      pub_lfront_.publish(msg_);
      ROS_INFO("pub_lfront_ =  %f ", msg_.twist.twist.linear.x);
      speed_mean += msg_.twist.twist.linear.x;

      stream_.str(speeds_vec_[2]);
      stream_ >> ang_vec;
      stream_.clear();
      // msg_.rfront = temp*0.01;
      msg_.twist.twist.linear.x = ang_vec * 0.01 * M_PI * diameter_ / 60.0f;
      msg_.header.frame_id = "0";
      msg_.header.stamp = ros::Time::now();
      pub_rfront_.publish(msg_);
      ROS_INFO("pub_rfront_ =  %f ", msg_.twist.twist.linear.x);
      speed_mean += msg_.twist.twist.linear.x;

      stream_.str(speeds_vec_[3]);
      stream_ >> ang_vec;
      stream_.clear();
      //msg_.lrear = temp*0.01;
      msg_.twist.twist.linear.x = ang_vec * 0.01 * M_PI * diameter_ / 60.0f;
      msg_.header.frame_id = "0";
      msg_.header.stamp = ros::Time::now();
      pub_lrear_.publish(msg_);
      ROS_INFO("pub_lrear_ =  %f ", msg_.twist.twist.linear.x);
      speed_mean += msg_.twist.twist.linear.x;

      stream_.str(speeds_vec_[4]);
      stream_ >> ang_vec;
      stream_.clear();
      //msg_.rrear = temp*0.01;
      msg_.twist.twist.linear.x = ang_vec * 0.01 * M_PI * diameter_ / 60.0f;
      msg_.header.frame_id = "0";
      msg_.header.stamp = ros::Time::now();
      pub_rrear_.publish(msg_);
      ROS_INFO("pub_rrear_ =  %f ", msg_.twist.twist.linear.x);
      speed_mean += msg_.twist.twist.linear.x;

      msg_.header.stamp = ros::Time().now();
      msg_.header.frame_id = "0";
      msg_.twist.twist.linear.x = 0.25 * speed_mean;
      pub_mean_.publish(msg_);
      ROS_INFO("pub_mean_ =  %f ", speed_mean);
    }
  }
}// class WhlEncoder
}//namespace sensor
}//namespace robosense

int main(int argc, char **argv)
{
  ros::init(argc, argv, "encoder");
  ros::NodeHandle private_nh("~");
  std::string port;
  int baud;
  //==================================================================
  if(private_nh.getParam(std::string("port"), port))
  {
  }
  else
  {
    port = "/dev/pts/18";
    ROS_INFO(" port is not setup");
  }
  cout << "port = " << port << endl;
  if(private_nh.getParam(std::string("baud"), baud))
  {
  }
  else
  {
    ROS_DEBUG("cannot find baud ");
    baud = 115200;
    ROS_INFO("baud is not setup ");
  }
  robosense::sensor::WhlEncoder whl_encoder;

  private_nh.getParam(std::string("diameter"), whl_encoder.diameter_);
  //ros::Publisher pub = private_nh.advertise<sensors::encoders>("encoders",100);

  whl_encoder.pub_lfront_ = private_nh.advertise<nav_msgs::Odometry>("encoder_lf", 100);
  whl_encoder.pub_rfront_ = private_nh.advertise<nav_msgs::Odometry>("encoder_rf", 100);
  whl_encoder.pub_lrear_ = private_nh.advertise<nav_msgs::Odometry>("encoder_lf", 100);
  whl_encoder.pub_rrear_ = private_nh.advertise<nav_msgs::Odometry>("encoder_rr", 100);
  whl_encoder.pub_mean_ = private_nh.advertise<nav_msgs::Odometry>("encoder_mean", 100);
  //ros::Publisher pub = private_nh.advertise<nav_msgs::Odometry>("encoders",100);
  whl_encoder.set(port, baud);
  whl_encoder.setHead("$WHEEL");
  //whl_encoder.setPublisher(pub);
  whl_encoder.open();
  if(whl_encoder.isOpen())
  {
    ROS_INFO("encoder port is open ");
    if(whl_encoder.checkhead())
    {
      ROS_INFO("port:%s is a encoder port  ", port.c_str());
    }
    else
    {
      ROS_INFO("port:%s is not a encoder port  ", port.c_str());
    }
  }
  else
  {
    ROS_INFO("encoder port is not open ");
  }
  //=====================================================


  // loop until shut  down or end of file
  while(private_nh.ok())
  {
    if(whl_encoder.isOpen())
    {
      whl_encoder.analysis();
    }
    else
    {
      private_nh.shutdown();
    }
    ros::spinOnce();
  }

  return 0;
}
