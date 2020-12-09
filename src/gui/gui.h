#include <iostream>
#include <pangolin/pangolin.h>
#include <limits>
// #include "../core/shaders/Shaders.h"
// #include "../core/inputs/ros/DepthSubscriber.h"
// #include "../core/inputs/ros/RGBSubscriber.h"
#include "../core/Camera.h"
// #include "../core/GPUTexture.h"
#include "utils/Parse.h"
#include "../core/inputs/ros/DepthSubscriber.h"
#include "../core/inputs/ros/RGBSubscriber.h"
/*
XXX
modulize inputs
fix relative paths
*/

using namespace GSLAM;

class GUIController
{
	public:
		GUIController(int width, int height)
		{
			pangolin::Params windowParams;
			pangolin::CreateWindowAndBind("Main", width, height, windowParams);
			pangolin::OpenGlRenderState s_cam(
				pangolin::ProjectionMatrix(320,240,277,277,160,120,0.1,1000),
				pangolin::ModelViewLookAt(-1,1,-1, 0,0,0, pangolin::AxisY)
				);	
			pangolin::View& d_cam = pangolin::Display("cam")
			.SetBounds(0,1.0f,0,1.0f,-320/240.0)
			.SetHandler(new pangolin::Handler3D(s_cam)
			);
			glEnable(GL_DEPTH_TEST	);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_ALWAYS);
		}
		void start(int argc, char* argv[])
		{
			bool ros_enable;
			Parse::get().arg(argc, argv, "-ros", ros_enable);
			if (ros_enable)
			{
				ros::init(argc, argv, "slam_rgbd_input_node");
				ros::NodeHandle nh;

			}
		}
	private:
		DepthSubscriber* depthsub;
		RGBSubscriber* rgbsub;
		
};



