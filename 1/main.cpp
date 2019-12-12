#include	"stdio.h"
#include	"ipp.h"
#include	"x86intrin.h"
#include	<ctime>

#define		SIZE		100000

int main (void){

	Ipp64u start, end;
	Ipp64u time1, time2;

	float *v1;

	v1 = new float [SIZE];

	float min_float;
	float final[4];

	//initializing
	for (int i = 0; i < SIZE; i++){
		v1[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
	}

	printf ("=== Vector Minimum  ==========================================\n");


	start = std::clock();

	min_float = v1[0];

	for (long i = 0; i < SIZE; i++) {
		if(v1[i] < min_float){
			min_float = v1[i];
		}
	}

	end   = std::clock();
	time1 = end - start;

	printf ("Serial Run Result = %f\n", min_float);
	printf ("Serial Run time = %u\n", (Ipp32s) time1);


	start = std::clock();

	__m128 min = _mm_loadu_ps (&v1[0]);
	__m128 temp;

	for (long i = 0; i < SIZE; i+=4) {
		temp = _mm_loadu_ps (&v1[i]);
		min = _mm_min_ps (min, temp);
	}
	
	_mm_store_ps (final, min);
	min_float = final[0];
	for (int i = 0; i < 4; i++) {
		if(final[i] < min_float){
			min_float = final[i];
		}
	}

	end   = std::clock();
	time2 = end - start;


	printf ("Parallel Run Result = %f\n", min_float);
	printf ("Parallel Run time = %u\n", (Ipp32s) time2);

	printf ("Speedup = %4.2f\n", (float) (time1)/(float) time2);


	return 0;
}
