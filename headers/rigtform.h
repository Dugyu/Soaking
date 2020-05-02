#ifndef RIGTFORM_H
#define RIGTFORM_H

#include <iostream>
#include <cassert>

#include "matrix4.h"
#include "quat.h"

class RigTForm {
  Cvec3 t_; // translation component
  Quat r_;  // rotation component represented as a quaternion

public:
  RigTForm() : t_(0) {
    assert(norm2(Quat(1,0,0,0) - r_) < CS175_EPS2);
  }

  RigTForm(const Cvec3& t, const Quat& r) {
      t_ = t;
      r_ = r;
  }

  explicit RigTForm(const Cvec3& t) {
     t_ = t;
     r_ = Quat();
  }

  explicit RigTForm(const Quat& r) {
     t_ = Cvec3(0);
     r_ = r;
  }

  Cvec3 getTranslation() const {
    return t_;
  }

  Quat getRotation() const {
    return r_;
  }

  RigTForm& setTranslation(const Cvec3& t) {
    t_ = t;
    return *this;
  }

  RigTForm& setRotation(const Quat& r) {
    r_ = r;
    return *this;
  }

  Cvec4 operator * (const Cvec4& a) const {
      //Cvec3 afterR = r_ * Cvec3(a[0],a[1],a[2]);
      //Cvec3 newT = t_ * a[3];  // * 0 if a[3] = 0(a is a vector), * 1 if a[3] = 1 (a is a point)
      //Cvec4 afterT = Cvec4(afterR + newT, a[3]);
      //return afterT;
      return Cvec4(t_, 1.0) * a[3] + Cvec4(r_ * Cvec3(a));
  }

  RigTForm operator * (const RigTForm& a) const {
      //Cvec3 newT = t_ + r_ * a.getTranslation();
      //Quat newR = r_ * a.getRotation();
      //RigTForm newRBT = RigTForm(newT, newR);
      //return RigTForm;
      return RigTForm(t_ + r_ * a.t_, r_ * a.r_);
  }
  static RigTForm lerp (const RigTForm & r0, const RigTForm & r1, const double alpha){
      // linear interpolation of translation component
      Cvec3 t = r0.getTranslation() * (1 - alpha) + r1.getTranslation() * alpha;
      // sphere linear interpolation of the rotation component
      const Quat q0 = r0.getRotation();
      const Quat q1 = r1.getRotation();
      Quat q = Quat::slerp(q0, q1, alpha);
      return RigTForm(t,q);
   }
    
   static RigTForm smoothLerp (const RigTForm & r0_, const RigTForm & r0,
                               const RigTForm & r1, const RigTForm & r1_,
                               const double t)
    {
        // Extract translation component
        const Cvec3 tr0_ = r0_.getTranslation();   // i-1
        const Cvec3 tr0 = r0.getTranslation();     // i
        const Cvec3 tr1 = r1.getTranslation();     // i+1
        const Cvec3 tr1_ = r1_.getTranslation();   // i+2
        
        // Catmull-Rom interpolation of translation component
        const Cvec3 d_tr = (tr1 - tr0_) / 6.0 + tr0;
        const Cvec3 e_tr = -(tr1_ - tr0) / 6.0 + tr1;
        const Cvec3 f_tr = tr0 * (1-t) + d_tr * t;
        const Cvec3 g_tr = d_tr * (1-t) + e_tr * t;
        const Cvec3 h_tr = e_tr * (1-t) + tr1 * t;
        const Cvec3 m_tr = f_tr * (1-t) + g_tr * t;
        const Cvec3 n_tr = g_tr * (1-t) + h_tr * t;
        const Cvec3 tr = m_tr * (1-t) + n_tr * t;   // final result
        
        // Extract translation component
        const Quat q0_ = r0_.getRotation();   // i-1
        const Quat q0 = r0.getRotation();     // i
        const Quat q1 = r1.getRotation();     // i+1
        const Quat q1_ = r1_.getRotation();   // i+2
        
        // Catmull-Rom interpolation of rotation component
        const Quat d_q = Quat::power(cn(q1 * inv(q0_)), 1.0/6.0) * q0;
        const Quat e_q = Quat::power(cn(q1_* inv(q0)), -1.0/6.0) * q1;
        const Quat f_q = Quat::slerp(q0,d_q,t);
        const Quat g_q = Quat::slerp(d_q,e_q,t);
        const Quat h_q = Quat::slerp(e_q,q1,t);
        const Quat m_q = Quat::slerp(f_q,g_q,t);
        const Quat n_q = Quat::slerp(g_q,h_q,t);
        const Quat q = Quat::slerp(m_q,n_q,t);

        return RigTForm(tr,q);
    }
    
};


inline RigTForm inv(const RigTForm& tform) {
    Quat r = tform.getRotation();
    Cvec3 t = tform.getTranslation();
    Cvec3 newT = - (inv(r) * t);
    Quat newR = inv(r);
    return RigTForm(newT, newR);
}

inline RigTForm transFact(const RigTForm& tform) {
  return RigTForm(tform.getTranslation());
}

inline RigTForm linFact(const RigTForm& tform) {
  return RigTForm(tform.getRotation());
}

inline Matrix4 rigTFormToMatrix(const RigTForm& tform) {
  //Matrix4 m = Matrix4();
  //Matrix4 mr = quatToMatrix(tform.getRotation());
  //Cvec3 t = tform.getTranslation();
    //m(0,3) = t[0];
    //m(1,3) = t[1];
    //m(2,3) = t[2];
    //m = m * mr;
  Matrix4 m = quatToMatrix(tform.getRotation());
  const Cvec3 t = tform.getTranslation();
    for (int i = 0; i < 3; ++i){
        m(i,3) = t(i);
    }
  return m;
}

#endif
