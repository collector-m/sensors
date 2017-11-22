# 使用说明
## 1 功能概述
使用串口通讯获得车载gps,车轮编码器,IMU,odb等的数据，并使用标准的ros消息和广播机制将数据发出


|Name           |消息类型　|单位　　 |　　　　　备注　　　|
|---------------|-----------------------|-----------------------|-------
|encoder         | nav_msgs::Odometry   | m/s |twist.twist.linear.x
|IMU          | sensor_msgs::Imu             |               |四元素
|canbus           | nav_msgs::Odometry         | m/s     |twist.twist.linear.x
|gps          | sensors::gprmc            | 字符串         | 自定义数据类型
|FindM    | sensors::rtcm            | 字符串             |自定义数据类型


## 2 安装
### 1 安装第三方库`serial`
[本地路径](3rdlib/serial)或者直接在[github]( https://github.com/wjwwood/serial.git) 上下载
```
mkdir build
cd build
cmake ../
make
sudo make install
```
### 2 编译工程
```
 catkin_ws catkin_make --pkg sensors
```
### 3 rosrun node
进入catkin_ws根目录,打开终端, 行命令行
 　　
```
roscore
```


#### 运行 encoder ros节点　　

打开一个新的终端，运行

```
source devel/setup.bash
rosrun sensors encoder _baud:=115200 _port:="/dev/ttyUSB2"
```

####  运行 gps ros节点　　

打开一个新的终端，运行　　　

```
source devel/setup.bash
rosrun sensors gps _name:=rtk _baud:=115200 _port:="/dev/ttyUSB1"      //
```

#### 运行IMU　　　

打开一个新的终端，运行
　　　
```
source devel/setup.bash
rosrun sensors nav440 _name:=nav_IMU _baud:=38400 _port:="/dev/ttyUSB0"    //黄色惯导模块，用来做返回topic: imu
```


####  运行千寻跬步FindM　　　

打开一个新的终端，运行

```
source devel/setup.bash
rosrun sensors findm _name:=rtk _baud:=115200 _port:="/dev/ttyUSB0" _appKey:="510328" _appSecret:="a9a55c74fa8797e5ba9ca482d83311b9a6341ab99996050345ad43a480bfded9" _deviceId:="123343455443" _deviceType:="gps"  //
```



__注意使用有效的key和secret替换上面数据__　　　　





##  3 参考资料
IMU [NAV440说明书](doc/440 Series Inertial -Manual.pdf)

GPS[西博泰科gps ](doc/CNT803H-36G GNSS惯性组合导航定位接收机 [西博泰科] .pdf)

RTK [北京星宇rtk ](doc/XW-GI5651使用维护说明书.doc)

千寻服务[开发指南_V1.3.0.pdf](doc/差分数据LinuxSDK开发指南_V1.3.0.pdf)


##  常见问题

- 1 `Unable to open serial port /dev/ttyUSB0 ubuntu`

   原因：用户没有设备访问权限,通过增加udev规则解决。
   创建文件

   ```sudo gedit /etc/udev/rules.d/70-ttyusb.rules```

   在文件内增加一行
   `KERNEL=="ttyUSB[0-9]*", MODE="0666"`

   保存，退出，重新插入USB转串口设备。

__如果遇到问题，请提交反馈到研发部__ [gitlab issues ](http://192.168.1.20/glm/sensors/issues)






