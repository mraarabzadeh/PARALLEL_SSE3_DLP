#include	"stdio.h"
#include	"ipp.h"
#include	"x86intrin.h"
#include	<ctime>
#include	<cmath>

#define		SIZE		100000

int main (void){

	Ipp64u start, end;
	Ipp64u time1, time2;

	float *v1, *v2;

	v1 = new float [SIZE];
	v2 = new float [SIZE];

	//initializing
	for (int i = 0; i < SIZE; i++){
		v1[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
		v2[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
	}

	printf ("=== Matrix Vector Multiplication  ==========================================\n");


	start = std::clock();

	float sum_float = 0;
	for (long i = 0; i < SIZE; i++) {
		sum_float += pow(v1[i] - v2[i], 2);
	}
	float result = sqrt(sum_float);

	end   = std::clock();
	time1 = end - start;

	printf ("Serial Run Result = %f\n", result);
	printf ("Serial Run time = %u\n", (Ipp32s) time1);


	start = std::clock();

	__m128 sum = _mm_set1_ps(0.0f);
	__m128 var1;
	__m128 var2;

	for (long i = 0; i < SIZE; i+=4) {
		var1 = _mm_loadu_ps (&v1[i]);
		var2 = _mm_loadu_ps (&v2[i]);
		var1 = _mm_sub_ps (var1, var2);
		var1 = _mm_mul_ps (var1, var1);
		sum = _mm_add_ps (var1, sum);
	}
	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_hadd_ps (sum, sum);
	result = _mm_cvtss_f32 (sum);

	end   = std::clock();
	time2 = end - start;

	printf ("Parallel Run Result = %f\n", sqrt(result));
	printf ("Parallel Run time = %u\n", (Ipp32s) time2);

	printf ("Speedup = %4.2f\n", (float) (time1)/(float) time2);

	return 0;
}
