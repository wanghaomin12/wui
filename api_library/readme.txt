api_library   


lib_library




这两个头文件是为无人机自主飞行任务开发的API库，提供了完整的无人机控制功能接口。让我详细解析：
文件结构概述
api_library.h - 核心API接口
高级无人机控制功能的主要接口
lib_library.h - 基础库函数
底层控制和辅助功能的支持库
主要功能模块分析
1. 基础飞行控制
状态监控
cpp
mavros_msgs::State current_state;           // 飞控状态
nav_msgs::Odometry local_pos;               // 位置信息
double roll, pitch, yaw;                    // 姿态角度
位置控制
cpp
// 自主巡航到指定位置
bool mission_pos_cruise(float x, float y, float z, float yaw, float error_max);

// 定点悬停
bool current_position_cruise(float x, float y, float z, float yaw);
2. 视觉识别与跟踪
目标识别跟踪
cpp
// 基于速度控制的目标跟踪
bool object_recognize_track_vel(string str, float yaw, float ALTITUDE, float speed, float error_max);

// 位置控制的目标跟踪
bool object_recognize_track(string str, float yaw, int reverse, float ALTITUDE, float error_max, float ctrl_coef);

// 全方向目标跟踪
bool object_recognize_track_omni(string str, float yaw, int reverse, float ALTITUDE, float error_max, float ctrl_coef);
视觉数据接口
cpp
geometry_msgs::PointStamped object_pos;     // 目标物体位置
double position_detec_x, position_detec_y, position_detec_z;  // 检测坐标
void object_pos_cb(const geometry_msgs::PointStamped::ConstPtr& msg);  // 回调函数
3. 二维码/标记识别
cpp
ar_track_alvar_msgs::AlvarMarker marker;    // AR标记信息
float marker_x, marker_y, marker_z;         // 标记位置
bool marker_found;                          // 发现标志
int ar_track_id_current;                    // 当前标记ID
4. 精确降落功能
cpp
// 基于目标识别的降落
bool object_recognition_land(string str, float land_threshold, float yaw, float ALTITUDE, float error_max);

// 基于二维码的降落
bool ar_lable_land(float marker_error_max, int ar_track_id, float ALTITUDE);

// 精确引导降落
void precision_land();
5. 任务执行功能
穿越任务
cpp
// 穿越圆框等障碍物
bool target_through(float pos_x, float pos_y, float z, float yaw);
辅助控制
cpp
// PWM舵机控制（用于投放等机械动作）
void lib_pwm_control(int pwm_channel_5, int pwm_channel_6);

// 时间记录函数（任务时序控制）
bool lib_time_record_func(float time_duration, ros::Time time_now);
技术特点
1. 分层设计
api_library.h: 面向应用的高级接口 
lib_library.h: 底层基础功能支持 
2. 状态管理
每个功能都有相应的状态标志和位置记录：
cpp
bool mission_pos_cruise_flag = false;
float mission_pos_cruise_last_position_x = 