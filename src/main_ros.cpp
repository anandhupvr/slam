#include "inputs/ros/DepthSubscriber.h"
#include "inputs/ros/RGBSubscriber.h"

#include "Camera.h"
#include "cuda/vertex_ops.cuh"
#include "cuda/containers/device_array.hpp"
#include "cuda/cudafuncs.cuh"
#include "RGBDOdometry.h"
#include "GPUTexture.h"

using namespace GSLAM;


int main(int argc, char **argv)
{

	ros::init(argc, argv, "test_node");
	ros::NodeHandle nh;
	DepthSubscriber* depthsub;
	RGBSubscriber* rgbsub;

	pangolin::CreateWindowAndBind("Main", 320, 240, pangolin::Params({{"scheme", "headless"}}));
  	std::map<std::string, GPUTexture*> textures;
  	textures[GPUTexture::RGB] = new GPUTexture(320, 240, GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE, true, true);

	RGBDOdometry odom(320,240,277,277,160,120);

	depthsub  = new DepthSubscriber("/X1/front/optical/depth", nh);
	rgbsub = new RGBSubscriber("/X1/front/image_raw", nh);

	cv::Mat img;
	GSLAM::CameraPinhole cam(320,240,277,277,160,120);
	std::vector<DeviceArray2D<unsigned char>> a; 

	cv::Mat s_img;
	while (ros::ok())
	{
		// img = depthsub->read();
		img = rgbsub->read();

		if (img.empty()) 
		{
			ros::spinOnce();
			continue;
		}
		textures[GPUTexture::RGB]->texture->Upload(img.data, GL_RGB, GL_UNSIGNED_BYTE);
		textures[GPUTexture::RGB]->cudaMap();
  		cudaArray* textPtr = textures[GPUTexture::RGB]->getCudaArray();
  		
		ros::spinOnce();

	}
}