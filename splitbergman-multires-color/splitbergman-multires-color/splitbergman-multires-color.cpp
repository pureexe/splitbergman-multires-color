// arrayfire-inpaint-multires.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SplitBergmanSolver.h"
#include "RecursiveInpainter.h"
#include "RootMeanSquareError.h"
#include "PeakSignalToNoiseRatio.h"
#include <string>


void application();
void TaskTest();

int main()
{
	application();
	//TaskTest();
	return 0;
}

void TaskTest() {
	// TEST PASS :D
}

void application()
{
	//defined related variable
	double lambda = 250;
	double theta = 5;
	double tolerant = 5*1e-4;
	int max_iteration = 100;
	int max_guass_seidel = 1;
	double omega = 1.3;


	//setBackend(AF_BACKEND_CUDA);
	setBackend(AF_BACKEND_OPENCL);

	std::string prefix = "../../images/256x256_color/case";
	std::string result_prefix = "../../images/case";
	std::string case_number = "4";
	std::string toInpaintFileName = prefix + case_number + "_toinpaint.png";
	std::string inpaintDomainFileName = prefix + case_number + "_inpaintdomain.png";
	std::string  originalImageFileName = prefix + case_number + "_original.png";

	
	array toInpaintImage = loadImage(toInpaintFileName.c_str(),true);
	array inpaintDomainImage = loadImage(inpaintDomainFileName.c_str());
	array originalImage = loadImage(originalImageFileName.c_str(),true);
	
	//Make 1-channel domain into 3-channel
	array buffDomain = constant(0, inpaintDomainImage.dims(0), inpaintDomainImage.dims(1), 3);
	seq row(inpaintDomainImage.dims(0)), col(inpaintDomainImage.dims(1));
	buffDomain(row, col, 0) = inpaintDomainImage;
	buffDomain(row, col, 1) = inpaintDomainImage;
	buffDomain(row, col, 2) = inpaintDomainImage;
	inpaintDomainImage = buffDomain.copy();

	toInpaintImage /= 255;
	originalImage /= 255;
	array lambdaDomain = ((inpaintDomainImage < 1) * lambda) / 1.0;
	timer::start();
	//array resultImage = SplitBergmanSolver(toInpaintImage, lambdaDomain, theta, tolerant, omega, max_guass_seidel, max_iteration);
	array resultImage = RecusriveInpainter(toInpaintImage, inpaintDomainImage, lambda, theta, tolerant, omega, max_guass_seidel, max_iteration, 4, 1);
	printf("Inpainting time %g seconds\n", timer::stop());
	printf("RMSE: %g\n", RootMeanSquareError(flat(resultImage), flat(originalImage))); 
	printf("PSNR: %g\n", PeakSignalToNoiseRatio(flat(resultImage), flat(originalImage))); 
	
	resultImage *= 255;
	result_prefix += case_number + ".png";
	saveImageNative(result_prefix.c_str(), resultImage.as(u8));
	Window resultWindows(256, 256, "ResultImage");
	do {
		resultWindows.image(resultImage.as(u8));
	} while (!resultWindows.close());
}
