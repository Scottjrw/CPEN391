#ifndef GEOMETRIC_TYPES_HPP
#define GEOMETRIC_TYPES_HPP

struct Point {
    unsigned x, y;

    Point(unsigned x, unsigned y):
        x(x), y(y) {}

    Point():
        Point(0, 0) {}

};

struct PointD {
    double x, y;

    PointD(double x, double y):
        x(x), y(y) {}
    
    PointD():
        PointD(0, 0) {}

    PointD(Point p):
        PointD(p.x, p.y) {}

    operator Point() {
        return Point(x, y);
    }
};

#endif // GEOMETRIC_TYPES_HPP


