#pragma once

#include "iir_filter_kernel.h"


extern "C" {
void iir_filter_kernel(
		DATA_TYPE *x,
		DATA_TYPE *y,
		DATA_TYPE *a,
		DATA_TYPE *b,
		int        p_,
		int        q_,
		int        n);
}
