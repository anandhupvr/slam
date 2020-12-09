#include "vertex_ops.cuh"



__global__
void unproject_kernel(unsigned char *depth, double* d_3d_points, int rows, int cols, double cx, double cy, double fx, double fy, double fx_inv, double fy_inv)
{

	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	int idy = threadIdx.y + (blockIdx.y * blockDim.y);
	int id;
	if ((idx < rows ) && (idy < cols))
	{
		// add depth info
		id = idx * idy;
		double z = (double)depth[id];
		d_3d_points[id] = (idx-cx)*fx_inv*z;
		d_3d_points[id+(rows*cols)] = (idy-cy)*fy_inv*z;
		d_3d_points[id+2*(rows*cols)] = z;
		// printf("--%f--\n",(cx));
	}
}


void unproject(cv::Mat img, GSLAM::CameraPinhole cam)
{

	// uchar4 **ddepth, **input_image;
	// unsigned char *hdepth;
	unsigned char *d_depth_image;
	double *d_3d_points, *h_3d_points;
	int rows, cols;
	rows = img.rows;
	cols = img.cols;
	size_t totalpixels = rows*cols;
	const dim3 dimGrid((int)ceil((cols)/16), (int)ceil((rows)/16));
	const dim3 dimBlock(16, 16);
	// *input_image = (uchar4 *)img.ptr<uchar4 *>(0);
	// cudaMalloc(ddepth, sizeof(uchar4) * totalpixels * CHANNELS);
	// cudaMemcpy(*ddepth, *input_image, sizeof(uchar4) * totalpixels * CHANNELS, cudaMemcpyHostToDevice);

	// h_3d_points = (double*)malloc(sizeof(double) * totalpixels * 3);
	// unsigned char* depth_image = (unsigned char*)img.data;
	// cudaMalloc((void **)&d_depth_image, sizeof(unsigned char) * totalpixels );
	// cudaMalloc((void **)&d_3d_points, sizeof(double) * totalpixels * 3 );
	// cudaMemcpy(d_depth_image, depth_image, sizeof(unsigned char) * totalpixels , cudaMemcpyHostToDevice);
	// unproject_kernel<<<dimGrid,dimBlock>>>(d_depth_image, d_3d_points, rows, cols, cam.cx, cam.cy, cam.fx, cam.fy, cam.fx_inv, cam.fy_inv);
	// cudaMemcpy(h_3d_points, d_3d_points, sizeof(double) * totalpixels * 3, cudaMemcpyDeviceToHost);
	
	// std::cout<<*(h_3d_points+sizeof(double)+(rows*cols));
	// cudaFree(d_depth_image);

      
}

// texture<uchar4,cudaTextureType2D,cudaReadModeNormalizedFloat> tex;



// __global__
// void test(char *img,int width,int heigth,int channels)
// {

//     unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
//     unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;


//     float4 pixel = tex2D(tex, x, y);

// 	img[(y*width+x)*channels+0] = pixel.x  * 255;
// 	img[(y*width+x)*channels+1] = pixel.y  * 255;
// 	img[(y*width+x)*channels+2] = pixel.z  * 255;
// 	img[(y*width+x)*channels+3] = 0;



// }

texture<uchar4, 2, cudaReadModeElementType> inTex;

__global__ void bgr2IntensityKernel(PtrStepSz<unsigned char> dst)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= dst.cols || y >= dst.rows)
        return;

    uchar4 src = tex2D(inTex, x, y);

    int value = (float)src.x * 0.114f + (float)src.y * 0.299f + (float)src.z * 0.587f;

    printf("%d\n", value);

    dst.ptr (y)[x] = value;
}

void imageBGRToIntensity(cudaArray * cuArr, DeviceArray2D<unsigned char> & dst)
{
    dim3 block (32, 8);
    dim3 grid (getGridDim (dst.cols (), block.x), getGridDim (dst.rows (), block.y));

    cudaSafeCall(cudaBindTextureToArray(inTex, cuArr));

    bgr2IntensityKernel<<<grid, block>>>(dst);

    cudaCheckError();

    cudaSafeCall(cudaUnbindTexture(inTex));
}

void rgb_texture_test(cv::Mat img,	cudaArray *cuArray, DeviceArray2D<unsigned char>& dst)
{
	int rows=img.rows;
	int cols=img.cols;
	int channels=img.channels();
	int width=cols,height=rows,size=rows*cols*channels;

	cudaChannelFormatDesc channelDesc=cudaCreateChannelDesc<uchar4>();
	cudaMallocArray(&cuArray,&channelDesc,width,height);
	cudaMemcpyToArray(cuArray,0,0,img.data,size,cudaMemcpyHostToDevice);

	tex.addressMode[0]=cudaAddressModeWrap; 
	tex.addressMode[1]=cudaAddressModeWrap;
	tex.filterMode = cudaFilterModeLinear;  
	tex.normalized =false;          //No normalization


	imageBGRToIntensity(cuArray, dst);

	// cudaBindTextureToArray(tex,cuArray,channelDesc);


	// cv::Mat out=cv::Mat::zeros(height, width, CV_8UC4);
	// char *dev_out=NULL;
	// cudaMalloc((void**)&dev_out, size);

	// dim3 dimBlock(16, 16);
	// dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);

	// test <<<dimGrid,dimBlock,0>>>(dev_out,width,height,channels);



 //    cudaMemcpy(out.data,dev_out,size,cudaMemcpyDeviceToHost);




    // cv::imwrite("src/MyImage.jpg", out);
    // cv::imshow("orignal",img);
    // cv::imshow("smooth_image",out);
    // cv::waitKey(0);
    // printf("saving\n");
    // cudaFree(dev_out);
    cudaFreeArray(cuArray);
    cudaUnbindTexture(tex);


}

// void _cudaFree(cudaArray* cuArray)
// {
//     cudaSafeCall(cudaFreeArray(cuArray));
// }