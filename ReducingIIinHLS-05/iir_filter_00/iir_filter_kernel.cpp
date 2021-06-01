#include "iir_filter_kernel.h"



extern "C" {
void iir_filter_kernel(
		DATA_TYPE x[1024],
		DATA_TYPE y[1024],
		DATA_TYPE a[3],
		DATA_TYPE b[3],
		int p,
		int q,
		int n)
{
	for (int i = 0; i < n; i++) {

		for (SIZE_TYPE j = 0; j < p; j++) {
			y[i] += (i >= j)? b[j]*x[i-j] : 0;
		}
		for (SIZE_TYPE j = 1; j < q; j++) {
			y[i] -= (i >= j)? a[j]*y[i-j] : 0;
		}
	}

}
}

