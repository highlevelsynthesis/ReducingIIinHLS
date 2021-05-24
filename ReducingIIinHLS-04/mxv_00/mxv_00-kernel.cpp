
#include <hls_stream.h>

#define N_MAX 8192
#define M_MAX 8192
#define DATA_TYPE float

extern "C" {
void mxv_00(DATA_TYPE *A, DATA_TYPE *x, DATA_TYPE *y, int n, int m) {


	for (int i = 0; i < n; i++) {
		DATA_TYPE y_tmp = 0;
		for (int j = 0; j < m; j++) {
			y_tmp += A[i*m+j]*x[j];
		}
		y[i] = y_tmp;
	}
}
}
