#ifndef CALC_H
#define CALC_H

//*******************************************************
//                                                      
//       Calc Unit for SOLDAT                            
//                                                      
//       Copyright (c) 2001 Michal Marcinkowski          
//                                                      
//*******************************************************

#include "Vector.h"
#include <cmath>
#include <algorithm>
#include <cstddef>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Type definitions
struct TIntersectionResult {
    TVector2 Points[2];
    uint8_t NumIntersections;
};

// Helper function to check if value is in range
inline bool InRange(float value, float min, float max) {
    return value >= min && value <= max;
}

// Helper functions for math operations
inline float Sqr(float x) {
    return x * x;
}

// Additional required functions
inline float SqrDist(float X1, float Y1, float X2, float Y2) {
    return Sqr(X1 - X2) + Sqr(Y1 - Y2);
}

inline float SqrDist(TVector2 P1, TVector2 P2) {
    return Sqr(P1.x - P2.x) + Sqr(P1.y - P2.y);
}

// Function implementations
namespace CalcImpl {
    inline TIntersectionResult IsLineIntersectingCircle(TVector2 Line1, TVector2 Line2, TVector2 CircleCenter,
                                                        float Radius) {
        TIntersectionResult result = {};
        
        float diffx = Line2.x - Line1.x;
        float diffy = Line2.y - Line1.y;

        if (std::abs(diffx) < 0.00001f && std::abs(diffy) < 0.00001f) {
            // The line is a point!
            return result;
        }

        bool flipped = false;
        float temp;
        
        // if the angle of the bullet is bigger than 45 degrees,
        // flip the coordinate system.
        // This algorithm deals with lines being nearly horizontal just fine,
        // but nearly vertical would cause a havoc, as vertical line is not a function.
        if (std::abs(diffy) > std::abs(diffx)) {
            flipped = true;
            temp = Line1.x;
            Line1.x = Line1.y;
            Line1.y = temp;

            temp = Line2.x;
            Line2.x = Line2.y;
            Line2.y = temp;

            temp = CircleCenter.x;
            CircleCenter.x = CircleCenter.y;
            CircleCenter.y = temp;

            temp = diffx;
            diffx = diffy;
            diffy = temp;
        }

        // Line equation: ax + b - y = 0. given x1, y1, x2, y2, let's calculate a and b
        // a = (y1 - y2)/(x1 - x2)
        float a = diffy / diffx;
        // b := y - ax
        float b = Line1.y - a * Line1.x;
        // Circle equation: (x - x1)^2 + (y - y1)^2 - r^2 = 0
        // Now we need to solve: (x - x1)^2 + (y - y1)^2 - r^2 = ax + b - y
        // Simplyfing above: (a^2 + 1)x^2 + 2(ab − ay1 − x1)x + (y1^2 − r^2 + x1^2 − 2by1b^2)=0
        // now, since this is a standard Ax^2 + Bx + C equation, we find x and y using
        // x = (-B +/- sqrt(B^2 - 4ac))/(2A)
        // A = (a^2 + 1)
        float a1 = Sqr(a) + 1;
        // B = 2(ab - ay1 - x1)
        float b1 = 2 * (a * b - a * CircleCenter.y - CircleCenter.x);
        // C = y1^2 − r^2 + x1^2 − 2by1 + b^2
        float c1 = Sqr(CircleCenter.y) - Sqr(Radius) + Sqr(CircleCenter.x) - 2 * b * CircleCenter.y + Sqr(b);
        // delta = B^2 - 4AC;
        float delta = Sqr(b1) - 4 * a1 * c1;
        // having x1 and x2 result, we can calculate y1 and y2 from y = a * x + b

        // if delta < 0, no intersection
        if (delta < 0) {
            return result;
        }

        float minx, miny, maxx, maxy;
        if (Line1.x < Line2.x) {
            minx = Line1.x;
            maxx = Line2.x;
        } else {
            minx = Line2.x;
            maxx = Line1.x;
        }

        if (Line1.y < Line2.y) {
            miny = Line1.y;
            maxy = Line2.y;
        } else {
            miny = Line2.y;
            maxy = Line1.y;
        }

        // we don't care about a case of delta = 0 as it's extremaly rare,
        // also this will handle it fine, just less effecient
        float sqrtdelta = std::sqrt(delta);
        float a2 = 2 * a1;
        TVector2 Intersect;
        Intersect.x = (-b1 - sqrtdelta) / a2;
        Intersect.y = a * Intersect.x + b;
        // we know that infinite line does intersect the circle, now let's see if our part does
        if (InRange(Intersect.x, minx, maxx) && InRange(Intersect.y, miny, maxy)) {
            if (flipped) {
                temp = Intersect.x;
                Intersect.x = Intersect.y;
                Intersect.y = temp;
            }
            result.Points[result.NumIntersections] = Intersect;
            result.NumIntersections = result.NumIntersections + 1;
        }

        Intersect.x = (-b1 + sqrtdelta) / a2;
        Intersect.y = a * Intersect.x + b;
        if (InRange(Intersect.x, minx, maxx) && InRange(Intersect.y, miny, maxy)) {
            if (flipped) {
                temp = Intersect.x;
                Intersect.x = Intersect.y;
                Intersect.y = temp;
            }
            result.Points[result.NumIntersections] = Intersect;
            result.NumIntersections = result.NumIntersections + 1;
        }

        return result;
    }

    inline bool LineCircleCollision(TVector2 StartPoint, TVector2 EndPoint, TVector2 CircleCenter,
                                   float Radius, TVector2& CollisionPoint) {
        float r2 = Sqr(Radius);

        if (SqrDist(StartPoint, CircleCenter) <= r2) {
            CollisionPoint = StartPoint;
            return true;
        }

        if (SqrDist(EndPoint, CircleCenter) <= r2) {
            CollisionPoint = EndPoint;
            return true;
        }

        TIntersectionResult IntersectionResult = IsLineIntersectingCircle(StartPoint, EndPoint, CircleCenter, Radius);
        if (IntersectionResult.NumIntersections > 0) {
            CollisionPoint = IntersectionResult.Points[0];
            if (IntersectionResult.NumIntersections == 2 &&
                SqrDist(IntersectionResult.Points[0], StartPoint) > SqrDist(IntersectionResult.Points[1], StartPoint)) {
                CollisionPoint = IntersectionResult.Points[1];
            }
            return true;
        }
        
        return false;
    }

    inline float PointLineDistance(TVector2 P1, TVector2 P2, TVector2 P3) {
        float U = ((P3.x - P1.x) * (P2.x - P1.x) + (P3.y - P1.y) *
                  (P2.y - P1.y)) / (Sqr(P2.x - P1.x) + Sqr(P2.y - P1.y));

        float X = P1.x + U * (P2.x - P1.x);
        float Y = P1.y + U * (P2.y - P1.y);

        return std::sqrt(Sqr(X - P3.x) + Sqr(Y - P3.y));
    }

    inline float Angle2Points(TVector2 P1, TVector2 P2) {
        float result;
        if ((P2.x - P1.x) != 0) {
            if (P1.x > P2.x) {
                result = std::atan((P2.y - P1.y) / (P2.x - P1.x)) + M_PI;
            } else {
                result = std::atan((P2.y - P1.y) / (P2.x - P1.x));
            }
        } else {
            if (P2.y > P1.y) {
                result = M_PI / 2;
            } else if (P2.y < P1.y) {
                result = -M_PI / 2;
            } else {
                result = 0;
            }
        }
        return result;
    }

    inline float Distance(float X1, float Y1, float X2, float Y2) {
        return std::sqrt(Sqr(X1 - X2) + Sqr(Y1 - Y2));
    }

    inline float Distance(TVector2 P1, TVector2 P2) {
        return std::sqrt(Sqr(P1.x - P2.x) + Sqr(P1.y - P2.y));
    }

    inline float SqrDist(float X1, float Y1, float X2, float Y2) {
        return Sqr(X1 - X2) + Sqr(Y1 - Y2);
    }

    inline float SqrDist(TVector2 P1, TVector2 P2) {
        return Sqr(P1.x - P2.x) + Sqr(P1.y - P2.y);
    }

    inline int GreaterPowerOf2(int N) {
        if (N <= 1) return 1;
        // Find the next power of 2
        int result = 1;
        while (result < N) {
            result <<= 1;
        }
        return result;
    }

    // Rounds, but witout that "Banker's rule" that prefers even numbers.
    inline int RoundFair(float Value) {
        return static_cast<int>(std::floor(Value + 0.5f));
    }
}

// Using declarations to bring the functions into the global scope
using CalcImpl::IsLineIntersectingCircle;
using CalcImpl::LineCircleCollision;
using CalcImpl::PointLineDistance;
using CalcImpl::Angle2Points;
using CalcImpl::Distance;
using CalcImpl::GreaterPowerOf2;
using CalcImpl::RoundFair;

#endif // CALC_H