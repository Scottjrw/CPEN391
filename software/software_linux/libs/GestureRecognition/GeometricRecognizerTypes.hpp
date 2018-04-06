#ifndef _GeometricRecognizerTypesIncluded_
#define _GeometricRecognizerTypesIncluded_

/*
* This code taken (and modified) from the javascript version of:
* The $1 Gesture Recognizer
*
*		Jacob O. Wobbrock
* 		The Information School
*		University of Washington
*		Mary Gates Hall, Box 352840
*		Seattle, WA 98195-2840
*		wobbrock@u.washington.edu
*
*		Andrew D. Wilson
*		Microsoft Research
*		One Microsoft Way
*		Redmond, WA 98052
*		awilson@microsoft.com
*
*		Yang Li
*		Department of Computer Science and Engineering
* 		University of Washington
*		The Allen Center, Box 352350
*		Seattle, WA 98195-2840
* 		yangli@cs.washington.edu
*/
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include "GeometricTypes.hpp"

namespace DollarRecognizer
{
	typedef std::vector<PointD>  Path2D;
	typedef Path2D::iterator Path2DIterator;

	struct BoundingBox
	{
	public:
		double x, y, width, height;

		BoundingBox(double x, double y, double width, double height):
            x(x), y(y), width(width), height(height) {}
	};

	struct RecognitionResult
	{
	public:
        std::string name;
        std::string name2;
		double score;
        double score2;

		RecognitionResult(std::string name, double score):
            name(name), name2(), score(score), score2(0) {}
		RecognitionResult(std::string name, double score,
                std::string name2, double score2):
            name(name), name2(name2), score(score), score2(score2) {}
	};
}
#endif
