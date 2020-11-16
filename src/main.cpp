#include <pangolin/pangolin.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

using namespace std;

int main(int argc, char const *argv[])
{	

	int _ViewpointF = 420;


	pangolin::CreateWindowAndBind("Plane_Slam: Map Viewer",1024,768);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,0.2);
	pangolin::Var<bool> menu("menu.test",true,true);
	pangolin::OpenGlRenderState s_cam(
	pangolin::ProjectionMatrix(1024,768,_ViewpointF,_ViewpointF,512,389,0.1,1000),
	pangolin::ModelViewLookAt(-2,2,-2, 0,0,0,0.0,-1.0, 0.0)
	);

	pangolin::View& d_cam = pangolin::CreateDisplay()
			.SetBounds(0.0, 1.0, 0.2, 1.0, -1024.0f/768.0f)
			.SetHandler(new pangolin::Handler3D(s_cam));

	// pangolin::View& rgb_image = pangolin::Display("rgb")
	//   .SetBounds(0, 0.2, 0.2, 0.4, 1024.0f/768.0f)
	//   .SetLock(pangolin::LockLeft, pangolin::LockBottom);



	pangolin::GlTexture* texture;
	const GLenum internalFormat = GL_RGB;
	const GLenum format = GL_BGR;
	const GLenum dataType = GL_UNSIGNED_BYTE;
	const bool draw = true;



	cv::Mat img = cv::imread("/home/developer/slam/src/muscle.jpg");
	cv::resize(img, img, cv::Size(640, 480));

	cout << img.size() << endl;

	int width = img.cols;
	int height = img.rows;

	pangolin::GlFramebuffer frameBuffer;
	texture = new pangolin::GlTexture(width, height, internalFormat, draw, 0, format, dataType);

	while(!pangolin::ShouldQuit())
	{	

		cout << texture->tid << endl;
		texture->Upload(img.data,  GL_BGR, GL_UNSIGNED_BYTE);




		d_cam.Activate();
		glColor3f(1.0,1.0,1.0);
		texture->RenderToViewportFlipY();
		pangolin::FinishFrame();
	}


	std::cout<<"yees";
	return 0;
}