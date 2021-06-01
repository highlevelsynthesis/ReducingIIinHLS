
#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>

#include "iir_filter-top.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <math.h>





double getTimestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}

double hardware_start;
double hardware_end;
double hardware_time;

void mxv_software(DATA_TYPE *A, DATA_TYPE *x, DATA_TYPE *y, int n, int m) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			y[i] += A[i*m+j]*x[j];
		}
	}
}


int main(int argc, char* argv[]) {

	srand (time(NULL));
    //TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];

	int n = 1024*1024;
	int p_ = 3;
	int q_ =	3;


    // Compute the size of array in bytes


    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary

    std::vector<cl::Device> devices;
    cl::Device device;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    //traversing all Platforms To find Xilinx Platform and targeted
    //Device in Xilinx Platform
    cl::Platform::get(&platforms);
    for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if ( platformName == "Xilinx"){
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
	    if (devices.size()){
		    device = devices[0];
		    found_device = true;
		    break;
	    }
        }
    }
    if (found_device == false){
       std::cout << "Error: Unable to find Target Device "
           << device.getInfo<CL_DEVICE_NAME>() << std::endl;
       return EXIT_FAILURE;
    }

    // Creating Context and Command Queue for selected device
    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg (0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg (0, bin_file.beg);
    char *buf = new char [nb];
    bin_file.read(buf, nb);

    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    devices.resize(1);
    cl::Program program(context, devices, bins);

    // This call will get the kernel object from program. A kernel is an
    // OpenCL function that is executed on the FPGA.
    cl::Kernel krnl_iir(program,"iir_filter_kernel");



    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    cl::Buffer buffer_x(context, CL_MEM_READ_ONLY,  n*sizeof(DATA_TYPE));
    cl::Buffer buffer_a(context, CL_MEM_READ_ONLY,  q_*sizeof(DATA_TYPE));
    cl::Buffer buffer_b(context, CL_MEM_READ_ONLY,  p_*sizeof(DATA_TYPE));
    cl::Buffer buffer_y(context, CL_MEM_WRITE_ONLY, n*sizeof(DATA_TYPE));


    //set the kernel Arguments
    int narg=0;
    krnl_iir.setArg(narg++,buffer_x);
    krnl_iir.setArg(narg++,buffer_y);
    krnl_iir.setArg(narg++,buffer_a);
    krnl_iir.setArg(narg++,buffer_b);
    krnl_iir.setArg(narg++,p_);
    krnl_iir.setArg(narg++,q_);
    krnl_iir.setArg(narg++,n);


    //We then need to map our OpenCL buffers to get the pointers
    DATA_TYPE *ptr_x = (DATA_TYPE *) q.enqueueMapBuffer (buffer_x , CL_TRUE , CL_MAP_WRITE , 0, n*sizeof(DATA_TYPE));
    DATA_TYPE *ptr_a = (DATA_TYPE *) q.enqueueMapBuffer (buffer_a , CL_TRUE , CL_MAP_WRITE , 0, q_*sizeof(DATA_TYPE));
    DATA_TYPE *ptr_b = (DATA_TYPE *) q.enqueueMapBuffer (buffer_b , CL_TRUE , CL_MAP_WRITE , 0, p_*sizeof(DATA_TYPE));
    DATA_TYPE *ptr_y = (DATA_TYPE *) q.enqueueMapBuffer (buffer_y , CL_TRUE , CL_MAP_READ  , 0, n*sizeof(DATA_TYPE));

    //setting input data
    for(int i = 0 ; i< n; i++){
    	ptr_x[i] = rand()/(1.0*RAND_MAX);
    	ptr_y[i] = 0;
    }

    ptr_b[0] = 0.20657;
    ptr_b[1] = 0.41314;
    ptr_b[2] = 0.20657;

	ptr_a[0] = 0;
	ptr_a[1] = 0.36953;
	ptr_a[2] = 0.19582;

    hardware_start = getTimestamp();
    // Data will be migrated to kernel space
    q.enqueueMigrateMemObjects({buffer_x, buffer_a, buffer_b},0/* 0 means from host*/);

    //Launch the Kernel
    q.enqueueTask(krnl_iir);

    // The result of the previous kernel execution will need to be retrieved in
    // order to view the results. This call will transfer the data from FPGA to
    // source_results vector
    q.enqueueMigrateMemObjects({buffer_y},CL_MIGRATE_MEM_OBJECT_HOST);

    q.finish();
    hardware_end = getTimestamp();
    hardware_time = (hardware_end-hardware_start)/1000;
    std::cout << "Exeution time running matrix multiplication in hardware: "
              << hardware_time << " msec " << std::endl;

    //Golden model
    DATA_TYPE *y_sw = (DATA_TYPE* )malloc(n* sizeof(DATA_TYPE));
    for ( int i = 0; i < n; i++) {
    	y_sw[i] = 0;
    }
	for ( int i = 0; i < n; i++) {

		for ( int j = 0; j < p_; j++) {
			y_sw[i] += (i-j > -1)? ptr_b[j]*ptr_x[i-j] : 0;
		}
		for ( int j = 1; j < q_; j++) {
			y_sw[i] -= (i-j > -1)? ptr_a[j]*y_sw[i-j] : 0;
		}
	}



    //Verify the result
    int match = 0;
    for(int i=0;i<n;i++) {
    	double diff = fabs(y_sw[i]-ptr_y[i]);
    		if(diff > 0.1 || diff != diff){
    			printf("error occurs at  %d with value y_hw = %lf, should be y_sw = %lf \n",i,ptr_y[i],y_sw[i]);
    			match = -1;
    			break;
    	    }

    	}

    q.enqueueUnmapMemObject(buffer_x , ptr_x);
    q.enqueueUnmapMemObject(buffer_a , ptr_a);
    q.enqueueUnmapMemObject(buffer_b , ptr_b);
    q.enqueueUnmapMemObject(buffer_y , ptr_y);
    q.finish();

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);

}



