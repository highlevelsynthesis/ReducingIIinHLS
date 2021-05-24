


#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <math.h>


#define DATA_TYPE float


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

    int n = 4096;
    int m = 2048;


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
    cl::Kernel krnl_mxv(program,"mxv_00");



    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    cl::Buffer buffer_A(context, CL_MEM_READ_ONLY,  n*m*sizeof(DATA_TYPE));
    cl::Buffer buffer_x(context, CL_MEM_READ_ONLY,  m*sizeof(DATA_TYPE));
    cl::Buffer buffer_y(context, CL_MEM_WRITE_ONLY, n*sizeof(DATA_TYPE));


    //set the kernel Arguments
    int narg=0;
    krnl_mxv.setArg(narg++,buffer_A);
    krnl_mxv.setArg(narg++,buffer_x);
    krnl_mxv.setArg(narg++,buffer_y);
    krnl_mxv.setArg(narg++,n);
    krnl_mxv.setArg(narg++,m);


    //We then need to map our OpenCL buffers to get the pointers
    DATA_TYPE *ptr_A = (DATA_TYPE *) q.enqueueMapBuffer (buffer_A , CL_TRUE , CL_MAP_WRITE , 0, n*m*sizeof(DATA_TYPE));
    DATA_TYPE *ptr_x = (DATA_TYPE *) q.enqueueMapBuffer (buffer_x , CL_TRUE , CL_MAP_WRITE , 0, m*sizeof(DATA_TYPE));
    DATA_TYPE *ptr_y = (DATA_TYPE *) q.enqueueMapBuffer (buffer_y , CL_TRUE , CL_MAP_READ  , 0, n*sizeof(DATA_TYPE));

    //setting input data
    for(int i = 0 ; i< n*m; i++){
    	ptr_A[i] = rand()/(1.0*RAND_MAX);
    }
    for(int i = 0 ; i< m; i++){
    	ptr_x[i] = rand()/(1.0*RAND_MAX);
    }

    hardware_start = getTimestamp();
    // Data will be migrated to kernel space
    q.enqueueMigrateMemObjects({buffer_A, buffer_x},0/* 0 means from host*/);

    //Launch the Kernel
    q.enqueueTask(krnl_mxv);

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
    for(int i = 0 ; i< n; i++){
    	y_sw[i] = 0;
    }

    mxv_software(ptr_A, ptr_x, y_sw, n, m);

    //Verify the result
    int match = 0;
    for(int i=0;i<m;i++) {
    	double diff = fabs(y_sw[i]-ptr_y[i]);
    		if(diff > 0.1 || diff != diff){
    			printf("error occurs at  %d with value y_ hw = %lf, should be y_sw = %lf \n",i,ptr_y[i],y_sw[i]);
    			match = -1;
    			break;
    	    }

    	}

    q.enqueueUnmapMemObject(buffer_x , ptr_x);
    q.enqueueUnmapMemObject(buffer_y , ptr_y);
    q.finish();

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);

}


