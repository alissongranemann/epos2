#include <utility/math.h>

__BEGIN_UTIL

template<unsigned int dimensions = 3, typename T = int>
struct Point {
    T x, y, z;

    typedef T Distance;

    Point(const T& xi = 0, const T& yi = 0, const T& zi = 0) :x(xi), y(yi), z(zi) { }

    friend Debug & operator<<(Debug & db, const Point & c) {
        db << "{" << c.x << "," << c.y << "," << c.z << "}";
        return db;
    }
    friend OStream & operator<<(OStream & os, const Point & c) {
        os << "{" << c.x << "," << c.y << "," << c.z << "}";
        return os;
    }

    Distance operator-(const Point & rhs) const {
        T xx = rhs.x - x;
        T yy = rhs.y - y;
        T zz = rhs.z - z;
        //return Math::sqrt(xx*xx + yy*yy + zz*zz); //TODO: fix when multiplication works on eMote3
        return Math::abs(xx) + Math::abs(yy) + Math::abs(zz);
    }

    bool operator==(const Point& rhs) { return x == rhs.x and y == rhs.y and z == rhs.z; }
    bool operator!=(const Point& rhs){ return !operator==(*this,rhs); }
};

template<>
struct Point<2, int> {
    typedef int T;

    T x, y;

    typedef T Distance;

    Point(const T& xi = 0, const T& yi = 0) :x(xi), y(yi) { }

    friend Debug & operator<<(Debug & db, const Point<2, T> & c) {
        db << "{" << c.x << "," << c.y << "}";
        return db;
    }
    friend OStream & operator<<(OStream & os, const Point<2, T> & c) {
        os << "{" << c.x << "," << c.y << "}";
        return os;
    }

    Distance operator-(const Point<2, T> & rhs) const {
        T xx = rhs.x - x;
        T yy = rhs.y - y;
        //return Math::sqrt(xx*xx + yy*yy); //TODO: fix when multiplication works on eMote3
        return Math::abs(xx) + Math::abs(yy);
    }

    bool operator==(const Point<2, T>& rhs) { return x == rhs.x and y == rhs.y; }
    bool operator!=(const Point<2, T>& rhs){ return !(*this == rhs); }
};

template<typename C = Point<>, typename R = typename C::Distance>
struct Sphere {
    Sphere() { }
    Sphere(const C & c, const R & r = 0) : center(c), radius(r) { }

    bool contains(const C & coord) const {
        return center - coord <= radius;
    }
    friend Debug & operator<<(Debug & db, const Sphere & s) {
        db << "c=" << s.center << ",r=" << s.radius;
        return db;
    }
    friend OStream & operator<<(OStream & os, const Sphere & s) {
        os << "c=" << s.center << ",r=" << s.radius;
        return os;
    }

    C center;
    R radius;
};

__END_UTIL

