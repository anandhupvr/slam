#include "inputs/ros/DepthSubscriber.h"
#include "inputs/ros/RGBSubscriber.h"
#include "Display.h"
// #include "GPUTexture.h"
#include <limits>
#include <iostream>
#include <pangolin/pangolin.h>
#include "shaders/Shaders.h"



int main(int argc, char* argv[])
{
  // Create OpenGL window in single line
  // pangolin::CreateWindowAndBind("Main",640,480);

	ros::init(argc, argv, "test_node");
	ros::NodeHandle nh;
	DepthSubscriber* depthsub;
	RGBSubscriber* rgbsub;
	depthsub  = new DepthSubscriber("/X1/front/depth", nh);
	rgbsub = new RGBSubscriber("/X1/front/image_raw", nh);

	cv::Mat img;
	cv::Mat dimg;

	const int width =  320;
	const int height = 240;
	Display display(width, height);

  // 3D Mouse handler requires depth testing to be enabled
  // glEnable(GL_DEPTH_TEST);
  pangolin::OpenGlRenderState s_cam(
    pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
    pangolin::ModelViewLookAt(-1,1,-1, 0,0,0, pangolin::AxisY)
  );

  // Aspect ratio allows us to constrain width and height whilst fitting within specified
  // bounds. A positive aspect ratio makes a view 'shrink to fit' (introducing empty bars),
  // whilst a negative ratio makes the view 'grow to fit' (cropping the view).
  pangolin::View& d_cam = pangolin::Display("cam")
      .SetBounds(0,1.0f,0,1.0f,-640/480.0)
      .SetHandler(new pangolin::Handler3D(s_cam));

  // This view will take up no more than a third of the windows width or height, and it
  // will have a fixed aspect ratio to match the image that it will display. When fitting
  // within the specified bounds, push to the top-left (as specified by SetLock).
  pangolin::View& d_image = pangolin::Display("image")
      .SetBounds(2/3.0f,1.0f,0,1/3.0f,640.0/480)
      .SetLock(pangolin::LockLeft, pangolin::LockTop);


  pangolin::GlTexture imageTexture(width,height,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);


	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	};
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
	std::shared_ptr<Shader> test_program;

    test_program = std::shared_ptr<Shader>(loadProgramFromFile("test.vert", "test.frag"));

  	int posAttr;

  while(ros::ok())
  {
		test_program->Bind();
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// posAttr = glGetAttribLocation(test_program->programId(), "position");
		// glEnableVertexAttribArray(posAttr);
		// glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 6);

		
		img = rgbsub->read();
		dimg = depthsub->read();

			if ((img.empty()) || (dimg.empty())) 
			{
				ros::spinOnce();
				continue;
			}
		d_cam.Activate(s_cam);
       	imageTexture.Upload(img.data,GL_RGB,GL_UNSIGNED_BYTE);
    	imageTexture.RenderToViewport();
		test_program->Unbind();
    	
 //    imageTexture.Upload(img.data,GL_RGB,GL_UNSIGNED_BYTE);
 //    display.draw();
 //    //display the image
 //    d_image.Activate();
 //    glColor3f(1.0,1.0,1.0);
    pangolin::FinishFrame();
  }


  return 0;
}
