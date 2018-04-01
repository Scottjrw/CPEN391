#include "GeometricRecognizer.hpp"
#include "GeometricRecognizerTypes.hpp"
#include <string>
#include <vector>
#include <iostream>

using namespace DollarRecognizer;

int main(int argc, const char *argv[])
{

    GeometricRecognizer gr;

	gr.loadSamples();

	// compare manual input with samples
	Path2D newPath = {
        PointD(50, 50),
        PointD(100, 100),
        PointD(150, 150),
        PointD(200, 100),
        PointD(250, 50)
    };

	RecognitionResult result = gr.recognize(newPath);

    std::cout << "the gesture input is: " << result.name << "\n";

    // add a new sample and compare with virtual user input
    Path2D LowerASample = {
        PointD(50,80),
        PointD(75,51),
        PointD(50,49),
        PointD(25,67.5),
        PointD(15,125),
        PointD(17.5,150),
        PointD(25,170),
        PointD(32.5,165),
        PointD(50,160),
        PointD(56.25,150),
        PointD(62.5,140),
        PointD(75,125),
        PointD(87.5,100),
        PointD(93.75,80),
        PointD(100,100),
        PointD(106.25,130),
        PointD(112.5,147),
        PointD(118.75,148.5),
        PointD(125,150)
    };

	gr.addTemplate("a", LowerASample);

	Path2D LowerACustom = {
        PointD(151,82),
        PointD(124,51),
        PointD(105,45),
        PointD(78,67.5),
        PointD(62,125),
        PointD(67.5,150),
        PointD(73,170),
        PointD(85,165),
        PointD(102,160),
        PointD(106.25,150),
        PointD(120.5,140),
        PointD(125,125),
        PointD(137.5,98),
        PointD(143.75,80),
        PointD(150,100),
        PointD(156.25,135),
        PointD(162.5,152),
        PointD(168.75,144),
        PointD(175,150)
    };

	RecognitionResult newresult = gr.recognize(LowerACustom);

    std::cout << "the gesture input is: " << newresult.name << "\n";


	return 0;
}
