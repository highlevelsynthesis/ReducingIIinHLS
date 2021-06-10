#include <ap_int.h>




extern "C" {
void scale_kernel(ap_uint<128> *x, ap_uint<128> *y, float alpha, float beta, int n) {
#pragma HLS INTERFACE m_axi bundle=gmem_0 port=x
#pragma HLS INTERFACE m_axi bundle=gmem_0 port=y

	for (int i = 0; i < n/(128/32); i++) {
		ap_uint<128> x_uint128 = x[i];
		ap_uint<128> y_uint128;
		for (int j = 0; j < (128/32); j++) {
			ap_uint<32> x_uint32 =  x_uint128((j+1)*32-1, j*32);
			float x_f = *((float *)&x_uint32);
			float y_f = alpha*x_f+beta;
			ap_uint<32> y_uint32 = *((ap_uint<32>*)&y_f);
			y_uint128((j+1)*32-1, j*32) = y_uint32;
		}
		y[i] = y_uint128;
	}

}
}
