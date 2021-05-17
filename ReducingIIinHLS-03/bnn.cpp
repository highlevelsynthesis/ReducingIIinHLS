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



//----------------------------------------------------------------
//             Original Code
//----------------------------------------------------------------
//void bnn(
//		int addr_in[10000],
//		int alpha,
//		int w[10000],
//		int addr_out[10000],
//		int data[10000],
//		int in[10000],
//		int mean[10000]
//	){
//
//	int lut, m,i,j,k, x, y, z, temp;
//
//	for (i=0; i<100; i++){
//		for (j=0;j<100;j++){
//			x = i*100+j;
//			lut=in[x]^w[x];
//			data[addr_in[x]] += lut*alpha;
//		}
//
//		if (i == 99){
//			for(k=0; k<100; k++){
//				y=i*100+k;
//				m=mean[y];
//				temp = data[addr_out[y]];
//				if (temp > 0)
//					z = temp -m;
//				else
//					z = temp +m;
//				data[addr_out[y]] = z;
//			}
//		}
//	}
//}


//----------------------------------------------------------------
//             Optimised Code
//----------------------------------------------------------------
void bnn(
		int addr_in[10000],
		int alpha,
		int w[10000],
		int addr_out[10000],
		int data[10000],
		int in[10000],
		int mean[10000]
	){

	int lut, m,i,j,k, x, y, z, temp;


	for (i=0; i<100; i++){
		for (j=0;j<100;j++){
			x = i*m+j;
			lut=in[x]^w[x];
			data[addr_in[x]] += lut*alpha;
		}
	}

	for(k=0; k<100; k++){
		y=9900+k;
		m=mean[y];
		temp = data[addr_out[y]];
		if (temp > 0)
			z = temp -m;
		else
			z = temp +m;
		data[addr_out[y]] = z;
	}

}
