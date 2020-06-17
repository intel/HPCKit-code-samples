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

#include "GSimulation.hpp"
#include <CL/sycl.hpp>
#include <chrono>
using namespace sycl;

auto exception_handler = [](exception_list list) {
  for (auto& excep_ptr : list) {
    try {
      std::rethrow_exception(excep_ptr);
    } catch (exception& e) {
      std::cout << "Asynchronous Exception caught: " << e.what() << "\n";
    }
    std::terminate();
  }
};

GSimulation ::GSimulation() {
  std::cout << "===============================" << std::endl;
  std::cout << " Initialize Gravity Simulation" << std::endl;
  set_npart(16000);
  set_nsteps(10);
  set_tstep(0.1);
  set_sfreq(1);
}

void GSimulation ::set_number_of_particles(int N) { set_npart(N); }

void GSimulation ::set_number_of_steps(int N) { set_nsteps(N); }

void GSimulation ::init_pos() {
  std::random_device rd;  // random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(0, 1.0);

  for (int i = 0; i < get_npart(); ++i) {
    particles[i].pos[0] = unif_d(gen);
    particles[i].pos[1] = unif_d(gen);
    particles[i].pos[2] = unif_d(gen);
  }
}

void GSimulation ::init_vel() {
  std::random_device rd;  // random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(-1.0, 1.0);

  for (int i = 0; i < get_npart(); ++i) {
    particles[i].vel[0] = unif_d(gen) * 1.0e-3f;
    particles[i].vel[1] = unif_d(gen) * 1.0e-3f;
    particles[i].vel[2] = unif_d(gen) * 1.0e-3f;
  }
}

void GSimulation ::init_acc() {
  for (int i = 0; i < get_npart(); ++i) {
    particles[i].acc[0] = 0.f;
    particles[i].acc[1] = 0.f;
    particles[i].acc[2] = 0.f;
  }
}

void GSimulation ::init_mass() {
  real_type n = static_cast<real_type>(get_npart());
  std::random_device rd;  // random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(0.0, 1.0);

  for (int i = 0; i < get_npart(); ++i) {
    particles[i].mass = n * unif_d(gen);
  }
}

void GSimulation ::start() {
  real_type dt = get_tstep();
  int n = get_npart();
  real_type* energy = new real_type[n];
  for (int i = 0; i < n; i++) energy[i] = 0.f;
  // allocate particles
  particles = new Particle[n];

  init_pos();
  init_vel();
  init_acc();
  init_mass();

  print_header();

  _totTime = 0.;

  const float softeningSquared = 1e-3f;
  // prevents explosion in the case the particles are really close to each other
  const float G = 6.67259e-11f;

  double nd = double(n);
  double gflops = 1e-9 * ((11. + 18.) * nd * nd + nd * 19.);
  double av = 0.0, dev = 0.0;
  int nf = 0;
  auto R = range<1>(n);
  // Create a queue to the selected device and enabled asynchronous exception
  // handling for that queue
  queue q(default_selector{}, exception_handler);
  // Create SYCL buffer for the Particle array of size "n"
  buffer pbuf(particles, R, {cl::sycl::property::buffer::use_host_ptr()});
  // Create SYCL buffer for the ener array
  buffer ebuf(energy, R, {cl::sycl::property::buffer::use_host_ptr()});

  auto t0 = std::chrono::system_clock::now();
  int nsteps = get_nsteps();
  for (int s = 1; s <= nsteps; ++s) {
    auto ts0 = std::chrono::system_clock::now();
    q.submit([&](handler& h) {
       auto p = pbuf.get_access<access::mode::read_write>(h);
       h.parallel_for(R, [=](id<1> i) {
         real_type acc0 = p[i].acc[0];
         real_type acc1 = p[i].acc[1];
         real_type acc2 = p[i].acc[2];
         for (int j = 0; j < n; j++) {
           real_type dx, dy, dz;
           real_type distanceSqr = 0.0;
           real_type distanceInv = 0.0;

           dx = p[j].pos[0] - p[i].pos[0];  // 1flop
           dy = p[j].pos[1] - p[i].pos[1];  // 1flop
           dz = p[j].pos[2] - p[i].pos[2];  // 1flop

           distanceSqr =
               dx * dx + dy * dy + dz * dz + softeningSquared;  // 6flops
           distanceInv = 1.0 / sycl::sqrt(distanceSqr);         // 1div+1sqrt

           acc0 += dx * G * p[j].mass * distanceInv * distanceInv *
                   distanceInv;  // 6flops
           acc1 += dy * G * p[j].mass * distanceInv * distanceInv *
                   distanceInv;  // 6flops
           acc2 += dz * G * p[j].mass * distanceInv * distanceInv *
                   distanceInv;  // 6flops
         }
         p[i].acc[0] = acc0;
         p[i].acc[1] = acc1;
         p[i].acc[2] = acc2;
       });
     })
        .wait_and_throw();
    q.submit([&](handler& h) {
       auto p = pbuf.get_access<access::mode::read_write>(h);
       auto e = ebuf.get_access<access::mode::read_write>(h);
       h.parallel_for(R, [=](id<1> i) {
         p[i].vel[0] += p[i].acc[0] * dt;  // 2flops
         p[i].vel[1] += p[i].acc[1] * dt;  // 2flops
         p[i].vel[2] += p[i].acc[2] * dt;  // 2flops

         p[i].pos[0] += p[i].vel[0] * dt;  // 2flops
         p[i].pos[1] += p[i].vel[1] * dt;  // 2flops
         p[i].pos[2] += p[i].vel[2] * dt;  // 2flops

         p[i].acc[0] = 0.;
         p[i].acc[1] = 0.;
         p[i].acc[2] = 0.;

         e[i] = p[i].mass *
                (p[i].vel[0] * p[i].vel[0] + p[i].vel[1] * p[i].vel[1] +
                 p[i].vel[2] * p[i].vel[2]);  // 7flops
       });
     })
        .wait_and_throw();
    q.submit([&](handler& h) {
        auto e = ebuf.get_access<access::mode::read_write>(h);
        h.single_task([=]() {
        for(int i = 1; i < n; i++)
                e[0] += e[i];
        });
    })
    	.wait_and_throw();	
    auto a = ebuf.get_access<access::mode::read_write>();
    _kenergy = 0.5 * a[0];
    a[0] = 0;

    auto ts1 = std::chrono::system_clock::now();
    double elapsedseconds =
        (static_cast<std::chrono::duration<double>>(ts1 - ts0)).count();
    if (!(s % get_sfreq())) {
      nf += 1;
      std::cout << " " << std::left << std::setw(8) << s << std::left
                << std::setprecision(5) << std::setw(8) << s * get_tstep()
                << std::left << std::setprecision(5) << std::setw(12)
                << _kenergy << std::left << std::setprecision(5)
                << std::setw(12) << elapsedseconds << std::left
                << std::setprecision(5) << std::setw(12)
                << gflops * get_sfreq() / elapsedseconds << std::endl;
      if (nf > 2) {
        av += gflops * get_sfreq() / elapsedseconds;
        dev += gflops * get_sfreq() * gflops * get_sfreq() /
               (elapsedseconds * elapsedseconds);
      }
    }

  }  // end of the time step loop
  auto t1 = std::chrono::system_clock::now();
  _totTime = (static_cast<std::chrono::duration<double>>(t1 - t0)).count();
  _totFlops = gflops * get_nsteps();

  av /= (double)(nf - 2);
  dev = sqrt(dev / (double)(nf - 2) - av * av);

  int nthreads = 1;

  std::cout << std::endl;
  std::cout << "# Number Threads     : " << nthreads << std::endl;
  std::cout << "# Total Time (s)     : " << _totTime << std::endl;
  std::cout << "# Average Performance : " << av << " +- " << dev << std::endl;
  std::cout << "===============================" << std::endl;
}

void GSimulation ::print_header() {
  std::cout << " nPart = " << get_npart() << "; "
            << "nSteps = " << get_nsteps() << "; "
            << "dt = " << get_tstep() << std::endl;

  std::cout << "------------------------------------------------" << std::endl;
  std::cout << " " << std::left << std::setw(8) << "s" << std::left
            << std::setw(8) << "dt" << std::left << std::setw(12) << "kenergy"
            << std::left << std::setw(12) << "time (s)" << std::left
            << std::setw(12) << "GFlops" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
}

GSimulation ::~GSimulation() { delete particles; }
