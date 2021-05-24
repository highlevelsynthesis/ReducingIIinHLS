#include <hls_stream.h>

#define N_MAX 4096
#define M_MAX 4096
#define DATA_TYPE float

void readA(DATA_TYPE *A, hls::stream<DATA_TYPE> &a_stream, int n, int m) {
	for (int i = 0; i < n*m; i++) {
		a_stream << A[i];
	}
}

void compute(hls::stream<DATA_TYPE> &a_stream, DATA_TYPE x_local[M_MAX], hls::stream<DATA_TYPE> &y_stream, int n, int m) {
	DATA_TYPE y_tmp = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j+=8) {
			if (j == 0)
				y_tmp = 0;
			DATA_TYPE a_l[8];

			for (int k = 0; k < 8; k++)
				a_l[k] = a_stream.read();

			DATA_TYPE y_tmp_l = 0;
			for (int k = 0; k < 8; k++) {
				y_tmp_l += a_l[k]*x_local[j+k];
			}

			y_tmp += y_tmp_l;

			if (j+8 == m)
				y_stream << y_tmp;
		}
	}
}

void writeY(hls::stream<DATA_TYPE> &y_stream, DATA_TYPE *y, int n, int m) {
	DATA_TYPE y_break = 0;
	for (int i = 0; i < n; i++) {
		DATA_TYPE y_l = y_stream.read();
		y[i] = y_l;
	}
}
void mxv_core(DATA_TYPE *A, DATA_TYPE x_local[M_MAX], DATA_TYPE *y, int n, int m) {

#pragma HLS DATAFLOW
	hls::stream<DATA_TYPE> a_stream;
	hls::stream<DATA_TYPE> y_stream;
	readA(A, a_stream, n, m);
	compute(a_stream, x_local, y_stream, n, m);
	writeY(y_stream, y, n, m);
}

extern "C" {
void mxv_03(DATA_TYPE *A, DATA_TYPE *x, DATA_TYPE *y, int n, int m) {

	DATA_TYPE y_tmp;
	DATA_TYPE x_local[M_MAX];


	for (int i = 0; i < m; i++) {
		x_local[i] = x[i];
	}


	mxv_core(A, x_local, y, n, m);

}

}
