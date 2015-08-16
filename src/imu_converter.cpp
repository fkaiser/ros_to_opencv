#include <ros/ros.h>
#include "sensor_msgs/Imu.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <fstream>
#include <string.h>


//class imu_subscriber{
//public:
//	imu_subscriber(){
//		imu_file_.open("IMU_data.txt");
//		//n_.subscribe("/mavros/imu/data",1000,&imu_subscriber::imusaver,this);
//
//	}
//
//	void imusaver(const sensor_msgs::Imu& msg){
//		//imu_file_<<"test";
//		ROS_INFO("Heard something");
//	}
//	~imu_subscriber(){
//		imu_file_.close();
//	}
//private:
//	ros::NodeHandle n_;
//	ros::Subscriber sub_;
//	std::ofstream imu_file_;
//};


void imusaver(const sensor_msgs::ImuPtr& msg){
	//imu_file_<<"test";
	ROS_INFO("Heard something");
}

//void imageCb(const sensor_msgs::ImageConstPtr& msg)
//  {
//	ROS_INFO("Hello world");
//  }


int main(int argc, char **argv)
{
	ros::init(argc, argv, "imu_listener");
	ros::NodeHandle n;
	n.subscribe("/mavros/imu/data",1,&imusaver);

	//imu_subscriber imu_collector;
	//imu_subscriber imu_collector2;

	ros::Rate r(10);
	while(ros::ok())
	{
		ros::spinOnce();

		ROS_INFO("I am here");
		r.sleep();
	}
	return 0;

}
