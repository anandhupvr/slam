#include <pangolin/pangolin.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <shaders.h>

using namespace std;

int main(int argc, char const *argv[])
{	

	// int _ViewpointF = 420;


	// pangolin::CreateWindowAndBind("Plane_Slam: Map Viewer",1024,768);
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,0.2);
	// pangolin::Var<bool> menu("menu.test",true,true);
	// pangolin::OpenGlRenderState s_cam(
	// pangolin::ProjectionMatrix(1024,768,_ViewpointF,_ViewpointF,512,389,0.1,1000),
	// pangolin::ModelViewLookAt(-2,2,-2, 0,0,0,0.0,-1.0, 0.0)
	// );


	// pangolin::View& d_cam = pangolin::CreateDisplay()
	// 		.SetBounds(0.0, 1.0, 0.2, 1.0, -1024.0f/768.0f)
	// 		.SetHandler(new pangolin::Handler3D(s_cam));

	// pangolin::View& rgb_image = pangolin::Display("rgb")
	//   .SetBounds(0, 0.2, 0.2, 0.4, 1024.0f/768.0f)
	//   .SetLock(pangolin::LockLeft, pangolin::LockBottom);


	pangolin::CreateWindowAndBind("Main",640,480);
			 // start depth test
	glEnable(GL_DEPTH_TEST);
		 // define initial model view matrix and the projection 
	pangolin::OpenGlRenderState s_cam(
			pangolin::ProjectionMatrix(640, 480, 420, 420, 320, 240, 0.2, 100),
						 // corresponds gluLookAt, the camera position, the position of the reference point, up vector (the vector)
			pangolin::ModelViewLookAt(0, -10, 0.1, 0, 0, 0, pangolin::AxisNegY));
		 // create an interactive view in the window
	pangolin::Handler3D handler(s_cam);
	pangolin::View &d_cam = pangolin::CreateDisplay()
							.SetBounds(0.0, 1.0,0.0,1.0,-640.0f/480.0f);




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


	texture = new pangolin::GlTexture(width, height, internalFormat, draw, 0, format, dataType);

	std::shared_ptr<Shader> program;

	program =  std::shared_ptr<Shader>(loadProgramFromFile("empty.vert", "int_to_color.frag", "quad.geom"));

	pangolin::GlRenderBuffer* renderBuffer;
	pangolin::GlFramebuffer frameBuffer;


	renderBuffer = new pangolin::GlRenderBuffer(640, 480);
	frameBuffer.AttachColour(*texture);
	frameBuffer.AttachDepth(*renderBuffer);



	cout << texture->tid << endl;
	// texture->Upload(img.data,  GL_BGR, GL_UNSIGNED_BYTE);

	// texture.Bind();
	// frameBuffer.Bind();
	// glPushAttrib(GL_VIEWPORT_BIT);


	// glViewport(0, 0, renderBuffer.width, renderBuffer.height);

	// glClearColor(0, 0, 0, 0);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// program->Bind();


	while(!pangolin::ShouldQuit())
	{	

		cout << texture->tid << endl;
		texture->Upload(img.data,  GL_BGR, GL_UNSIGNED_BYTE);


		texture.Bind();
		frameBuffer.Bind();
		glPushAttrib(GL_VIEWPORT_BIT);


		glViewport(0, 0, renderBuffer.width, renderBuffer.height);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program->Bind();


		d_cam.Activate();
		glColor3f(1.0,1.0,1.0);
		texture->RenderToViewportFlipY();
		pangolin::FinishFrame();
	}


	std::cout<<"yees";
	return 0;
}