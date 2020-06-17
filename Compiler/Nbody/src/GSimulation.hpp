//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

// =============================================================
// Copyright (c) 2019 Fabio Baruffa
// Source: https://github.com/fbaru-dev/particle-sim
// MIT License
// =============================================================

#ifndef _GSIMULATION_HPP
#define _GSIMULATION_HPP

#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include <CL/sycl.hpp>
#include "Particle.hpp"

class GSimulation {
 public:
  GSimulation();
  ~GSimulation();

  void init();
  void set_number_of_particles(int N);
  void set_number_of_steps(int N);
  void start();

 private:
  Particle *particles;

  int _npart;        // number of particles
  int _nsteps;       // number of integration steps
  real_type _tstep;  // time step of the simulation

  int _sfreq;  // sample frequency

  real_type _kenergy;  // kinetic energy

  double _totTime;   // total time of the simulation
  double _totFlops;  // total number of flops

  void init_pos();
  void init_vel();
  void init_acc();
  void init_mass();

  inline void set_npart(const int &N) { _npart = N; }
  inline int get_npart() const { return _npart; }

  inline void set_tstep(const real_type &dt) { _tstep = dt; }
  inline real_type get_tstep() const { return _tstep; }

  inline void set_nsteps(const int &n) { _nsteps = n; }
  inline int get_nsteps() const { return _nsteps; }

  inline void set_sfreq(const int &sf) { _sfreq = sf; }
  inline int get_sfreq() const { return _sfreq; }

  void print_header();
};

#endif
