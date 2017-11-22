### TF setup
[ros参考](http://wiki.ros.org/navigation/Tutorials/RobotSetup/TF)

Many ROS packages require the transform tree of a robot to be published using the tf software library. At an abstract level, a transform tree defines offsets in terms of both translation and rotation between different coordinate frames.

Tf uses a tree structure to guarantee that there is only a single traversal that links any two coordinate frames together, and assumes that all edges in the tree are

The nav_msgs/Odometry message stores an estimate of the position and velocity of a robot in free space:



canbus   base_link


