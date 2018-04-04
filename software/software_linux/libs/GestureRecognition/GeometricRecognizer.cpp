#include "GeometricRecognizer.hpp"
#include <cmath>
#include <fstream>

namespace DollarRecognizer
{
	GeometricRecognizer::GeometricRecognizer()
	{

		setRotationInvariance(false);
	}

    void GeometricRecognizer::loadCharData(char c) {
        std::string dataname = chardatafolder;
        dataname += c;
        dataname += ".txt";

        std::ifstream data(dataname.c_str(), std::ios_base::in);
        Path2D path;

        while (!data.eof()) {
            int x, y;
            char comma;
            data >> x >> comma >> y;

            path.emplace_back(x, y);
        }

        addTemplate(std::string(1, c), path);

    }

	void GeometricRecognizer::loadSamples()
	{
		addTemplate("Arrow", Samples::getGestureArrow());
		addTemplate("Caret", Samples::getGestureCaret());
		addTemplate("CheckMark", Samples::getGestureCheckMark());
		addTemplate("Circle", Samples::getGestureCircle());
		addTemplate("Delete", Samples::getGestureDelete());
		addTemplate("Diamond", Samples::getGestureDiamond());
		//addTemplate("LeftCurlyBrace", Samples::getGestureLeftCurlyBrace());
		addTemplate("LeftSquareBracket", Samples::getGestureLeftSquareBracket());
		addTemplate("LeftToRightLine", Samples::getGestureLeftToRightLine());
		addTemplate("LineDownDiagonal", Samples::getGestureLineDownDiagonal());
		addTemplate("Pigtail", Samples::getGesturePigtail());
		addTemplate("QuestionMark", Samples::getGestureQuestionMark());
		addTemplate("Rectangle", Samples::getGestureRectangle());
		//addTemplate("RightCurlyBrace", Samples::getGestureRightCurlyBrace());
		addTemplate("RightSquareBracket", Samples::getGestureRightSquareBracket());
		addTemplate("RightToLeftLine", Samples::getGestureRightToLeftLine());
		addTemplate("RightToLeftLine2", Samples::getGestureRightToLeftLine2());
		addTemplate("RightToLeftSlashDown", Samples::getGestureRightToLeftSlashDown());
		addTemplate("Spiral", Samples::getGestureSpiral());
		addTemplate("Star", Samples::getGestureStar());
		addTemplate("Triangle", Samples::getGestureTriangle());
		addTemplate("V", Samples::getGestureV());
		addTemplate("X", Samples::getGestureX());
	}

	void GeometricRecognizer::addTemplate(std::string name, const Path2D &points)
	{
		Path2D pointnorm = normalizePath(points);

		templates.emplace_back(name, pointnorm);
	}

	BoundingBox GeometricRecognizer::boundingBox(const Path2D &points)
	{
		double minX =  std::numeric_limits<double>::max();
		double maxX = -std::numeric_limits<double>::max();
		double minY =  std::numeric_limits<double>::max(); 
		double maxY = -std::numeric_limits<double>::max();

		for (auto i = points.cbegin(); i != points.cend(); i++)
		{
			PointD point = *i;
			if (point.x < minX)
				minX = point.x;
			if (point.x > maxX)
				maxX = point.x;
			if (point.y < minY)
				minY = point.y;
			if (point.y > maxY)
				maxY = point.y;
		}
		return BoundingBox(minX, minY, (maxX - minX), (maxY - minY));
	}

	PointD GeometricRecognizer::centroid(const Path2D &points)
	{
		double x = 0.0, y = 0.0;
		for (auto i = points.cbegin(); i != points.cend(); i++)
		{
			PointD point = *i;
			x += point.x;
			y += point.y;
		}
		x /= points.size();
		y /= points.size();
		return PointD(x, y);
	}	

	double GeometricRecognizer::getDistance(PointD p1, PointD p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		double distance = sqrt((dx * dx) + (dy * dy));
		return distance;
	}

	double GeometricRecognizer::distanceAtAngle(
		const Path2D &points, const GestureTemplate &aTemplate, double rotation)
	{
		Path2D newPointDs = rotateBy(points, rotation);
		return pathDistance(newPointDs, aTemplate.points);
	}	

	double GeometricRecognizer::distanceAtBestAngle(
		const Path2D &points, const GestureTemplate &aTemplate)
	{
		double startRange = -angleRange;
		double endRange   =  angleRange;
		double x1 = goldenRatio() * startRange + (1.0 - goldenRatio()) * endRange;
		double f1 = distanceAtAngle(points, aTemplate, x1);
		double x2 = (1.0 - goldenRatio()) * startRange + goldenRatio() * endRange;
		double f2 = distanceAtAngle(points, aTemplate, x2);
		while (std::abs(endRange - startRange) > anglePrecision)
		{
			if (f1 < f2)
			{
				endRange = x2;
				x2 = x1;
				f2 = f1;
				x1 = goldenRatio() * startRange + (1.0 - goldenRatio()) * endRange;
				f1 = distanceAtAngle(points, aTemplate, x1);
			}
			else
			{
				startRange = x1;
				x1 = x2;
				f1 = f2;
				x2 = (1.0 - goldenRatio()) * startRange + goldenRatio() * endRange;
				f2 = distanceAtAngle(points, aTemplate, x2);
			}
		}
		return std::min(f1, f2);
	}

	Path2D GeometricRecognizer::normalizePath(const Path2D &points)
	{
		/* Recognition algorithm from 
			http://faculty.washington.edu/wobbrock/pubs/uist-07.1.pdf
			Step 1: Resample the PointD Path
			Step 2: Rotate Once Based on the "Indicative Angle"
			Step 3: Scale and Translate
			Step 4: Find the Optimal Angle for the Best Score
		*/
		// TODO: Switch to $N algorithm so can handle 1D shapes

		//--- Make everyone have the same number of points (anchor points)
        Path2D points2 = resample(points);
		//--- Pretend that all gestures began moving from right hand side
		//---  (degree 0). Makes matching two items easier if they're
		//---  rotated the same
        
        Path2D points3;
		if (getRotationInvariance())
			points3 = rotateToZero(points2);
        else
            points3 = std::move(points2);

        //--- Pretend all shapes are the same size. 
        //--- Note that since this is a square, our new shape probably
        //---  won't be the same aspect ratio
        Path2D points4 = scaleToSquare(points3);
        //--- Move the shape until its center is at 0,0 so that everyone
        //---  is in the same coordinate system
        Path2D points5 = translateToOrigin(points4);

        return points5;
	}

	/*  
	 *  Return:
	 *		relative distance between 2 gestures
	 * 	Require:
	 *		1. 2 gestures resampled to same size
	 *		2. 
	 */
	double GeometricRecognizer::pathDistance(const Path2D &pts1, const Path2D &pts2)
	{
		// assumes pts1.size == pts2.size

		double distance = 0.0;
		for (int i = 0; i < (int)pts1.size(); i++) 
			distance += getDistance(pts1[i], pts2[i]);
		return (distance / pts1.size());
	}

	/**
	 *  Return:
	 *		total distance of the gesture path from the 1st point to its last point
	 */
	double GeometricRecognizer::pathLength(const Path2D &points)
	{
		double distance = 0;
		for (int i = 1; i < (int)points.size(); i++)
			distance += getDistance(points[i - 1], points[i]);
		return distance;
	}

	RecognitionResult GeometricRecognizer::recognize(const Path2D &points)
	{
		//--- Make sure we have some templates to compare this to
		//---  or else recognition will be impossible
		if (templates.empty())
		{
			std::cout << "No templates loaded so no symbols to match." << std::endl;
			return RecognitionResult("Unknown", 0.0);
		}

		Path2D points2 = normalizePath(points);
	
		//--- Initialize best distance to the largest possible number
		//--- That way everything will be better than that
		double bestDistance = std::numeric_limits<double>::max();
		//--- We haven't found a good match yet
		int indexOfBestMatch = -1;

		//--- Check the shape passed in against every shape in our database
		for (int i = 0; i < (int)templates.size(); i++)
		{
			//--- Calculate the total distance of each point in the passed in
			//---  shape against the corresponding point in the template
			//--- We'll rotate the shape a few degrees in each direction to
			//---  see if that produces a better match
			double distance = distanceAtBestAngle(points2, templates[i]);
			if (distance < bestDistance)
			{
				bestDistance     = distance;
				indexOfBestMatch = i;
			}
		}

		//--- Turn the distance into a percentage by dividing it by 
		//---  half the maximum possible distance (across the diagonal 
		//---  of the square we scaled everything too)
		//--- Distance = hwo different they are
		//--- Subtract that from 1 (100%) to get the similarity
		double score = 1.0 - (bestDistance / halfDiagonal());

		//--- Make sure we actually found a good match
		//--- Sometimes we don't, like when the user doesn't draw enough points
		if (-1 == indexOfBestMatch)
		{
            std::cout << "Couldn't find a good match." << std::endl;
			return RecognitionResult("Unknown", 1);
		}

		RecognitionResult bestMatch(templates[indexOfBestMatch].name, score);
		return bestMatch;
	};

	Path2D GeometricRecognizer::resample(Path2D points)
	{
		double interval = pathLength(points) / (numPointDsInGesture - 1); // interval length
		double D = 0.0;
		Path2D newPointDs;

		//--- Store first point since we'll never resample it out of existence
		newPointDs.push_back(points.front());
	    for(int i = 1; i < (int)points.size(); i++)
		{
			const PointD &currentPointD  = points[i];
			const PointD &previousPointD = points[i-1];
			double d = getDistance(previousPointD, currentPointD);
			if ((D + d) >= interval)
			{
				double qx = previousPointD.x + ((interval - D) / d) * (currentPointD.x - previousPointD.x);
				double qy = previousPointD.y + ((interval - D) / d) * (currentPointD.y - previousPointD.y);
				PointD point(qx, qy);
				newPointDs.push_back(point);
				points.insert(points.begin() + i, point);
				D = 0.0;
			}
			else D += d;
		}

		// somtimes we fall a rounding-error short of adding the last point, so add it if so
		if (newPointDs.size() == (numPointDsInGesture - 1))
		{
			newPointDs.push_back(points.back());
		}

		return newPointDs;
	}

	Path2D GeometricRecognizer::rotateBy(const Path2D &points, double rotation) 
	{
		PointD c     = centroid(points);
		//--- can't name cos; creates compiler error since VC++ can't
		//---  tell the difference between the variable and function
		double cosine = cos(rotation);	
		double sine   = sin(rotation);
		
		Path2D newPointDs;
		for (auto i = points.cbegin(); i != points.cend(); i++)
		{
			PointD point = *i;
			double qx = (point.x - c.x) * cosine - (point.y - c.y) * sine   + c.x;
			double qy = (point.x - c.x) * sine   + (point.y - c.y) * cosine + c.y;
			newPointDs.push_back(PointD(qx, qy));
		}
		return newPointDs;
	}

	Path2D GeometricRecognizer::rotateToZero(const Path2D &points)
	{
		PointD c = centroid(points);
		double rotation = atan2(c.y - points[0].y, c.x - points[0].x);
		return rotateBy(points, -rotation);
	}

	Path2D GeometricRecognizer::scaleToSquare(const Path2D &points)
	{
		//--- Figure out the smallest box that can contain the path
		BoundingBox box = boundingBox(points);
		Path2D newPointDs;
		for (auto i = points.cbegin(); i != points.cend(); i++)
		{
			PointD point = *i;
			//--- Scale the points to fit the main box
			//--- So if we wanted everything 100x100 and this was 50x50,
			//---  we'd multiply every point by 2
			double scaledX = point.x * (this->squareSize / box.width);
			double scaledY = point.y * (this->squareSize / box.height);
			//--- Why are we adding them to a new list rather than 
			//---  just scaling them in-place?
			// TODO: try scaling in place (once you know this way works)
			newPointDs.push_back(PointD(scaledX, scaledY));
		}
		return newPointDs;
	}

	void   GeometricRecognizer::setRotationInvariance(bool ignoreRotation)
	{
		shouldIgnoreRotation = ignoreRotation;

		if (shouldIgnoreRotation)
		{
			angleRange = 45.0;
		}
		else
		{
			angleRange = 15.0;
		}
	}

	/**
	 * Shift the points so that the center is at 0,0.
	 * That way, if everyone centers at the same place, we can measure
	 *  the distance between each pair of points without worrying about
	 *  where each point was originally drawn
	 * If we didn't do this, shapes drawn at the top of the screen
	 *  would have a hard time matching shapes drawn at the bottom
	 *  of the screen
	 */
	Path2D GeometricRecognizer::translateToOrigin(const Path2D &points)
	{
		PointD c = centroid(points);
		Path2D newPointDs;
		for (auto i = points.cbegin(); i != points.cend(); i++)
		{
			PointD point = *i;
			double qx = point.x - c.x;
			double qy = point.y - c.y;
			newPointDs.push_back(PointD(qx, qy));
		}
		return newPointDs;
	}
}
