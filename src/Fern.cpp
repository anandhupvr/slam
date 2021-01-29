#include "Fern.h"

Ferns::Ferns(int n, int maxDepth, const float photoThresh)
    : num(n),
      factor(8),
      width(Resolution::getInstance().width() / factor),
      height(Resolution::getInstance().height() / factor),
      maxDepth(maxDepth),
      photoThresh(photoThresh),
      widthDist(0, width - 1),
      heightDist(0, height - 1),
      rgbDist(0, 255),
      dDist(400, maxDepth),
      imageBuff(width, height){
  random.seed(time(0));
  // generateFerns();
}
Ferns::~Ferns() {
  for (size_t i = 0; i < frames.size(); i++) {
    delete frames.at(i);
  }
}
bool Ferns::addFrame(DeviceArray<float>& imageTexture, const Eigen::Matrix4f& pose, int srcTime,
				const float threshold){

	// std::cout << imageTexture.size() << " device array size " << std::endl;
	Img<Eigen::Matrix<unsigned char, 3, 1>> img(height, width);

	DeviceArray<float> dst;
	dst.create(height*width*3);
  float dwd(height*width*3);
  // std::cout << dst[0] << " --- before " << std::endl;
	resizeRGB(height, width, imageTexture, dst);

  dst.download(&dwd);
  std::cout << dwd << " --- after " << std::endl;

	// std::cout << img.cols << " = cols,  " <<  img.rows << "  = rows " << std::endl;
	// std::cout << dst.size() << " resized size " << std::endl;
	// Frame* frame = new Frame(num, frames.size(), pose, srcTime, width * height,
	// 							(unsigned char*)img.data);

	// int* coOccurrences = new int[frames.size()];
	// memset(coOccurrences, 0, sizeof(int) * frames.size());

	return true;

}