#pragma once
#include <stdint.h>

template <int F>
class SignedFixedPoint {
public:
   SignedFixedPoint() : value(0) {
   }

   SignedFixedPoint(const SignedFixedPoint<F>& other) {
      //Serial << " COPYING! " << endl;
      value = other.value;
   }
   SignedFixedPoint(float v) {
      value = (int32_t)(v * (float)(1L<<F));
   }
   SignedFixedPoint(int32_t initialValue) {
      value = initialValue << F;
   }
   SignedFixedPoint(int16_t initialValue) {
      value = int32_t(initialValue) << F;
   }
   static SignedFixedPoint<F> FromRaw(int32_t r) {
      SignedFixedPoint<F> result;
      result.value = r;
      return result;
   }

   explicit operator int32_t() const {
      //Serial << " CONVERTING! " << endl;
      return RoundToInt();
   }

   int32_t Truncated() const {
      return value >> F;
   }

   SignedFixedPoint<F>& operator=(const SignedFixedPoint<F>& other) {
      value = other.value;
      return *this;
   }
   SignedFixedPoint<F>& operator=(float v) {
      value = (int32_t)(v * (float)(1L<<F));
      return *this;
   }
   SignedFixedPoint<F>& operator=(int32_t v) {
      value = v << F;
      return *this;
   }
   /* SignedFixedPoint<F>& operator=(int v) {
      value = (int32_t)v << F;
      return *this;
   } */
   SignedFixedPoint<F>& operator=(int16_t v) {
      value = (int32_t)v << F;
      return *this;
   }
   SignedFixedPoint<F>& operator=(int8_t v) {
      value = (int32_t)v << F;
      return *this;
   }

   SignedFixedPoint<F>& operator++() {
      value += (1L << F);
      return *this;
   }
   SignedFixedPoint<F> operator++(int) {
      SignedFixedPoint<F> tmp = *this;
      operator++();
      return tmp;
   }
   SignedFixedPoint<F>& operator--() {
      value -= (1L << F);
      return *this;
   }
   SignedFixedPoint<F> operator--(int) {
      SignedFixedPoint<F> tmp = *this;
      operator--();
      return tmp;
   }

   SignedFixedPoint<F>& operator+=(const SignedFixedPoint<F>& rhs) {
      value += rhs.value;
      return *this;
   }
   SignedFixedPoint<F>& operator-=(const SignedFixedPoint<F>& rhs) {
      //Serial << "my value: " << _HEX(value) << " rhs value: " << _HEX((int32_t)rhs.value);
      value -= rhs.value;
      //Serial << " my new value: " << _HEX(value) << endl;
      return *this;
   }
   SignedFixedPoint<F>& operator*=(const SignedFixedPoint<F>& rhs) {
      value *= rhs.value;
      value >>= F;
      return *this;
   }
   SignedFixedPoint<F>& operator/=(const SignedFixedPoint<F>& rhs) {
      value = (value << F) / rhs.value;
      return *this;
   }
   SignedFixedPoint<F> operator+(const SignedFixedPoint<F>& rhs) {
      SignedFixedPoint<F> result = *this;
      result += rhs;
      return result;
   }
   SignedFixedPoint<F> operator-(const SignedFixedPoint<F>& rhs) {
      SignedFixedPoint<F> result = *this;
      result -= rhs;
      return result;
   }
   SignedFixedPoint<F> operator*(const SignedFixedPoint<F>& rhs) {
      SignedFixedPoint<F> result = *this;
      result *= rhs;
      return result;
   }
   SignedFixedPoint<F> operator/(const SignedFixedPoint<F>& rhs) {
      SignedFixedPoint<F> result = *this;
      result /= rhs;
      return result;
   }
   bool operator==(const SignedFixedPoint<F>& rhs) {
      return value == rhs.value;
   }
   bool operator!=(const SignedFixedPoint<F>& rhs) {
      return !operator==(rhs);
   }
   bool operator<(const SignedFixedPoint<F>& rhs) {
      return value < rhs.value;
   }
   bool operator<=(const SignedFixedPoint<F>& rhs) {
      return value <= rhs.value;
   }
   bool operator>(const SignedFixedPoint<F>& rhs) {
      return value > rhs.value;
   }
   bool operator>=(const SignedFixedPoint<F>& rhs) {
      return value >= rhs.value;
   }
   bool AlmostEquals(const SignedFixedPoint<F>& other) const {
      int32_t diff = value - other.value;
      diff *= (diff < 0) ? -1 : 1;
      return diff < 2;
   }

   SignedFixedPoint<F>& operator+=(int32_t rhs) {
      value += rhs << F;
      return *this;
   }
   SignedFixedPoint<F>& operator-=(int32_t rhs) {
      //Serial << " MMM " << _HEX(value) << " rhs " << _HEX(rhs << F);
      value -= rhs << F;
      return *this;
   }
   SignedFixedPoint<F>& operator*=(int32_t rhs) {
      value *= rhs;
      return *this;
   }
   SignedFixedPoint<F>& operator/=(int32_t rhs) {
      value /= rhs;
      return *this;
   }
   SignedFixedPoint<F> operator+(int32_t rhs) {
      SignedFixedPoint<F> result = *this;
      result += rhs;
      return result;
   }
   SignedFixedPoint<F> operator-(int32_t rhs) {
      SignedFixedPoint<F> result = *this;
      //Serial << "rhs int: " << rhs << " value " << _HEX(value) <<" res value: " << _HEX((int32_t)result) << " ";
      result -= rhs;
      //Serial << endl;
      return result;
   }
   SignedFixedPoint<F> operator*(int32_t rhs) {
      SignedFixedPoint<F> result = *this;
      result *= rhs;
      return result;
   }
   SignedFixedPoint<F> operator/(int32_t rhs) {
      SignedFixedPoint<F> result = *this;
      result /= rhs;
      return result;
   }
   bool operator==(int32_t rhs) {
      return value == rhs << F;
   }
   bool operator!=(int32_t rhs) {
      return !operator==(rhs);
   }
   bool operator<(int32_t rhs) {
      return value < rhs << F;
   }
   bool operator<=(int32_t rhs) {
      return value <= rhs << F;
   }
   bool operator>(int32_t rhs) {
      return value > rhs << F;
   }
   bool operator>=(int32_t rhs) {
      return value >= rhs << F;
   }
   int32_t RoundToInt() const {
      if (((uint32_t)value & MantissaMask()) == Point5()) {
         if (value < 0)
            return (value >> F);
         return (value >> F) + 1;
      }
      return (value + (int32_t)Point5()) >> F;
   }

   SignedFixedPoint<F>& operator+=(int16_t rhs) {
      value += int32_t(rhs) << F;
      return *this;
   }
   SignedFixedPoint<F>& operator-=(int16_t rhs) {
      value -= int32_t(rhs) << F;
      return *this;
   }
   SignedFixedPoint<F>& operator*=(int16_t rhs) {
      value *= int32_t(rhs);
      return *this;
   }
   SignedFixedPoint<F>& operator/=(int16_t rhs) {
      value /= int32_t(rhs);
      return *this;
   }
   SignedFixedPoint<F> operator+(int16_t rhs) {
      SignedFixedPoint<F> result = *this;
      result += int32_t(rhs);
      return result;
   }
   SignedFixedPoint<F> operator-(int16_t rhs) {
      SignedFixedPoint<F> result = *this;
      result -= int32_t(rhs);
      return result;
   }
   SignedFixedPoint<F> operator*(int16_t rhs) {
      SignedFixedPoint<F> result = *this;
      result *= int32_t(rhs);
      return result;
   }
   SignedFixedPoint<F> operator/(int16_t rhs) {
      SignedFixedPoint<F> result = *this;
      result /= int32_t(rhs);
      return result;
   }
   bool operator==(int16_t rhs) {
      return value == int32_t(rhs) << F;
   }
   bool operator!=(int16_t rhs) {
      return !operator==(int32_t(rhs));
   }
   bool operator<(int16_t rhs) {
      return value < int32_t(rhs) << F;
   }
   bool operator<=(int16_t rhs) {
      return value <= int32_t(rhs) << F;
   }
   bool operator>(int16_t rhs) {
      return value > int32_t(rhs) << F;
   }
   bool operator>=(int16_t rhs) {
      return value >= int32_t(rhs) << F;
   }

   SignedFixedPoint<F>& operator+=(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      *this += rhsFp;
      return *this;
   }
   SignedFixedPoint<F>& operator-=(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      *this -= rhsFp;
      return *this;
   }
   SignedFixedPoint<F>& operator*=(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      *this *= rhsFp;
      return *this;
   }
   SignedFixedPoint<F>& operator/=(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      *this /= rhsFp;
      return *this;
   }
   SignedFixedPoint<F> operator+(float rhs) {
      SignedFixedPoint<F> result = rhs;
      result += *this;
      return result;
   }
   SignedFixedPoint<F> operator-(float rhs) {
      SignedFixedPoint<F> result = *this;
      SignedFixedPoint<F> rhsFp = rhs;
      result -= rhsFp;
      return result;
   }
   SignedFixedPoint<F> operator*(float rhs) {
      SignedFixedPoint<F> result = rhs;
      result *= *this;
      return result;
   }
   SignedFixedPoint<F> operator/(float rhs) {
      SignedFixedPoint<F> result = *this;
      SignedFixedPoint<F> rhsFp = rhs;
      result /= rhsFp;
      return result;
   }
   bool operator==(float rhs) {
      return this->AlmostEquals(rhs);
   }
   bool operator!=(float rhs) {
      return !this->AlmostEquals(rhs);
   }
   bool operator<(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      return *this < rhsFp;
   }
   bool operator<=(float rhs) {
      return this->AlmostEquals(rhs) || !operator>(rhs);
   }
   bool operator>(float rhs) {
      SignedFixedPoint<F> rhsFp = rhs;
      return *this > rhsFp;
   }
   bool operator>=(float rhs) {
      return this->AlmostEquals(rhs) || !operator<(rhs);
   }
   bool AlmostEquals(float other) const {
      SignedFixedPoint<F> otherFp = other;
      return AlmostEquals(otherFp);
   }
   float ToFloat() const {
      return (float)value / (float)(1L<<F);
   }

#ifdef ARDUINO
   friend
   Print &operator <<(Print &obj, const SignedFixedPoint<F>& fp) {
      float f = fp.ToFloat();
      obj.print(f);
      return obj;
   }
#endif // ARDUINO
private:
   int32_t value;
   constexpr static uint32_t MantissaMask() { return (1<<F)-1;                      }
   constexpr static uint32_t TopBitMask()   { return MantissaMask() << (31 - F);    }
   constexpr static uint32_t Point5()       { return 1<<(F-1);                      }
   constexpr static int32_t  HalfNumBitsL() { return (F>>1);                        }
   constexpr static int32_t  HalfNumBitsR() { return (F & 0x1) ? (F>>1)+1 : (F>>1); }
};
