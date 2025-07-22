#ifndef __API_LIBRARY__
#define __API_LIBRARY__

#include <string> 
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/TwistStamped.h>
#include <mavros_msgs/PositionTarget.h>
#include <cmath>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <mavros_msgs/CommandLong.h>   
#include <string>
#include <geometry_msgs/Twist.h>
#include <ar_track_alvar_msgs/AlvarMarkers.h>
 
using namespace std;

mavros_msgs::PositionTarget setpoint_raw;

//mavros控制相关函数
//1、定义变量，用于接收无人机的状态信息
mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg);


//2、定义变量，用于接收无人机的里程计信息,定义存储r\p\y的变量，定义四元素变量
tf::Quaternion quat; 
nav_msgs::Odometry local_pos; 
double roll, pitch, yaw;
float init_position_x_take_off =0;
float init_position_y_take_off =0;
float init_position_z_take_off =0;
bool  flag_init_position = false;
void  local_pos_cb(const nav_msgs::Odometry::ConstPtr& msg);


//3、室内外通用自主巡航函数API，通过发布目标位置，控制无人机到达目标点
float mission_pos_cruise_last_position_x = 0;
float mission_pos_cruise_last_position_y = 0;
bool  mission_pos_cruise_flag = false;
bool  mission_pos_cruise(float x, float y, float z, float yaw, float error_max);

//4、通过定点飞行，发布无人机的目标位置
float current_position_cruise_last_position_x = 0;
float current_position_cruise_last_position_y = 0;
bool  current_position_cruise_flag = false;
bool  current_position_cruise(float x, float y, float z, float yaw);

//5、室内外目标识别，采用速度控制运动，任务是识别到目标后，保持无人机正对着目标
float object_recognize_track_vel_last_time_position_x = 0;
float object_recognize_track_vel_last_time_position_y = 0;
bool  object_recognize_track_vel_flag = false;
bool  object_recognize_track_vel(string str, float yaw, float ALTITUDE, float speed, float error_max);


//室内使用接口
//yolov识别相关函数 
//6、目标识别，这个任务是识别目标后，保持无人机正对着目标,已经做了验证，效果良好。有时间后可以进一步优化
float object_recognize_track_last_time_position_x = 0;
float object_recognize_track_last_time_position_y = 0;
bool  object_recognize_track_flag = false;
bool  object_recognize_track(string str, float yaw, int reverse, float ALTITUDE, float error_max,float ctrl_coef);


//6.1、目标识别，全方向的。
float object_recognize_track_omni_last_time_position_x = 0;
float object_recognize_track_omni_last_time_position_y = 0;
bool  object_recognize_track_omni_flag = false;
bool  object_recognize_track_omni(string str, float yaw, int reverse, float ALTITUDE, float error_max, float ctrl_coef);


//7、识别指定目标进行降落,此处代码尚未实际验证，待验证
float object_recognition_land_last_position_x;
float object_recognition_land_last_position_y;
bool  object_recognition_land_init_position_flag = false;
bool  object_recognition_land(string str,float land_threshold,float yaw, float ALTITUDE, float error_max);


//8、USB二维摄像头只能给出目标在图像中的位置
geometry_msgs::PointStamped object_pos; 
double position_detec_x = 0;
double position_detec_y = 0;
double position_detec_z = 0;
void   object_pos_cb(const geometry_msgs::PointStamped::ConstPtr& msg);








//二维码识别相关
//9、定义变量，用于接收二维码识别返回值
ar_track_alvar_msgs::AlvarMarker marker;
float marker_x = 0, marker_y = 0, marker_z = 0;
bool  marker_found = false;
int   ar_track_id_current = 0;
void  ar_marker_cb(const ar_track_alvar_msgs::AlvarMarkers::ConstPtr &msg);

//10、二维码降落,此处代码尚未实际验证，待验证
float ar_lable_land_last_position_x = 0;
float ar_lable_land_last_position_y = 0;
bool  ar_lable_land_init_position_flag = false;
bool  ar_lable_land(float marker_error_max, int ar_track_id, float ALTITUDE);












//避障相关
//13、订阅和发布函数
void CmdVelCallback(const geometry_msgs::Twist &msg);



//穿越圆框
//15、穿越圆框，已经验证。直接发布圆框后方目标点，效果良好，有时间以后可以继续优化
float target_through_init_position_x = 0;
float target_through_init_position_y = 0;
bool  target_through_init_position_flag = false;
bool  target_through(float pos_x, float pos_y, float z, float yaw);
 

//降落相关
//16、引导降落函数
float precision_land_init_position_x = 0;
float precision_land_init_position_y = 0;
bool  precision_land_init_position_flag = false;
void  precision_land();


#endif
