//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_VEC2_H
#define COMP4300_ASSIGNMENT3_VEC2_H

#include <cmath>

class Vec2 {
private:
    float m_x;
    float m_y;
public:
    Vec2();
    Vec2(double x, double y);

    float x() const;
    float y() const;

    void setX(float x);
    void setY(float y);

    bool operator==(const Vec2& that) const;
    bool operator!=(const Vec2& that) const;
    Vec2 operator+(const Vec2& that) const;
    Vec2 operator-(const Vec2& that) const;
    Vec2 operator-() const;
    Vec2 operator+=(const Vec2& that);
    Vec2 operator*(double scalar) const;
    Vec2 operator/(double scalar) const;

    void normalize();
    float length() const;
};


#endif //COMP4300_ASSIGNMENT3_VEC2_H
