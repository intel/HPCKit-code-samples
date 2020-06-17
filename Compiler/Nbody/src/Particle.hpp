#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP
#include <cmath>
#include "type.hpp"

struct Particle {
 public:
  Particle() { init(); }
  void init() {
    pos[0] = 0.;
    pos[1] = 0.;
    pos[2] = 0.;
    vel[0] = 0.;
    vel[1] = 0.;
    vel[2] = 0.;
    acc[0] = 0.;
    acc[1] = 0.;
    acc[2] = 0.;
    mass = 0.;
  }
  real_type pos[3];
  real_type vel[3];
  real_type acc[3];
  real_type mass;
};

#endif
