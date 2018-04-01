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
        Point2D(50, 50),
        Point2D(100, 100),
        Point2D(150, 150),
        Point2D(200, 100),
        Point2D(250, 50)
    };

	RecognitionResult result = gr.recognize(newPath);

    std::cout << "the gesture input is: " << result.name << "\n";

    // add a new sample and compare with virtual user input
    Path2D LowerASample = {
        Point2D(50,80),
        Point2D(75,51),
        Point2D(50,49),
        Point2D(25,67.5),
        Point2D(15,125),
        Point2D(17.5,150),
        Point2D(25,170),
        Point2D(32.5,165),
        Point2D(50,160),
        Point2D(56.25,150),
        Point2D(62.5,140),
        Point2D(75,125),
        Point2D(87.5,100),
        Point2D(93.75,80),
        Point2D(100,100),
        Point2D(106.25,130),
        Point2D(112.5,147),
        Point2D(118.75,148.5),
        Point2D(125,150)
    };

	gr.addTemplate("a", LowerASample);

	Path2D LowerACustom = {
        Point2D(151,82),
        Point2D(124,51),
        Point2D(105,45),
        Point2D(78,67.5),
        Point2D(62,125),
        Point2D(67.5,150),
        Point2D(73,170),
        Point2D(85,165),
        Point2D(102,160),
        Point2D(106.25,150),
        Point2D(120.5,140),
        Point2D(125,125),
        Point2D(137.5,98),
        Point2D(143.75,80),
        Point2D(150,100),
        Point2D(156.25,135),
        Point2D(162.5,152),
        Point2D(168.75,144),
        Point2D(175,150)
    };

	RecognitionResult newresult = gr.recognize(LowerACustom);

    std::cout << "the gesture input is: " << newresult.name << "\n";


	return 0;
}
