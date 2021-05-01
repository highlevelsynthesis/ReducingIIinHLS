/************************************************
Copyright (c) 2021, Mohammad Hosseinabady
	mohammad@highlevel-synthesis.com.
	https://highlevel-synthesis.com/

All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. // Copyright (c) 2020, Mohammad Hosseinabady.
************************************************/
#include "cond_histogram-tb.h"
#include <iostream>
#include <math.h>
void cond_histogram_sw(int f[N], double w[N], double hist[N])
{
  for (int i=0; i<N; ++i) {
	double temp = w[i];
    if (temp >= 0){
		double x = hist[f[i]];

		hist[f[i]] = x + temp;
	}
  }
}


int main() {

	int status = 0;
	int f[N];
	double w[N];
	double hist_hw[N];
	double hist_sw[N];

	for(int i = 0; i<N; i++){
		f[i] = rand() % N;
		double a = rand()/(1.0*RAND_MAX);
		if (a < 0.5)
			w[i] = rand()/(1.0*RAND_MAX);
		else
			w[i] = -1*rand()/(1.0*RAND_MAX);
		hist_hw[i] = 0.0;
		hist_sw[i] = 0.0;
	}

	cond_histogram(f, w, hist_hw);
	cond_histogram_sw(f, w, hist_sw);

	for(unsigned int i=0;i<N;i++) {
		double diff = fabs(hist_sw[i]-hist_hw[i]);
		if(diff > 0.1 || diff != diff){
			std::cout << "error occurs at " << i
					  << " with value hist_hw = " << hist_hw[i]
					  << " should be hist_sw = " << hist_sw[i]
					  << std::endl;
			status = -1;
			break;
	    }


	}
	if(!status) {
		std::cout << "Validation PASSED!\n"<< std::endl;
	} else {
		std::cout << "Validation FAILED!\n"<< std::endl;
	}

	return status;
}
