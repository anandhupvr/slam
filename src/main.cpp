#include <pangolin/pangolin.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

using namespace std;

int main(int argc, char const *argv[])
{	
	pangolin::GlTexture texture;
	const GLenum internalFormat = GL_RGB;
	const GLenum format = GL_BGR;
	const GLenum dataType = GL_UNSIGNED_BYTE;
	const bool draw = true;



	cv::Mat img = cv::imread("muscle.jpg");
	int width = img.cols;
	int height = img.rows;


	texture = pangolin::GlTexture(width, height, internalFormat, draw, 0, format, dataType);
	texture.Upload(img.data,  GL_BGR, GL_UNSIGNED_BYTE);

	// texture(new pangolin::GlTexture(width, height, internalFormat, draw, 0, format, dataType));

	std::cout<<"yees";
	return 0;
}