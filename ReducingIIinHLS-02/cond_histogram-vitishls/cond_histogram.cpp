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
#include "cond_histogram.h"

//---------------------------------------------------------------------
//     Original Code
//---------------------------------------------------------------------
void cond_histogram(int f[N], double w[N], double hist[N])
{
  for (int i=0; i<N; ++i) {
	double temp = w[i];
    if (temp >= 0){
		double x = hist[f[i]];

		hist[f[i]] = x + temp;
	}
  }
}

//---------------------------------------------------------------------
//     First Modification
//---------------------------------------------------------------------

//void cond_histogram(int f[N], double w[N], double hist[N])
//{
//  for (int i=0; i<N; ++i) {
//	double temp = w[i];
//	double t;
//    if (temp >= 0)
//		t = temp;
//	else
//		t = 0;
//	hist[f[i]] = hist[f[i]] + t;
//  }
//}


//---------------------------------------------------------------------
//     Second Modification
//---------------------------------------------------------------------
//#define M 10
//void cond_histogram(int f[N], double w[N], double hist[N])
//{
//	double hist_local[M][N];
//#pragma HLS ARRAY_PARTITION variable=hist_local dim=1 complete
//
//	for (int i=0; i<N; ++i) {
//	#pragma HLS pipeline
//		  double h = 0;
//		  for ( int j = 0; j < M; j++) {
//			  hist_local[j][i] = 0;
//		  }
//	  }
//
//	  for (int i=0; i<N/M; ++i) {
//	#pragma HLS pipeline
//		  for ( int j = 0; j < M; j++) {
//			  double temp = w[i*M+j];
//			  double a;
//			  if (temp >= 0)
//				  a = temp;
//			  else
//				  a = 0;
//			  hist_local[j][f[i*M+j]] += a;
//		  }
//	  }
//	  for (int i=0; i<N; ++i) {
//	#pragma HLS pipeline
//		  double h = 0;
//		  for ( int j = 0; j < M; j++) {
//			  h += hist_local[j][i];
//		  }
//		  hist[i] += h;
//	  }
//}
//
