#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include "sensor_msgs/Imu.h"
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <std_srvs/Trigger.h>
#include <ros/callback_queue.h>

using namespace std;

static const std::string OPENCV_WINDOW = "Image window";
int timetmp=0;

ros::Time capture_time;
sensor_msgs::Image testimage;

class ImageConverter
{
  ros::NodeHandle nh_;
  ros::NodeHandle ch_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::ServiceServer server_trigger_;
  ros::CallbackQueue trigger_queue_;

public:

  bool servtrig(std_srvs::Trigger::Request &req,std_srvs::Trigger::Response &resp){
  		resp.success=true;
  		ros::getGlobalCallbackQueue()->callAvailable(ros::WallDuration(0.0));
  		ROS_INFO_STREAM("Saved images");
  		return true;

  	}


  void advertisetriggerservice(string  camname)
  {
	  server_trigger_=ch_.advertiseService(camname+"/trigger_service", &ImageConverter::servtrig,this);
  }
  void EmtpyQueue()
  {
	  trigger_queue_.callOne();
  }


  ImageConverter()
    : it_(nh_),
      ch_(nh_, "trigger_node")
  {
	  ch_.setCallbackQueue(&trigger_queue_);
    // Subscribe to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW);
    advertisetriggerservice("no_name");
  }

  ImageConverter(string topic_name)
      : it_(nh_),
        ch_(nh_, "trigger_node")
    {
	  ch_.setCallbackQueue(&trigger_queue_);

      // Subscribe to input video feed and publish output video feed
      image_sub_ = it_.subscribe(topic_name, 1,
        &ImageConverter::imageCb, this);
      image_pub_ = it_.advertise("/image_converter/output_video", 1);

      cv::namedWindow(OPENCV_WINDOW);
      advertisetriggerservice(topic_name);
    }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
	  cv::Mat A;
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    //Save image

    A= cv_ptr->image;
    string total_name;
    string direct;
    string format;
    string sec;
    string nsec;
    ostringstream convert;

    // Extract time stamp

    if (cv_ptr->header.stamp.sec!=0){
    convert << cv_ptr->header.stamp.sec;
    sec = convert.str();
    convert << cv_ptr->header.stamp.nsec;
    nsec = convert.str();
    }
    else{
    	timetmp++;
    	convert<<timetmp;
    	 sec = convert.str();
    	nsec="noROStimestamp";
    }

    // Get directory for saving and format from ros parameter server
    ros::param::get("~direct", direct);
    ros::param::get("~format", format);

    total_name=direct + sec + '_' + nsec+format;
    capture_time=ros::Time::now();
    testimage.header.stamp.sec=capture_time.sec;
    testimage.header.stamp.nsec=capture_time.nsec;

    imwrite(total_name, A);
    cout<< "Saved images: " << total_name << " at time instance:"<<testimage.header.stamp.sec<<"."<< testimage.header.stamp.nsec<<endl;

  }
};



int main(int argc, char** argv)
{

  ros::init(argc, argv, "image_converter");

// Subscribe to desired image defined in launch file
  string topic_name;
  ros::param::get("~identifier",topic_name);
  ImageConverter ic(topic_name);
  cout<<topic_name<<endl;

  // Define time update rate to call callback function if necessary
    ros::Rate r(50); //   Number in Hz
    while( ros::ok())
    {
    ic.EmtpyQueue();
    r.sleep();

    }
	return 0;

}

