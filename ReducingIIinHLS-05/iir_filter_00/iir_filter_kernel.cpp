#include "iir_filter_kernel.h"




extern "C" {
void iir_filter_kernel(
		DATA_TYPE *x,
		DATA_TYPE *y,
		DATA_TYPE *a,
		DATA_TYPE *b,
		int p,
		int q,
		int n)
{
	for ( int i = 0; i < n; i++) {

		for ( int j = 0; j < p; j++) {
			y[i] += (i-j > -1)? b[j]*x[i-j] : 0;
		}
		for ( int j = 1; j < q; j++) {
			y[i] -= (i-j > -1)? a[j]*y[i-j] : 0;
		}
	}

}
}






