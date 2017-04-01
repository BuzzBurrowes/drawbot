#ifdef ARDUINO
#include <Streaming.h>
#endif
#include "fixedpoint.h"


template<>
SignedFixedPoint<16>& SignedFixedPoint<16>::operator/=(const SignedFixedPoint<16>& rhs) {
   value = (value << 8) / (rhs.value >> 8) ;
   return *this;
}

template<>
SignedFixedPoint<15>& SignedFixedPoint<15>::operator/=(const SignedFixedPoint<15>& rhs) {
   value = (value << 7) / (rhs.value >> 8) ;
   return *this;
}

template<>
SignedFixedPoint<14>& SignedFixedPoint<14>::operator/=(const SignedFixedPoint<14>& rhs) {
   value = (value << 7) / (rhs.value >> 7) ;
   return *this;
}

template<>
SignedFixedPoint<13>& SignedFixedPoint<13>::operator/=(const SignedFixedPoint<13>& rhs) {
   value = (value << 6) / (rhs.value >> 7) ;
   return *this;
}

template<>
SignedFixedPoint<12>& SignedFixedPoint<12>::operator/=(const SignedFixedPoint<12>& rhs) {
   value = (value << 6) / (rhs.value >> 6) ;
   return *this;
}

template<>
SignedFixedPoint<11>& SignedFixedPoint<11>::operator/=(const SignedFixedPoint<11>& rhs) {
   value = (value << 5) / (rhs.value >> 6) ;
   return *this;
}

template<>
SignedFixedPoint<10>& SignedFixedPoint<10>::operator/=(const SignedFixedPoint<10>& rhs) {
   value = (value << 5) / (rhs.value >> 5) ;
   return *this;
}

template<>
SignedFixedPoint<9>& SignedFixedPoint<9>::operator/=(const SignedFixedPoint<9>& rhs) {
   value = (value << 4) / (rhs.value >> 5) ;
   return *this;
}

template<>
SignedFixedPoint<8>& SignedFixedPoint<8>::operator/=(const SignedFixedPoint<8>& rhs) {
   value = (value << 4) / (rhs.value >> 4) ;
   return *this;
}

template<>
SignedFixedPoint<7>& SignedFixedPoint<7>::operator/=(const SignedFixedPoint<7>& rhs) {
   value = (value << 3) / (rhs.value >> 4) ;
   return *this;
}

