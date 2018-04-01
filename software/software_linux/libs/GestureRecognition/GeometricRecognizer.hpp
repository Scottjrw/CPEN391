#ifndef _GeometricRecognizerIncluded_
#define _GeometricRecognizerIncluded_

#include <string>
#include <limits>
#include <iostream>
#include <vector>
#include "GeometricRecognizerTypes.hpp"
#include "GestureTemplate.hpp"
#include "SampleGestures.hpp"

namespace DollarRecognizer
{
	class GeometricRecognizer
	{
	public:
		GeometricRecognizer();

		void addTemplate(std::string name, Path2D points);
		RecognitionResult recognize(Path2D points);
		void loadSamples();

	private:
		BoundingBox boundingBox(Path2D &points);
		PointD centroid(Path2D &points);
		double getDistance(PointD p1, PointD p2);
		bool   getRotationInvariance() { return shouldIgnoreRotation; }
		double distanceAtAngle(
			Path2D points, GestureTemplate aTemplate, double rotation);
		double distanceAtBestAngle(Path2D points, GestureTemplate T);
		Path2D normalizePath(Path2D points);
		double pathDistance(Path2D pts1, Path2D pts2);
		double pathLength(Path2D points);
		Path2D resample(Path2D points);
		Path2D rotateBy(Path2D points, double rotation);
		Path2D rotateToZero(Path2D points);
		Path2D scaleToSquare(Path2D points);
		void   setRotationInvariance(bool ignoreRotation);
		Path2D translateToOrigin(Path2D points);

        // constexpr square root courtesy of https://gist.github.com/alexshtf/eb5128b3e3e143187794
        static constexpr double sqrtNewtonRaphson(double x, double curr, double prev) {
            return curr == prev
                ? curr
                : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
        }

        static constexpr double ct_sqrt(double x) {
            return x >= 0 && x < std::numeric_limits<double>::infinity()
                ? sqrtNewtonRaphson(x, x, 0)
                : std::numeric_limits<double>::quiet_NaN();
        }

		//numTemplates = 16;
		//--- How many points do we use to represent a gesture
		//--- Best results between 32-256
		static constexpr unsigned numPointDsInGesture = 128;
		//--- Before matching, we stretch the symbol across a square
		//--- That way we don't have to worry about the symbol the user drew
		//---  being smaller or larger than the one in the template
		static constexpr unsigned squareSize = 250;
		//--- 1/2 max distance across a square, which is the maximum distance
		//---  a point can be from the center of the gesture
		static constexpr double halfDiagonal() { return 0.5 * ct_sqrt(
                (squareSize * squareSize) + (squareSize * squareSize)); };
		//--- Before matching, we rotate the symbol the user drew so that the 
		//---  start point is at degree 0 (right side of symbol). That's how 
		//---  the templates are rotated so it makes matching easier
		//--- Note: this assumes we want symbols to be rotation-invariant, 
		//---  which we might not want. Using this, we can't tell the difference
		//---  between squares and diamonds (which is just a rotated square)
		static constexpr double anglePrecision = 2.0;
		//--- A magic number used in pre-processing the symbols
		static constexpr double goldenRatio() { return 0.5 * (-1.0 + ct_sqrt(5.0)); }

		double angleRange;

		bool shouldIgnoreRotation;

		//--- What we match the input shape against
        std::vector<GestureTemplate> templates;
	};
}
#endif
