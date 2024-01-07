//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Vec2.h"

Vec2::Vec2(double x, double y) : m_x(x), m_y(y) {}

bool Vec2::operator==(const Vec2 &that) const {
    return this->m_x == that.m_x && this->m_y == that.m_y;
}

bool Vec2::operator!=(const Vec2 &that) const {
    return !(*this == that);
}

Vec2 Vec2::operator+(const Vec2 &that) const {
    return {this->m_x + that.m_x, this->m_y + that.m_y};
}

Vec2 Vec2::operator-(const Vec2 &that) const {
    return *this + -that;
}

Vec2 Vec2::operator-() const {
    return {-this->m_x, -this->m_y};
}

Vec2 Vec2::operator*(double scalar) const {
    return {this->m_x * scalar, this->m_y * scalar};
}

Vec2 Vec2::operator/(double scalar) const {
    return *this * (1 / scalar);
}

float Vec2::length() const {
    return std::sqrt(this->m_x * this->m_x + this->m_y * this->m_y);
}

void Vec2::normalize() {
    double length = this->length();
    *this = *this / length;
}

Vec2::Vec2()
    : Vec2(0, 0)
{

}

float Vec2::x() const {
    return m_x;
}

float Vec2::y() const {
    return m_y;
}

void Vec2::setX(float x) {
    m_x = x;
}

void Vec2::setY(float y) {
    m_y = y;
}

Vec2 Vec2::operator+=(const Vec2 &that) {
    *this = *this + that;
    return *this;
}



