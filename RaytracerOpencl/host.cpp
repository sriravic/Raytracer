#include <iostream>
#include <CL/cl.hpp>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <memory>

int main(int argc, char** argv) {
    std::cout << "OpenCL Raytracer" << std::endl;
    std::cout << "Srinath Ravichandran" << std::endl;

    // Step1 : initialize and get all available platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() == 0) {
        std::cout << "Did not find any available opencl platforms" << std::endl;
        return -1;
    }

    // lets us the 1'st platform for now
    cl::Platform platform = platforms[0];
    std::cout << "Using OpenCL platform : " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    // Step2 : Get a device first
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (devices.size() == 0) {
        std::cout << "Did not find any devices" << std::endl;
        return -1;
    }
    
    // let's use the 1st device
    cl::Device device = devices[0];
    std::cout << "Using OpenCL device : " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    // Step3 : Create a context
    cl::Context context(device);

    // Step 4: Create a program
    cl::Program::Sources sources;

    // Step 5: Compose the opencl program string
    std::ifstream inputSrc("raytracer.cl");
    std::string srcCode(std::istreambuf_iterator<char>(inputSrc), (std::istreambuf_iterator<char>()));
    sources.push_back(std::make_pair(srcCode.c_str(), srcCode.length()));
    
    // Step 6: Create a program
    cl::Program program(context, sources);
    if (program.build({ device }) != CL_SUCCESS) {
        std::cout << "Error building :" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return -1;
    }

    // Define the raytracing constants
    int nx = 400;
    int ny = 200;
    int ns = 1;
    
    // Write the output file
    std::ofstream outfile("out.ppm");
    outfile << "P3\n" << nx << " " << ny << "\n255\n";      // header for the pfm file

    // create buffers on the device
    cl::Buffer dColorBuffer(context, CL_MEM_READ_WRITE, sizeof(unsigned char) * 3 * nx * ny);
        
    // create a command queue
    cl::CommandQueue queue(context, device);

    // run the kernel.
    cl::Kernel kernel(program, "color");
    kernel.setArg(0, dColorBuffer);
    kernel.setArg(1, nx);
    kernel.setArg(2, ny);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(nx * ny), cl::NullRange);
    queue.finish();

    // get the reults
    std::shared_ptr<unsigned char> hColorBuffer(new unsigned char[nx * ny * 3]);
    queue.enqueueReadBuffer(dColorBuffer, CL_TRUE, 0, sizeof(unsigned char) * 3 * nx * ny, hColorBuffer.get());

    // write the file out
    std::cout << "Writing out image file.." << std::endl;
    for (int i = 0; i < nx * ny; i++) {
        outfile << (int)hColorBuffer.get()[3 * i + 0] << " " << 
                   (int)hColorBuffer.get()[3 * i + 1] << " " << 
                   (int)hColorBuffer.get()[3 * i + 2] << "\n";
    }
    
    return 0;
}