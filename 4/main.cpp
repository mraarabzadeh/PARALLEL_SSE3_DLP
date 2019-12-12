#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "ipp.h"
#include "x86intrin.h"
#include <ctime>

using namespace cv;
using namespace std;

int main( ){

	IplImage *in_img;
	IplImage *in_img2;
	IplImage *out_img;
	IplImage *out_img2;
	unsigned char *in_image;
	unsigned char *in_image2;
	unsigned char *out_image;


	Ipp64u start, end;
	Ipp64u time1, time2;

	// LOAD image
	in_img = cvLoadImage("./lena.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file "image.jpg".
	in_img2 = cvLoadImage("./lena2.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file "image.jpg".
	out_img = cvCreateImage (cvGetSize(in_img), IPL_DEPTH_8U, 1);
	out_img2 = cvCreateImage (cvGetSize(in_img), IPL_DEPTH_8U, 1);


	if(! in_img )  // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	if(! in_img2 )  // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	in_image  = (unsigned char *) in_img->imageData;
	in_image2  = (unsigned char *) in_img2->imageData;
	out_image = (unsigned char *) out_img->imageData;

	float diff = 0;

	start = clock();

	for (int row = 0; row < 512; row++)
		for (int col = 0; col < 512; col++){
			diff = *(in_image + row * 512 + col)-*(in_image2 + row * 512 + col);
			if(diff >= 0){
				*(out_image + row * 512 + col) = diff;
			}
			else{
				*(out_image + row * 512 + col) = -diff;
			}
		}

	end   = clock();
	time1 = end - start;



	__m128i *pSrc;
	__m128i *pSrc2;
	__m128i *pRes;
	__m128i m1, m2, m3, m4, m5, m6, m7;
	__m128i sign_bit;

	pSrc = (__m128i *) in_img->imageData;
	pSrc2 = (__m128i *) in_img2->imageData;
	pRes = (__m128i *) out_img2->imageData;

	start = clock();

	sign_bit = _mm_set1_epi8(-128);

	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512 / 16; j++){
			m1 = _mm_loadu_si128(pSrc + i * 512/16 + j);
			m2 = _mm_loadu_si128(pSrc2 + i * 512/16 + j);

			m3 = _mm_cmpgt_epi8(_mm_xor_si128(m1, sign_bit), _mm_xor_si128(m2, sign_bit));
			m4 = _mm_add_epi8(_mm_and_si128(m3, m1), _mm_andnot_si128(m3, m2));
			m5 = _mm_add_epi8(_mm_and_si128(m3, m2), _mm_andnot_si128(m3, m1));
			m5 = _mm_sub_epi8(m4, m5);
			
			_mm_storeu_si128 (pRes + i * 512/16 + j, m5);
		}

	end   = clock();
	time2 = end - start;

	//DISPLAY image
	namedWindow( "input", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "input", in_img ); 			// Show our image inside it.
	namedWindow( "output", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "output", out_img ); 			// Show our image inside it.
	namedWindow( "output2", CV_WINDOW_AUTOSIZE ); // Create a window for display.
	cvShowImage( "output2", out_img2 ); 			// Show our image inside it.

	waitKey(0);                       	// Wait for a keystroke in the window

	printf ("Serial Run time = %d \n", (Ipp32s) time1);
	printf ("Parallel Run time = %d \n", (Ipp32s) time2);
	printf ("Speedup = %4.2f\n", (float) (time1)/(float) time2);

	return 0;
}