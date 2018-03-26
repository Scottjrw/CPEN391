#include "GeometricRecognizer.h"
#include "GeometricRecognizerTypes.h"
#include <string>
#include <vector>
#include <iostream>

using namespace DollarRecognizer;

int main(int argc, const char *argv[])
{

	GeometricRecognizer *gr = new GeometricRecognizer();

	gr->loadTemplates();

	Path2D newPath;
	// compare manual input with samples
	newPath.push_back(Point2D(50, 50));
	newPath.push_back(Point2D(100, 100));
	newPath.push_back(Point2D(150, 150));
	newPath.push_back(Point2D(200, 100));
	newPath.push_back(Point2D(250, 50));

	RecognitionResult result = gr->recognize(newPath);

	cout << "the gesture input is: " << result.name << "\n";

	// add a new sample and compare with virtual user input
	Path2D LowerACustom;
	Path2D LowerASample;
	
	LowerASample.push_back(Point2D(50,80));
	LowerASample.push_back(Point2D(75,51));
	LowerASample.push_back(Point2D(50,49));
	LowerASample.push_back(Point2D(25,67.5));
	LowerASample.push_back(Point2D(15,125));
	LowerASample.push_back(Point2D(17.5,150));
	LowerASample.push_back(Point2D(25,170));
	LowerASample.push_back(Point2D(32.5,165));
	LowerASample.push_back(Point2D(50,160));
	LowerASample.push_back(Point2D(56.25,150));
	LowerASample.push_back(Point2D(62.5,140));
	LowerASample.push_back(Point2D(75,125));
	LowerASample.push_back(Point2D(87.5,100));
	LowerASample.push_back(Point2D(93.75,80));
	LowerASample.push_back(Point2D(100,100));
	LowerASample.push_back(Point2D(106.25,130));
	LowerASample.push_back(Point2D(112.5,147));
	LowerASample.push_back(Point2D(118.75,148.5));
	LowerASample.push_back(Point2D(125,150));

	gr->addTemplate("a", LowerASample);

	
	LowerACustom.push_back(Point2D(151,82));
	LowerACustom.push_back(Point2D(124,51));
	LowerACustom.push_back(Point2D(105,45));
	LowerACustom.push_back(Point2D(78,67.5));
	LowerACustom.push_back(Point2D(62,125));
	LowerACustom.push_back(Point2D(67.5,150));
	LowerACustom.push_back(Point2D(73,170));
	LowerACustom.push_back(Point2D(85,165));
	LowerACustom.push_back(Point2D(102,160));
	LowerACustom.push_back(Point2D(106.25,150));
	LowerACustom.push_back(Point2D(120.5,140));
	LowerACustom.push_back(Point2D(125,125));
	LowerACustom.push_back(Point2D(137.5,98));
	LowerACustom.push_back(Point2D(143.75,80));
	LowerACustom.push_back(Point2D(150,100));
	LowerACustom.push_back(Point2D(156.25,135));
	LowerACustom.push_back(Point2D(162.5,152));
	LowerACustom.push_back(Point2D(168.75,144));
	LowerACustom.push_back(Point2D(175,150));
	


	RecognitionResult newresult = gr->recognize(LowerACustom);

	cout << "the gesture input is: " << newresult.name << "\n";


	return 0;
}
