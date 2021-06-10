

extern "C" {
void scale_kernel(float *x, float *y, float alpha, float beta, int n) {
#pragma HLS INTERFACE m_axi bundle=gmem_0 port=x
#pragma HLS INTERFACE m_axi bundle=gmem_0 port=y
	for (int i = 0; i < n; i++) {
		y[i] = alpha*x[i]+beta;
	}

}
}
