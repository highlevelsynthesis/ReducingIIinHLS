#include "iir_filter_kernel.h"



extern "C" {
void iir_filter_kernel(
		DATA_TYPE x[1024],
		DATA_TYPE y[1024],
		DATA_TYPE a[3],
		DATA_TYPE b[3],
		int  p,
		int  q,
		int n)
{


	DATA_TYPE x_buffer[P_MAX];
#pragma HLS ARRAY_PARTITION variable=x_buffer dim=1 complete
	DATA_TYPE y_buffer[Q_MAX];
#pragma HLS ARRAY_PARTITION variable=y_buffer dim=1 complete
	DATA_TYPE a_buffer[P_MAX];
#pragma HLS ARRAY_PARTITION variable=a_buffer dim=1 complete
	DATA_TYPE b_buffer[Q_MAX];
#pragma HLS ARRAY_PARTITION variable=b_buffer dim=1 complete




	for (int i = 0; i < p; i++) {
		x_buffer[i] = 0;
		b_buffer[i] = b[i];
	}
	for (int i = 0; i < q; i++) {
		y_buffer[i] = 0;
		a_buffer[i] = a[i];
	}


	for (int i = 0; i < n; i++) {


		for (int  j = 0; j < p; j++) {
#pragma HLS pipeline
			x_buffer[p-j] = x_buffer[p-j-1];
		}
		x_buffer[0] = x[i];


		DATA_TYPE y_local_x = 0;
		for (int j = 0; j < P_MAX; j++) {
#pragma HLS unroll
			if (j < p)
				y_local_x += b_buffer[j]*x_buffer[j];
			else
				y_local_x += 0;
		}

		DATA_TYPE y_local_y = 0;
		for (int j = 1; j < Q_MAX; j++) {
#pragma HLS unroll
			if (j < q)
				y_local_y += a_buffer[j]*y_buffer[j];
			else
				y_local_y += 0;
		}

		DATA_TYPE y_local = y_local_x - y_local_y;



		for (int j = 0; j < q; j++) {
#pragma HLS pipeline
			y_buffer[q-j] = y_buffer[q-j-1];
		}


		y_buffer[1] = y_local;
		y[i] = y_local;
	}
}
}






