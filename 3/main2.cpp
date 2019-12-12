#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "ipp.h"
#include "x86intrin.h"
#include <ctime>
#include "immintrin.h"

using namespace cv;
using namespace std;

int main (void)
{
	IplImage *in_image;
	IplImage *image_smooth, *image_smooth_simd;
	IplImage *image_dilate, *image_dilate_simd;
	IplImage *image_edge, *image_edge_simd;

	unsigned char *in_img;
	unsigned char *img_smooth, *img_smooth_simd;
	unsigned char *img_dilate, *img_dilate_simd;
	unsigned char *img_edge, *img_edge_simd;


	Ipp64u start, end;
	Ipp64u time1, time2;

	in_image = cvLoadImage("./lena.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file "image.jpg".

	if(! in_image )  // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	image_smooth = cvCreateImage (cvGetSize(in_image), IPL_DEPTH_8U, 1);

	in_img = (unsigned char *) in_image->imageData;
	img_smooth = (unsigned char *) image_smooth->imageData;

	const int offset[] = {-1, +1, 0, -512-1, -512, -512+1, +512-1, +512, +512+1};



	printf ("=== Picture Smoothing ==========================================\n");
	start = std::clock();
	// Convert the input image to BW
	int sum = 0;
	for (int row = 1; row < 512-1; row++)
		for (int col = 1; col < 512-1; col++){
			sum = 0;
			for (int window = 0; window < 9; window++){
				sum += *(in_img + row * 512 + col + offset[window]);
			}
			*(img_smooth + row * 512 + col) = (sum/9);
		}
			
	end   = std::clock();
	time1 = end - start;
	printf ("Serial Run time = %u\n", (Ipp32s) time1);

	//DISPLAY image
	namedWindow( "input", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "input", in_image ); 			// Show our image inside it.
	namedWindow( "output", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "output", image_smooth ); 			// Show our image inside it.

	waitKey(0); 

	start = std::clock();

	__m128i *pSrc;
	__m128i *pRes;

	pSrc = (__m128i *) in_image->imageData;
	pRes = (__m128i *) image_smooth->imageData;
	
	__m128i row0x, row1x, row2x, temp;
	__m128i nine = _mm_set1_epi8((char)9);

	row0x = _mm_loadu_si128(pSrc + 510 * 512 + 510 + offset[0]);

	for (int row=1; row<512-1; row++){
		for (int col=1; col<512-1; col+=8){
			// First Row
			// row0x = _mm_loadu_si128((pSrc + row * 512 + col + offset[0]));
			// temp = _mm_loadu_si128((pSrc + row * 512 + col + offset[1]));
			// row0x = _mm_add_epi16(row0x, temp);

			// temp = _mm_loadu_si128((pSrc + row * 512 + col + offset[2]));
			// row0x = _mm_add_epi16(row1x, temp);
			// //Second Row
			// row1x = _mm_loadu_si128((pSrc + (row-1) * 512 + col + offset[0]));
			// temp = _mm_loadu_si128((pSrc + (row-1) * 512 + col + offset[1]));
			// row1x = _mm_add_epi16(row1x, temp);

			// temp = _mm_loadu_si128((pSrc + (row-1) * 512 + col + offset[2]));
			// row1x = _mm_add_epi16(row1x, temp);
			// //Third Row
			// row2x = _mm_loadu_si128((pSrc + (row+1) * 512 + col + offset[0]));
			// temp = _mm_loadu_si128((pSrc + (row+1) * 512 + col + offset[1]));
			// row2x = _mm_add_epi16(row2x, temp);

			// temp = _mm_loadu_si128((pSrc + (row+1) * 512 + col + offset[2]));
			// row2x = _mm_add_epi16(row2x, temp);
			// //Result
			// temp = _mm_add_epi16(row0x, row1x);
			// temp = _mm_add_epi16(temp, row2x);

			// // temp = _mm_div_epi8(temp, nine);

			// _mm_storeu_si128 ((pRes + row * 512 + col), temp);		

		}
	}

	end   = std::clock();
	time2 = end - start;
	printf ("Parallel Run time = %u\n", (Ipp32s) time2);
	printf ("Speedup = %4.2f\n", (float) (time1)/(float) time2);

	//DISPLAY image
	namedWindow( "input", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "input", in_image ); 			// Show our image inside it.
	namedWindow( "output", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "output", image_smooth ); 			// Show our image inside it.

	waitKey(0); 

	return 0;
}