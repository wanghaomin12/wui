#ifndef __OBJECT_DARKNET_H__ 
#define __OBJECT_DARKNET_H__

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/PointStamped.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/Image.h>
#include "/home/cwkj/cwkj_ws/src/object_darknet/include/BoundingBoxes.h"
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include "tf/transform_datatypes.h"
#include "tf/transform_listener.h"



using namespace cv;
using namespace std;

class MoveObject
{
private:
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_color; 
    image_transport::Subscriber image_sub_depth; 

    ros::Subscriber Object_sub; 
    ros::Subscriber camera_info_sub_; //subscribe the topic, which pubbed by depth image

    ros::Publisher  position_pub ;   

    sensor_msgs::CameraInfo camera_info;
    //geometry_msgs::PointStamped output_point;

    /* Mat depthImage,colorImage; */
    Mat colorImage;
    Mat depthImage = Mat::zeros(480, 640, CV_16UC1); //size of image
    Point mousepos = Point(0, 0);                    /* mousepoint to be map */

    actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_base;
    ros::ServiceClient pose;
    tf::TransformListener listener;

    float real_x, real_y, real_z;
    bool detect_object , get_object_position, getflagsuccess, robot_move;   //running flag
    geometry_msgs::PoseStamped Object_pose, Object_pose_tmp;


public:
    MoveObject();
    ~MoveObject();
    bool move_finish;
    void ObjectCallback(const darknet_ros_msgs::BoundingBoxes &object_tmp);
    void cameraInfoCb(const sensor_msgs::CameraInfo &msg);
    void imageDepthCb(const sensor_msgs::ImageConstPtr &msg);
    void imageColorCb(const sensor_msgs::ImageConstPtr &msg);
};
#endif

