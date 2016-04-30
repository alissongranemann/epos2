// EPOS Geometry Utility Declarations

#include <utility/math.h>

__BEGIN_UTIL

template<typename T, unsigned int dimensions>
struct Point;

template<typename T>
struct Point<T, 3>
{
    typedef T Distance;

    Point(const T & xi = 0, const T & yi = 0, const T & zi = 0): x(xi), y(yi), z(zi) {}

    Distance operator-(const Point & p) const {
        T xx = p.x - x;
        T yy = p.y - y;
        T zz = p.z - z;
        return sqrt(xx*xx + yy*yy + zz*zz);
    }

    bool operator==(const Point & p) const { return x == p.x and y == p.y and z == p.z; }
    bool operator!=(const Point & p) const { return !operator==(*this, p); }

    friend Debug & operator<<(Debug & db, const Point & c) {
        db << "{" << c.x << "," << c.y << "," << c.z << "}";
        return db;
    }
    friend OStream & operator<<(OStream & os, const Point & c) {
        os << "{" << c.x << "," << c.y << "," << c.z << "}";
        return os;
    }

    T x, y, z;
};

template<typename T>
struct Point<T, 2>
{
    typedef T Distance;

    Point(const T & xi = 0, const T & yi = 0): x(xi), y(yi) {}

    Distance operator-(const Point<T, 2> & p) const {
        T xx = p.x - x;
        T yy = p.y - y;
        return sqrt(xx*xx + yy*yy);
    }

    bool operator==(const Point & p) const { return x == p.x and y == p.y; }
    bool operator!=(const Point & p) const { return !(*this == p); }

    friend Debug & operator<<(Debug & db, const Point<T, 2> & c) {
        db << "{" << c.x << "," << c.y << "}";
        return db;
    }
    friend OStream & operator<<(OStream & os, const Point<T, 2> & c) {
        os << "{" << c.x << "," << c.y << "}";
        return os;
    }

    T x, y;
};

template<typename T>
struct Sphere
{
    typedef Point<T, 3> C;
    typedef typename C::Distance R;

    Sphere() {}
    Sphere(const C & c, const R & r = 0): center(c), radius(r) { }

    bool contains(const C & coord) const { return (center - coord) <= radius; }

    friend Debug & operator<<(Debug & db, const Sphere & s) {
        db << "{" << "c=" << s.center << ",r=" << s.radius << "}";
        return db;
    }
    friend OStream & operator<<(OStream & os, const Sphere & s) {
        os << "{" << "c=" << s.center << ",r=" << s.radius << "}";
        return os;
    }

    C center;
    R radius;
};

__END_UTIL

