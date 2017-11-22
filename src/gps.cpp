#include <iostream>
#include <sstream>
#include "std_msgs/String.h"
#include <sensors/gprmc.h>
#include <string>
#include <sensor.h>
#include <ros/ros.h>
//
using std::cout;
using std::endl;

namespace robosense
{
namespace sensor
{
class GPS : public Sensor
{
public:
  GPS();
  void analysis();

private:
  sensors::gprmc msg_;
  //std_msgs::String msg1_;
  std::vector<std::string> s_vec_;
  std::stringstream stream_;
};


GPS::GPS()
{
  head_ = "$GPRMC";
  ros::Time().now();
}

void GPS::analysis()
{
  std::string line;
  serial_.readline(line);

  if(line.find(" ") != line.npos)
  {
    line.replace(line.find(" "), 1, "");
  }


  if(0 == line.find(head_))
  {
    //msg1_.data= line;
    ROS_INFO("%s", line.c_str());
    //pub_.publish(msg1_);

    stream_.str(line);
    stream_ >> msg_.initdata_0;
    stream_.clear();
    s_vec_ = split(line, ",");
    if("$GPRMC" == s_vec_[0] || "$GNRMC" == s_vec_[0])
    {
      msg_.rmc_1 = s_vec_[0];

      msg_.utc_time_2 = s_vec_[1];
      msg_.status_3 = s_vec_[2];

      stream_.str(s_vec_[3]);
      stream_ >> msg_.latitude_4;
      stream_.clear();

      msg_.N_5 = s_vec_[4];

      stream_.str(s_vec_[5]);
      stream_ >> msg_.longitude_6;
      stream_.clear();

      msg_.E_7 = s_vec_[6];

      stream_.str(s_vec_[7]);
      stream_ >> msg_.spd_8;
      stream_.clear();


      stream_.str(s_vec_[8]);
      stream_ >> msg_.cog_9;
      stream_.clear();

      msg_.utc_day_10 = s_vec_[9];

      stream_.str(s_vec_[10]);
      stream_ >> msg_.mv_11;
      stream_.clear();

      msg_.mvE_12 = s_vec_[11];
      msg_.mode_13 = s_vec_[12];
      /*
             msg_.cs_14  = s_vec_[13];
           */

      msg_.header.stamp = ros::Time().now();
      msg_.header.frame_id = "/gps";
      pub_.publish(msg_);
      //ROS_INFO("msg %s,%s ",msg_.rmc_1.c_str(), msg_.utc_time_2.c_str());
      // ROS_INFO("msg %s, %s, %s, %f, %s, %f, %s, %f, %f, %s, %f, %s, %s, %s, ",msg_.rmc_1.c_str(), msg_.utc_time_2.c_str(), msg_.status_3.c_str(), msg_.latitude_4, msg_.N_5.c_str() ,msg_.longitude_6, msg_.E_7.c_str(), msg_.spd_8 , msg_.cog_9 ,msg_.utc_day_10.c_str(), msg_.mv_11 , msg_.mvE_12.c_str(), msg_.mode_13.c_str() , msg_.cs_14.c_str());
    }
  }
}
}// namespace sensor
}//namespace robosense


int main(int argc, char **argv)
{
  ros::init(argc, argv, "gps");
  ros::NodeHandle private_nh("~");
  std::string port;
  int baud;

  //=========================GPS=========================================
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
    baud = 115200;
    ROS_INFO("baud is not setup ");
  }
  robosense::sensor::GPS gps;
  ros::Publisher pub = private_nh.advertise<sensors::gprmc>("gps", 100);

  gps.set(port, baud);
  gps.setHead("$GNRMC");
  gps.setPublisher(pub);
  gps.open();

  if(gps.isOpen())
  {
    ROS_INFO("port is open ");
    if(gps.checkhead())
    {
      ROS_INFO("port:%s is a gps port  ", port.c_str());
    }
    else
    {
      ROS_INFO("port:%s is not a gps port ", port.c_str());
      private_nh.shutdown();
    }
  }
  else
  {
    ROS_INFO("port is not open ");
    private_nh.shutdown();
  }
  //=====================================================

  // loop until shut  down or end of file
  while(private_nh.ok())
  {

    if(gps.isOpen())
    {
      gps.analysis();
    }
    ros::spinOnce();
    //================================================
  }
  return 0;
}
