 // 2D float texture
#include "cuda/cuda_array_test.cuh"

texture<float, cudaTextureType2D, cudaReadModeElementType> texRef;


// Simple transformation kernel
__global__ void transformKernel(float* output, int width, int height, float theta)
{
      // Calculate normalized texture coordinates
      unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
     
      unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
     
      float u = x / (float)width;
     
      float v = y / (float)height;
     
      // Transform coordinates
      u -= 0.5f;
     
      v -= 0.5f; 
     
      float tu = u * cosf(theta) - v * sinf(theta) + 0.5f ;
     
      float tv = v * cosf(theta) + u * sinf(theta) + 0.5f ;
     
      // Read from texture and write to global memory
      output[y * width + x] = tex2D(texRef, tu, tv);
}


void transform(cudaChannelFormatDesc channelDesc,  cudaArray* cuArray)
{    


      int width = 3 , height = 3 ;

      float h_data[3][3] ;
      for ( int i =0 ; i<3 ; i++ )
            for ( int j = 0 ; j<3; j++ )
                        h_data [i][j] = i*j ;

      int size = width*height*sizeof(float) ;

     
      cudaMallocArray(&cuArray, &channelDesc, width, height);
     
      // Copy to device memory some data located at address h_data
      // in host memory
      cudaMemcpyToArray(cuArray, 0, 0, h_data, size, cudaMemcpyHostToDevice);
     
      // Set texture parameters
      texRef.addressMode[0] = cudaAddressModeWrap;
      texRef.addressMode[1] = cudaAddressModeWrap;
      texRef.filterMode = cudaFilterModeLinear;
      texRef.normalized = true;
     
      // Bind the array to the texture reference
      cudaBindTextureToArray(texRef, cuArray, channelDesc);
     
      // Allocate result of transformation in device memory
      float* output;
     
      cudaMalloc(&output, size );
     
      // Invoke kernel
      dim3 dimBlock(16, 16);
      dim3 dimGrid( (width + dimBlock.x - 1) / dimBlock.x,
                       (height + dimBlock.y - 1) / dimBlock.y );
     
      transformKernel<<<dimGrid, dimBlock>>>(output, width, height, 90 );
     
      printf ("\n Original array \n");
      for ( int i =0 ; i<3 ; i++ )
      {
            for ( int j = 0 ; j<3; j++ )
                              printf ("%f ", h_data [i][j] );
           
            printf ("\n");
      }

      cudaMemcpy (h_data , output, size, cudaMemcpyDeviceToHost );

      printf ("\n After operation \n ");
      for ( int i =0 ; i<3 ; i++ )
      {
            for ( int j = 0 ; j<3; j++ )
                              printf ("%f ", h_data [i][j] );
           
            printf ("\n");
      }

      // Free device memory
      cudaFreeArray(cuArray);
      cudaFree(output);
}