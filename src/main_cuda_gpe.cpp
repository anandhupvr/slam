#include <cuda_runtime_api.h>
#include "inputs/ros/DepthSubscriber.h"
#include "inputs/ros/RGBSubscriber.h"
#include "Camera.h"
#include <iostream>
#include "cuda/cudafuncs.cuh"
#include "cuda/containers/device_array.hpp"
#include "RGBDOdometry.h"

using namespace std;


int main(int argc, char  *argv[])
{
	int width, height, rows, cols;
	width = cols = 320;
	height = rows = 240;
	Eigen::Matrix4f pose = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f tinv;

	const int TEXTURE_DIMENSION = 3072;
	const int MAX_VERTICES = TEXTURE_DIMENSION * TEXTURE_DIMENSION;
	const int NODE_TEXTURE_DIMENSION = 16384;
	const int MAX_NODES = NODE_TEXTURE_DIMENSION / 16;  // 16 floats per node
	int VSIZE = sizeof(Eigen::Vector4f) ;
	const int bufferSize = MAX_VERTICES * VSIZE * 3;
	// int d2d_w, d2d_h;
	// d2d_w = sqrt(MAX_VERTICES);
	// d2d_h = d2d_w;


	Eigen::Vector3f transObject = pose.topRightCorner(3, 1);
	Eigen::Matrix<float, 3, 3, Eigen::RowMajor> rotObject = pose.topLeftCorner(3, 3);

	ros::init(argc, argv, "test_node");
	ros::NodeHandle nh;
	DepthSubscriber* depthsub;
	RGBSubscriber* rgbsub;
	cv::Mat dimg, img;
	RGBDOdometry* rgbd_odom;

	GSLAM::CameraPinhole cam_model(320,240,277,277,160,120);
	CameraModel intr;
	intr.cx = cam_model.cx;
	intr.cy = cam_model.cy;
	intr.fx = cam_model.fx;
	intr.fy = cam_model.fy;

	rgbd_odom = new RGBDOdometry(width, height, (float)cam_model.cx, (float)cam_model.cy, (float)cam_model.fx, (float)cam_model.fy);

	DeviceArray<float> rgb, rgb_prev, vmaps_tmp, nmaps_tmp;
	DeviceArray2D<float> depth;
	DeviceArray2D<float> vmap, nmap, vmap_splat_prev, nmap_splat_prev;
	DeviceArray<float> model_buffer;
	std::vector<DeviceArray2D<float>> depthPyr;

	depthPyr.resize(3);
	for (int i = 0; i < 3; ++i) 
	{
		int pyr_rows = height >> i;
		int pyr_cols = width >> i;
		depthPyr[i].create(pyr_rows, pyr_cols);
	}

	rgb.create(height*3*width);
	rgb_prev.create(height*3*width);
	depth.create(height, width);
	model_buffer.create(bufferSize);

	// initialize model buffers
	float* vertices = new float[bufferSize];
	memset(&vertices[0], 0, bufferSize);
	model_buffer.upload(&vertices[0], bufferSize);

	depthsub  = new DepthSubscriber("/X1/front/depth", nh);
	rgbsub = new RGBSubscriber("/X1/front/image_raw", nh);

	int i = 0;
	while (ros::ok())
	{
		img  = rgbsub->read();
		dimg = depthsub->read();
		if (dimg.empty() || img.empty())
		{
			ros::spinOnce();
			continue;	
		}
		img.convertTo(img, CV_32FC3);
		dimg.convertTo(img, CV_32FC1);
		rgb.upload((float*)img.data, height*3*width);
		depth.upload((float*)dimg.data, width*sizeof(float), height, width);

		if (i==0)
		{
			rgbd_odom->initFirstRGB(rgb);
			createVMap(intr, depth, vmap, 100);
			createNMap(vmap, nmap);
			rgb_prev.upload((float*)img.data, height*3*width);
			tinv  = pose.inverse();
			//debug start
			initModelBuffer(intr, vmap, nmap, rgb, model_buffer);

			//printing 
			float* data = new float[bufferSize];
			model_buffer.download(&data[0]);
			for (int i=0; i<int(100);i++) std::cout<<data[i]<<"*"<<data[i+rows*cols]<<"*"<<data[i+2*rows*cols]<<"*"<<data[i+3*rows*cols]<<"*"<<data[i+4*rows*cols]<<"*"<<data[i+5*rows*cols]<<"*"<<data[i+6*rows*cols]<<"*"<<data[i+7*rows*cols]<<"*"<<data[i+8*rows*cols]<<"*"<<data[i+9*rows*cols]<<"*"<<data[i+10*rows*cols]<<"*"<<data[i+11*rows*cols]<<"*";
			exit(0);
			//debug end

			splatDepthPredict(intr, height, width, tinv.data(), vmap, vmap_splat_prev, nmap, nmap_splat_prev);
			ros::spinOnce();
			i++;
			continue;
		}

		//generateCUDATextures
		copyDMaps(depth, depthPyr[0]);
		for (int i = 1; i < 3; ++i) 
			pyrDownGaussF(depthPyr[i - 1], depthPyr[i]);
		cudaDeviceSynchronize();
		cudaCheckError();
		tinv  = pose.inverse();

		// perfrom tracking
		rgbd_odom->initICPModel(vmap_splat_prev, nmap_splat_prev, 100, pose);
		copyMaps(vmap_splat_prev, nmap_splat_prev, vmaps_tmp, nmaps_tmp);
		rgbd_odom->initRGBModel(rgb_prev, vmaps_tmp);
		rgbd_odom->initICP(depthPyr, 100);
		rgbd_odom->initRGB(rgb, vmaps_tmp);
		transObject = pose.topRightCorner(3, 1);
		rotObject = pose.topLeftCorner(3, 3);
		rgbd_odom->getIncrementalTransformation(transObject, rotObject, false, 0.3, true, false, true, 0, 0);
		pose.topRightCorner(3, 1) = transObject;
		pose.topLeftCorner(3, 3) = rotObject;


		//splat predict
		createVMap(intr, depth, vmap, 100);
		createNMap(vmap, nmap);
		std::swap(rgb, rgb_prev);
		splatDepthPredict(intr, height, width, tinv.data(), vmap, vmap_splat_prev, nmap, nmap_splat_prev);
		

		//predict indicies
		std::cout<<"i :"<< i<< "\ntrans :"<<transObject<<std::endl<<"rot :"<<rotObject<<std::endl;
		// std::cout<<"i :"<< i<< "\npose :"<<pose<<std::endl;
		i++;
		ros::spinOnce();
	}

	return 0;
}