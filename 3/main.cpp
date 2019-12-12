#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "ipp.h"
#include "x86intrin.h"
#include <ctime>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

int main( ){

	IplImage *in_img;
	IplImage *out_img;
	IplImage *out_img2;
	unsigned char *in_image;
	unsigned char *out_image;


	Ipp64u start, end;
	Ipp64u time1, time2;

	// LOAD image
	in_img = cvLoadImage("./lena.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file "image.jpg".
	out_img = cvCreateImage (cvGetSize(in_img), IPL_DEPTH_8U, 1);
	out_img2 = cvCreateImage (cvGetSize(in_img), IPL_DEPTH_8U, 1);


	if(! in_img )  // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	in_image  = (unsigned char *) in_img->imageData;
	out_image = (unsigned char *) out_img->imageData;


	const int offset[] = {-1, +1, 0, -512-1, -512, -512+1, +512-1, +512, +512+1};


	start = clock();

	int sum = 0;
	for (int row = 1; row < 512-1; row++)
		for (int col = 1; col < 512-1; col++){
			sum = 0;
			for (int window = 0; window < 9; window++){
				sum += *(in_image + row * 512 + col + offset[window]);
			}
			*(out_image + row * 512 + col) = (sum/9);
		}

	end   = clock();
	time1 = end - start;

	

	__m128i *pSrc;
	__m128i *pRes;
	__m128i m1, m4;
	__m128i n11, n12, n41, n42;

	char result[16];
	char result2[16];

	pSrc = (__m128i *) in_img->imageData;
	pRes = (__m128i *) out_img2->imageData;

	__m128i zero, two;

	start = clock();

	zero = _mm_set1_epi8(0x00);
	two = _mm_set1_epi8(2);
	for (int row=0; row<512; row++){
		for (int col=0; col<512/16; col+=1){
			// First Row
			m1 = _mm_loadu_si128((pSrc + row * 512/16 + col + 0));
			m4 = _mm_loadu_si128((pSrc + row * 512/16 + col - 1/16));

			n11 = _mm_unpacklo_epi8(m1, zero);
			n12 = _mm_unpackhi_epi8(m1, zero);
			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);

			m4 = _mm_loadu_si128((pSrc + row * 512/16 + col + 1/16));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);
			//Second Row
			m4 = _mm_loadu_si128((pSrc + (row-1) * 512/16 + col + 0));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);

			m4 = _mm_loadu_si128((pSrc + (row-1) * 512/16 + col - 1/16));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);

			m4 = _mm_loadu_si128((pSrc + (row-1) * 512/16 + col + 1/16));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);
			//Third Row
			m4 = _mm_loadu_si128((pSrc + (row+1) * 512/16 + col + 0));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);

			m4 = _mm_loadu_si128((pSrc + (row+1) * 512/16 + col - 1/16));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);

			m4 = _mm_loadu_si128((pSrc + (row+1) * 512/16 + col + 1/16));

			n41 = _mm_unpacklo_epi8(m4, zero);
			n42 = _mm_unpackhi_epi8(m4, zero);

			n11 = _mm_add_epi16(n11, n41);
			n12 = _mm_add_epi16(n12, n42);
			//Result
			n11 = _mm_srli_epi16(n11, 3);
			n12 = _mm_srli_epi16(n12, 3);

			n11 = _mm_sub_epi16(n11, two);
			n12 = _mm_sub_epi16(n12, two);

			n12 = _mm_slli_epi16(n12, 8);
			n11 = _mm_add_epi8(n11, n12);

			_mm_storeu_si128(((__m128i *) result), n11);

			result2[0] = result[0];
			result2[1] = result[2];
			result2[2] = result[4];
			result2[3] = result[6];
			result2[4] = result[8];
			result2[5] = result[10];
			result2[6] = result[12];
			result2[7] = result[14];
			result2[8] = result[1];
			result2[9] = result[3];
			result2[10] = result[5];
			result2[11] = result[7];
			result2[12] = result[9];
			result2[13] = result[11];
			result2[14] = result[13];
			result2[15] = result[15];

			strcpy ((char *)(pRes + row * 512/16 + col), result2);	

		}
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