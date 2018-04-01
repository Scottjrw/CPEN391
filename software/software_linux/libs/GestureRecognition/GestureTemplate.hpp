#ifndef _GestureTemplateIncluded_
#define _GestureTemplateIncluded_

#include <vector>
#include <string>
#include "GeometricRecognizerTypes.hpp"

namespace DollarRecognizer
{
struct GestureTemplate
{
public:
    std::string name;
	Path2D points;
	
	GestureTemplate(std::string name, Path2D points):
        name(name), points(points) {}
};
}

#endif
