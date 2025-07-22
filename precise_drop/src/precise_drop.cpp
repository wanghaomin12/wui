#include "api_library.h"
#include "lib_library.h"
#define ALTITUDE  2

int mission_num = 0; 

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");

	ros::init(argc, argv, "precise_drop");

  	ros::NodeHandle nh; 

	ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);   

	ros::Subscriber local_pos_sub = nh.subscribe<nav_msgs::Odometry>("/mavros/local_position/odom", 10, local_pos_cb);

    ros::Subscriber object_pos_sub = nh.subscribe<geometry_msgs::PointStamped>("object_position", 100, object_pos_cb); 
     
	ros::Publisher  mavros_setpoint_pos_pub = nh.advertise<mavros_msgs::PositionTarget>("/mavros/setpoint_raw/local", 100);  
	 
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");

    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");   

    ros::ServiceClient ctrl_pwm_client = nh.serviceClient<mavros_msgs::CommandLong>("mavros/cmd/command");

    ros::Rate rate(20);
   
    while(ros::ok() && !current_state.connected)
    {
        ros::spinOnce();
        rate.sleep();
    }
	setpoint_raw.type_mask = /*1 + 2 + 4 + 8 + 16 + 32*/ + 64 + 128 + 256 + 512 /*+ 1024 + 2048*/;
	setpoint_raw.coordinate_frame = 1;
	setpoint_raw.position.x = init_position_x_take_off;
	setpoint_raw.position.y = init_position_y_take_off;
	setpoint_raw.position.z = init_position_z_take_off + ALTITUDE;
	setpoint_raw.yaw        = 0;

    for(int i = 100; ros::ok() && i > 0; --i)
    {
		mavros_setpoint_pos_pub.publish(setpoint_raw);
        ros::spinOnce();
        rate.sleep();
    }

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";
    
    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();

    lib_pwm_control(20,20);
    ctrl_pwm_client.call(lib_ctrl_pwm);
    
    while(ros::ok())
    {
        if( current_state.mode != "OFFBOARD" && (ros::Time::now() - last_request > ros::Duration(5.0)))
        {
            if( set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent)
            {
                ROS_INFO("Offboard enabled");
            }
           	last_request = ros::Time::now();
           	flag_init_position = false;		    
       	}
        else 
		{
			if( !current_state.armed && (ros::Time::now() - last_request > ros::Duration(5.0)))
			{
		        if( arming_client.call(arm_cmd) && arm_cmd.response.success)
		       	{
		            ROS_INFO("Vehicle armed");
		        }
		        last_request = ros::Time::now();
		        flag_init_position = false;		    
			}
		}
	    
	    //1、添加高度判断，使得无人机跳出模式切换循环
	    if(fabs(local_pos.pose.pose.position.z- init_position_z_take_off -ALTITUDE)<0.5)
		{	
			if(ros::Time::now() - last_request > ros::Duration(3.0))
			{
				mission_num = 1;
				break;
			}
		}
		//2、添加时间判断，使得无人机跳出模式切换循环
		if(ros::Time::now() - last_request > ros::Duration(10.0))
		{
			mission_num = 1;
			break;
		}
		//此处添加是为增加无人机的安全性能，在实际测试过程中，采用某款国产的GPS和飞控，气压计和GPS定位误差极大，
		//导致了无人机起飞后直接飘走，高度和位置都不正常，无法跳出模式循环，导致遥控且无法接管
		//因此增加了时间判断，确保无人机在切入offboard模式和解锁后，确保任何情况下，8秒后遥控器都能切入其他模式接管无人机	
		//注意：一定要确定GPS和飞控传感器都是正常的
		//注意：一定要确定GPS和飞控传感器都是正常的
		//注意：一定要确定GPS和飞控传感器都是正常的
		//注意：一定要确定GPS和飞控传感器都是正常的
		//注意：一定要确定GPS和飞控传感器都是正常的
		mission_pos_cruise(0, 0, ALTITUDE, 0, 0.5);		
		mavros_setpoint_pos_pub.publish(setpoint_raw);
        ros::spinOnce();
        rate.sleep();
    } 

	bool flag_arrive = false;
	while(ros::ok())
    {
    	printf("mission_num = %d\r\n",mission_num);
    	switch(mission_num)
    	{	
    		case 1:
    			//通过定点飞行，发布无人机的目标位置
    			if(mission_pos_cruise(3, 0, ALTITUDE, 0, 0.5))
    			{
    				if(lib_time_record_func(3.0,ros::Time::now()))
    				{
    					mission_num=2;
    				}
    			}
    			break;
    		case 2:	
				if(object_pos.header.frame_id == "landing" || object_pos.header.frame_id == "landing")
    			{
    				if(object_recognize_track_vel(object_pos.header.frame_id, 0, ALTITUDE, 0.15, 40))
					{
						if(lib_time_record_func(2.0,ros::Time::now()))
						{
							lib_pwm_control(100,100);
							ctrl_pwm_client.call(lib_ctrl_pwm);
							ROS_INFO("舵机执行物体投放");
							mission_num=3;
						}
					}
    			}
    			else
    			{
    				if(lib_time_record_func(30.0,ros::Time::now()))
					{
						mission_num=3;
					}
    			}
    			break;	   			
    		case 3:
    			if(mission_pos_cruise(0, 0, ALTITUDE, 0, 0.5))
    			{
    				if(lib_time_record_func(3.0,ros::Time::now()))
    				{
    					mission_num=4;
    				}
    			}
    			break;	   		
    		case 4:	
				if(object_pos.header.frame_id == "landing" || object_pos.header.frame_id == "ring")
    			{
    				if(object_recognize_track_vel(object_pos.header.frame_id, 0, ALTITUDE, 0.2, 30))
					{
							flag_arrive = true;
							ROS_INFO("进入自动降落模式,任务结束");
							break;
					}
    			}
    			else
    			{
    				if(lib_time_record_func(30.0,ros::Time::now()))
					{
						mission_num=4;
					}
    			}
    			break;		
    	}
    	//此处使用高度低于起飞初始值，可以利用位置环的PID控制，有效的抵消风的影响
		if(flag_arrive == true)
		{
			printf("23456\r\n");
			setpoint_raw.type_mask = /*1 + 2 + 4 + 8 + 16 + 32*/ + 64 + 128 + 256 + 512 /*+ 1024 + 2048*/;
			setpoint_raw.coordinate_frame = 1;
			setpoint_raw.position.x =init_position_x_take_off;
			setpoint_raw.position.y =init_position_y_take_off;
			setpoint_raw.position.z =init_position_z_take_off - 1;		
		}	
    	mavros_setpoint_pos_pub.publish(setpoint_raw);
        ros::spinOnce();
        rate.sleep();
    }
	return 0;
}









