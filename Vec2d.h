#pragma once

template<typename T>
class Vec2d {
public:
   Vec2d() : mX(0), mY(0) {}
   Vec2d(T x, T y) : mX(x), mY(y) {}
   Vec2d(const Vec2d<T>& other) : mX(other.mX), mY(other.mY) { }
   void Set(T x, T y) { mX = x; mY = y; }

   T x() const { return mX; }
   T y() const { return mY; }
   T X() const { return mX; }
   T Y() const { return mY; }

   Vec2d<T>& operator=(const Vec2d<T>& other) {
      mX = other.mX; mY = other.mY;
      return *this;
   }

   Vec2d<T>& operator+=(const Vec2d<T>& other) {
      mX += other.mX; mY += other.mY;
      return *this;
   }

   friend Vec2d<T> operator+(const Vec2d<T> lhs, const Vec2d<T>& rhs) {
      lhs += rhs;
      return lhs;
   }

   Vec2d<T>& operator-=(const Vec2d<T>& other) {
      mX -= other.mX; mY -= other.mY;
      return *this;
   }

   friend Vec2d<T> operator-(Vec2d<T> lhs, const Vec2d<T>& rhs) {
      lhs -= rhs;
      return lhs;
   }

   friend Vec2d<T> operator*(Vec2d<T> lhs, const T& rhs) {
      lhs.mX *= rhs; lhs.mY *= rhs;
      return lhs;
   }

   friend Vec2d<T> operator/(Vec2d<T> lhs, const T& rhs) {
      lhs.mX /= rhs; lhs.mY /= rhs;
      return lhs;
   }

   T Magnitude() const {
      return sqrt(mX * mX + mY * mY);
   }

   void Normalize() { 
      T length = Magnitude();
      mX /= length; mY /= length;
   }

   Vec2d<T> Normalized() {
      Vec2d<T> normVec = *this;
      normVec.Normalize();
      return normVec;
   }

   T Dot(const Vec2d<T>& other) const {
      return mX * other.mX + mY * other.mY;
   } 

   T RadiansTo(const Vec2d<T>& other, T* dir = nullptr) const {
      Vec2d<T> myUnit    = Normalized();
      Vec2d<T> otherUnit = other.Normalized();
      T dot = acos(myUnit.Dot(otherUnit));
      if (dir) {
         *dir = myUnit.mX * otherUnit.mY - myUnit.mY * otherUnit.mX;
      }
      return dot;
   }

   void Rotate(T radians)
   {
      T cosR = cos(radians);
      T sinR = sin(radians);
      T newX = mX * cosR - mY * sinR;
      T newY = mY * cosR + mX * sinR;
      mX = newX;
      mY = newY;
   }

private:
   T mX;
   T mY;
};

template<> Vec2d<float>::Vec2d();
template<> Vec2d<double>::Vec2d();

typedef Vec2d<float>  Vec2dF;
typedef Vec2d<double> Vec2dD;