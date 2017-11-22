//
// Created by guoleiming on 17-7-7.
// 千寻跬步FindM RTD服务
//

#include <iostream>
#include <sstream>
#include <string>

#include <std_msgs/String.h>
#include <ros/ros.h>
#include <sensors/rtcm.h>

#include <sensor.h>
#include <sensors/qxwz_rtcm.h>
#include <qxwz_rtcm.h>
//
using std::cout;
using std::endl;
using std::string;

serial::Serial mycom_;
qxwz_account_info *p_account_info_ = NULL;
bool g_ntrip_ok_ = false;

void get_qxwz_sdk_account_info(void)
{
  p_account_info_ = getqxwzAccount();
  if(p_account_info_->appkey != NULL)
  {
    ROS_INFO("appkey=%s\n", p_account_info_->appkey);
  }
  if(p_account_info_->deviceID != NULL)
  {
    ROS_INFO("deviceID=%s\n", p_account_info_->deviceID);
  }
  if(p_account_info_->deviceType != NULL)
  {
    ROS_INFO("deviceType=%s\n", p_account_info_->deviceType);
  }

  if(p_account_info_->NtripUserName != NULL)
  {
    ROS_INFO("NtripUserName=%s\n", p_account_info_->NtripUserName);
  }
  if(p_account_info_->NtripPassword != NULL)
  {
    ROS_INFO("NtripPassword=%s\n", p_account_info_->NtripPassword);
  }
  ROS_INFO("expire_time=%ld\n", p_account_info_->expire_time);
}

void qxwz_rtcm_response_callback(qxwz_rtcm data)
{
  if(mycom_.isOpen())
  {
    int s = mycom_.write((const uint8_t *) data.buffer, data.length);
    //cout <<"write " << s <<endl;
  }
}

void qxwz_status_response_callback(qxwz_rtcm_status code)
{
  //printf("QXWZ_RTCM_STATUS:%d\n",code);
  ROS_INFO("QXWZ_RTCM_STATUS:%d\n", code);
  struct tm *ptr = NULL;
  g_ntrip_ok_ = false;
  //test account expire
  if(code == QXWZ_STATUS_OPENAPI_ACCOUNT_TOEXPIRE)
  {
    cout << "账号即将过期" << endl;
    //get_qxwz_sdk_account_info();
  }
  else if(QXWZ_STATUS_OPENAPI_ACCOUNT_EXPIRED == code)
  {
    cout << "账号已过期" << endl;
  }
  else if(QXWZ_STATUS_APPKEY_IDENTIFY_SUCCESS == code)
  {
    cout << "验证成功" << endl;
    g_ntrip_ok_ = true;
  }
  else if(QXWZ_STATUS_NTRIP_RECEIVING_DATA == code)
  {
    g_ntrip_ok_ = true;
  }
}

namespace robosense
{
namespace sensor
{
class FindM : public Sensor
{
public:
  FindM();

  void analysis();

  void setConfig(string appKey, string appSecret, string deviceId, string deviceType);

  bool checkhead();

  qxwz_config config_;

private:
  sensors::rtcm msg_;
  //std_msgs::String msg1_;
  std::vector<std::string> s_vec_;
  std::stringstream stream_;
  string gga_;

  friend void qxwz_rtcm_response_callback(qxwz_rtcm data);
};

FindM::FindM()
{
  head_ = "$GNGGA";
  ros::Time().now();
}

void FindM::setConfig(string appKey, string appSecret, string deviceId, string deviceType)
{
  config_.appkey = (char *) appKey.c_str();
  config_.appSecret = (char *) appSecret.c_str();
  config_.deviceId = (char *) deviceId.c_str();
  config_.deviceType = (char *) deviceType.c_str();
}

bool FindM::checkhead()
{
  std::string line;
  serial_.flushInput();
  serial_.close();
  qxwz_setting(&config_);
  mycom_.setPort(port_);
  qxwz_rtcm_start(qxwz_rtcm_response_callback, qxwz_status_response_callback);
  mycom_.setBaudrate(baud_);
  serial::Timeout timeout = serial::Timeout::simpleTimeout(300);
  mycom_.setTimeout(timeout);
  mycom_.open();
  int error = 0;
  for(int i = 0; i < 100; ++i)  //有的gps 发送多种格式的数据
  {
    mycom_.readline(line);

    if(line.find(" ") != line.npos)
    {
      line.replace(line.find(" "), 1, "");
    }

    std::cout << line;//<<std::endl;
    if(line.npos != line.find(head_))
    {
      return true;
    }
    line.clear();
  }
  return false;
}

void FindM::analysis()
{
  while(mycom_.isOpen())
  {
    string gps = mycom_.readline();
    if(gps.find(" ") != gps.npos)
    {
      gps.replace(gps.find(" "), 1, "");
    }
    if(gps.npos != gps.find(head_))
    {
      s_vec_ = split(gps, ",");
      ROS_INFO("%s", gps.c_str());
      qxwz_rtcm_sendGGAWithGGAString((char *) gga_.c_str());
      if("$GPGGA" == s_vec_[0] && "" != s_vec_[3]) //$GNGGA
      {
        gga_ = gps;
        if(g_ntrip_ok_)
        {
          qxwz_rtcm_sendGGAWithGGAString((char *) gga_.c_str());
          ROS_INFO("Send GGA done");
        }
        else
        {
          ROS_INFO("no ntrip service");
        }

        msg_.GPGGA = gps.c_str();

        msg_.header.stamp = ros::Time().now();
        msg_.header.frame_id = "/FindM";
        pub_.publish(msg_);
      }
    }
    usleep(10000);
  }
  usleep(80000);
}
}// namespace sensor
}// namespace robosense


int main(int argc, char **argv)
{
  ros::init(argc, argv, "gps");
  ros::NodeHandle private_nh("~");

  // private_nh.param(std::string("port"), port , std::string("/dev/pts/33"));
  //private_nh.param(std::string("baud"), baud,57600);
  std::string port;
  int baud;

  //=========================GPS=========================================
  if(private_nh.getParam(std::string("port"), port))
  {
  }
  else
  {
    port = "/dev/ttyUSB0";
    ROS_INFO(" port is not setup");
  }
  cout << "port = " << port << endl;
  if(private_nh.getParam(std::string("baud"), baud))
  {
  }
  else
  {
    baud = 9600;
    ROS_INFO("baud is set  to  9600 ");
  }
  robosense::sensor::FindM sensor;
  ros::Publisher pub = private_nh.advertise<sensors::rtcm>("FindM", 100);

  sensor.set(port, baud);
  sensor.setHead("GGA");
  sensor.setPublisher(pub);
  sensor.open();

  std::string value;
  if(private_nh.getParam(std::string("appKey"), value))
  {
    sensor.config_.appkey = (char *) value.c_str();
  }
  else
  {
    sensor.config_.appkey = (char *) "510328";
    ROS_INFO("appkey is set  to default 510328 ");
  }

  if(private_nh.getParam(std::string("appSecret"), value))
  {
    sensor.config_.appSecret = (char *) value.c_str();
  }
  else
  {
    sensor.config_.appSecret = (char *) "a9a55c74fa8797e5ba9ca482d83311b9a6341ab99996050345ad43a480bfded9";  //"请输入AppSecret";
    ROS_INFO("appSecret is set  to default   a9a55c74fa8797e5ba9ca482d83311b9a6341ab99996050345ad43a480bfded9 ");
  }

  if(private_nh.getParam(std::string("deviceId"), value))
  {
    sensor.config_.deviceId = (char *) value.c_str();
  }
  else
  {
    sensor.config_.deviceId = (char *) "123343455443";  //"请输入唯一的设备ID";
    ROS_INFO("deviceId is set  to default   123343455443 ");
  }

  if(private_nh.getParam(std::string("deviceType"), value))
  {
    sensor.config_.deviceType = (char *) value.c_str();
  }
  else
  {
    sensor.config_.deviceType = (char *) "gps"; //"请输入设备的类型";
    ROS_INFO("deviceType is set  to default   gps ");
  }

  if(sensor.isOpen())
  {
    ROS_INFO("port is open ");
    if(sensor.checkhead())
    {
      ROS_INFO("port:%s is a FindM port  ", port.c_str());
    }
    else
    {
      ROS_INFO("port:%s is not a FindM port ", port.c_str());
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
    if(mycom_.isOpen())
    {
      sensor.analysis();
    }
    ros::spinOnce();
    //================================================
  }
  return 0;
}
