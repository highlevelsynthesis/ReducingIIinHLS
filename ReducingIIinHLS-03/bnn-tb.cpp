/*

  Benchmark:	BNNKernel
  FileName:		bnn.cpp
  Author:		Jianyi Cheng
  Date:			12 Sep 2019

*/

/*
Modified by Mohammad Hosseinabady, 2021
https://highlevel-synthesis.com/

*/
#include <iostream>
void bnn(
		int addr_in[10000],
		int alpha,
		int w[10000],
		int addr_out[10000],
		int data[10000],
		int in[10000],
		int mean[10000]
	);
void inlined(int addr_in[10000], int alpha, int w[10000], int addr_out[10000], int data[10000], int in[10000], int mean[10000]){
  int lut, m,i,j,k, x, y, z, temp;

  for (i=0; i<100; i++){
	for (j=0;j<100;j++){
		x = i*100+j;
		lut=in[x]^w[x];
		data[x] += lut*alpha;
	}

	if (i==99){
		  for(k=0; k<100; k++){
		  y=i*100+k;
		  m=mean[y];
		  temp = data[y];
		  if (temp > 0)
			z = temp -m;
		  else
			z = temp +m;
		  data[y] = z;
		}
	}
  }
}
int main(){

	int status = 0;
  int addr_in[10000], alpha, w[10000], addr_out[10000], data[10000], gold[10000],in[10000], mean[10000];

  for (int i=0; i<10000; i++){
 	addr_in[i] = i;
	addr_out[i] = i;
	in[i] = 1;
	data[i] = 0;
	gold[i] = 0;
	mean[i] = 1;
	w[i] = 1;
  }

  alpha = 2;

  bnn(addr_in, alpha, w, addr_out, data, in, mean);
  inlined(addr_in, alpha, w, addr_out, gold, in, mean);

  int count = 0;
  for(int i=0;i<10000; i++) {
	if (data[i] != gold[i]) {
		status = 1;
		std::cout << " Error at i = " << i
				  << "hardware data[i] = "
				  << data[i] << " software data[i] = " << gold[i]
			      << std::endl;
		break;
	}
  }

	if(!status) {
		std::cout <<"Validation PASSED!"<< std::endl;
	} else {
		std::cout <<"Validation FAILED!"<< std::endl;
	}

  return status;
}
