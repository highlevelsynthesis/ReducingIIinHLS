#include "iir_filter-tb.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>


int main() {


	unsigned int n = 1024;
	SIZE_TYPE p = 3;
	SIZE_TYPE q = 3;


//	DATA_TYPE *x = (DATA_TYPE *)malloc(n*sizeof(DATA_TYPE));
//	DATA_TYPE *y_hw = (DATA_TYPE *)malloc(n*sizeof(DATA_TYPE));
//	DATA_TYPE *y_sw = (DATA_TYPE *)malloc(n*sizeof(DATA_TYPE));
//
//	DATA_TYPE *b = (DATA_TYPE *)malloc(p*sizeof(DATA_TYPE));
//	DATA_TYPE *a = (DATA_TYPE *)malloc(q*sizeof(DATA_TYPE));

	DATA_TYPE x[1024];
	DATA_TYPE y_hw[1024];
	DATA_TYPE y_sw[1024];

	DATA_TYPE b[3];
	DATA_TYPE a[3];


	for (unsigned int i = 0; i < n; i++) {
		x[i] = rand()/(1.0*RAND_MAX);
		y_hw[i] = 0;
		y_sw[i] = 0;
	}
//	for (int i = 0; i < p; i++) {
//		b[i] = rand()/(1.0*RAND_MAX);
//	}
//
//	for (int i = 0; i < q; i++) {
//		a[i] = rand()/(10.0*RAND_MAX);
//	}

	b[0] = 0.20657;
	b[1] = 0.41314;
	b[2] = 0.20657;

	a[0] = 0;
	a[1] = 0.36953;
	a[2] = 0.19582;

	for (int i = 0; i < n; i++) {

		for (SIZE_TYPE j = 0; j < p; j++) {
			y_sw[i] += (i >= j)? b[j]*x[i-j] : 0;
		}
		for (SIZE_TYPE j = 1; j < q; j++) {
			y_sw[i] -= (i >= j)? a[j]*y_sw[i-j] : 0;
		}
	}

	iir_filter_kernel(x, y_hw, a, b, p, q, n);

    int match = 0;
    for(unsigned int i=0;i<n;i++) {
    	double diff = fabs(y_sw[i]-y_hw[i]);
//    	printf("Data at  %d with value y_hw = %lf,  y_sw = %lf \n",i,y_hw[i],y_sw[i]);
    	if(diff > 0.1 || diff != diff){
    		printf("error occurs at  %d with value y_hw = %lf, should be y_sw = %lf \n",i,y_hw[i],y_sw[i]);
    		match = -1;
    		break;
    	}
    }
    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return match;

}
