#ifndef FERNS_H_
#define FERNS_H_
#include <cuda_runtime_api.h>

#include <iostream>
#include <random>
#include <Eigen/Core>
#include <vector>
#include <limits>
#include "img.h"
#include "cuda/cudafuncs.cuh"
#include "cuda/containers/device_array.hpp"
#include "Resolution.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

class Ferns {
public:
	Ferns( int n, int maxDepth, const  float photoThresh);
	virtual ~Ferns();

	bool addFrame(DeviceArray<float>& imageTexture, const Eigen::Matrix4f& pose, int srcTime,
					const float threshold);

	class Fern {
	public:
	Fern() {}

		Eigen::Vector2i pos;
		Eigen::Vector4i rgbd;
		std::vector<int> ids[16];
	};

	std::vector<Fern> conservatory;


    class Frame {
    public:
        Frame(int n, int id , const Eigen::Matrix4f& pose, const int srcTime, const int numPixels, unsigned char* rgb = 0)
        : goodCodes(0), id(id), pose(pose), srcTime(srcTime), initRgb(rgb){
            codes = new unsigned char[n];

            if (rgb) {
                this->initRgb = new unsigned char[numPixels * 3];
                memcpy(this->initRgb, rgb, numPixels * 3);
            }
        }
    virtual ~Frame(){
        delete[] codes;

        if (initRgb) delete[] initRgb;
    }

    unsigned char* codes;
    int goodCodes;
    const int id;
    Eigen::Matrix4f pose;
    const int srcTime;
    unsigned char* initRgb;
    };

    std::vector<Frame*> frames;

    const int num;
    std::mt19937 random;
    const int factor;
    const int width;
    const int height;
    const int maxDepth;
    const float photoThresh;
    std::uniform_int_distribution<int32_t> widthDist;
    std::uniform_int_distribution<int32_t> heightDist;
    std::uniform_int_distribution<int32_t> rgbDist;
    std::uniform_int_distribution<int32_t> dDist;

    Img<Eigen::Matrix<unsigned char, 3, 1>> imageBuff;

};

#endif /* FERNS_H_ */
