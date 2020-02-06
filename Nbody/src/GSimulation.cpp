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

#include<CL/sycl.hpp>
#include "GSimulation.hpp"
#include<chrono>
using namespace cl::sycl;

auto exception_handler = [](exception_list list){
        for(auto &excep_ptr : list){
                try{
                        std::rethrow_exception(excep_ptr);
                } catch (exception &e){
                        std::cout<<"Asynchronous Exception caught: "<<e.what()<<"\n";
                }
                std::terminate();
        }
};

GSimulation :: GSimulation()
{
  std::cout << "===============================" << std::endl;
  std::cout << " Initialize Gravity Simulation" << std::endl;
  set_npart(16000);
  set_nsteps(10);
  set_tstep(0.1);
  set_sfreq(1);
}

void GSimulation :: set_number_of_particles(int N)
{
  set_npart(N);
}

void GSimulation :: set_number_of_steps(int N)
{
  set_nsteps(N);
}

void GSimulation :: init_pos()
{
  std::random_device rd;        //random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(0,1.0);

  for(int i=0; i<get_npart(); ++i)
  {
    particles[i].pos[0] = unif_d(gen);
    particles[i].pos[1] = unif_d(gen);
    particles[i].pos[2] = unif_d(gen);
  }
}

void GSimulation :: init_vel()
{
  std::random_device rd;        //random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(-1.0,1.0);

  for(int i=0; i<get_npart(); ++i)
  {
    particles[i].vel[0] = unif_d(gen) * 1.0e-3f;
    particles[i].vel[1] = unif_d(gen) * 1.0e-3f;
    particles[i].vel[2] = unif_d(gen) * 1.0e-3f;
  }
}

void GSimulation :: init_acc()
{
  for(int i=0; i<get_npart(); ++i)
  {
    particles[i].acc[0] = 0.f;
    particles[i].acc[1] = 0.f;
    particles[i].acc[2] = 0.f;
  }
}

void GSimulation :: init_mass()
{
  real_type n   = static_cast<real_type> (get_npart());
  std::random_device rd;        //random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(0.0,1.0);

  for(int i=0; i<get_npart(); ++i)
  {
    particles[i].mass = n * unif_d(gen);
  }
}

void GSimulation :: start()
{
  real_type dt = get_tstep();
  int n = get_npart();
  real_type *energy = new real_type[n];
  for(int i = 0; i < n; i++)
        energy[i] = 0.f;
  //allocate particles
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
  double gflops = 1e-9 * ( (11. + 18. ) * nd*nd  +  nd * 19. );
  double av=0.0, dev=0.0;
  int nf = 0;

  default_selector defaultSelector;
  queue defaultQueue(defaultSelector, exception_handler);

  buffer<Particle, 1> buf(particles, range<1>(n), {cl::sycl::property::buffer::use_host_ptr()});
  buffer<real_type, 1> ener(energy, range<1>(n), {cl::sycl::property::buffer::use_host_ptr()});

  auto t0 = std::chrono::system_clock::now();
  int nsteps = get_nsteps();
  for (int s=1; s<=nsteps; ++s)
  {
    auto ts0 = std::chrono::system_clock::now();
    defaultQueue.submit([&](handler& cgh) {
    auto ptr = buf.get_access<access::mode::read_write>(cgh);
    cgh.parallel_for<class kernel1>(range<1>(n), [=](id<1> index) {
      int i = index.get(0);
      real_type acc0 = ptr[i].acc[0];
      real_type acc1 = ptr[i].acc[1];
      real_type acc2 = ptr[i].acc[2];
      for (int j = 0; j < n; j++)
      {
          real_type dx, dy, dz;
          real_type distanceSqr = 0.0;
          real_type distanceInv = 0.0;

          dx = ptr[j].pos[0] - ptr[i].pos[0];   //1flop
          dy = ptr[j].pos[1] - ptr[i].pos[1];   //1flop
          dz = ptr[j].pos[2] - ptr[i].pos[2];   //1flop

          distanceSqr = dx*dx + dy*dy + dz*dz + softeningSquared;       //6flops
          distanceInv = 1.0 / cl::sycl::sqrt(distanceSqr);                        //1div+1sqrt


          acc0 += dx * G * ptr[j].mass * distanceInv * distanceInv * distanceInv;       //6flops
          acc1 += dy * G * ptr[j].mass * distanceInv * distanceInv * distanceInv;       //6flops
          acc2 += dz * G * ptr[j].mass * distanceInv * distanceInv * distanceInv;       //6flops

      }
      ptr[i].acc[0] = acc0;
      ptr[i].acc[1] = acc1;
      ptr[i].acc[2] = acc2;
    });
    }).wait_and_throw();
    defaultQueue.submit([&](handler& cgh) {
    auto ptr = buf.get_access<access::mode::read_write>(cgh);
    auto ptr_ener = ener.get_access<access::mode::read_write>(cgh);
    cgh.parallel_for<class kernel2>(range<1>(n), [=](id<1> index) {
      int i = index.get(0);
      ptr[i].vel[0] += ptr[i].acc[0] * dt;      //2flops
      ptr[i].vel[1] += ptr[i].acc[1] * dt;      //2flops
      ptr[i].vel[2] += ptr[i].acc[2] * dt;      //2flops

      ptr[i].pos[0] += ptr[i].vel[0] * dt;      //2flops
      ptr[i].pos[1] += ptr[i].vel[1] * dt;      //2flops
      ptr[i].pos[2] += ptr[i].vel[2] * dt;      //2flops

      ptr[i].acc[0] = 0.;
      ptr[i].acc[1] = 0.;
      ptr[i].acc[2] = 0.;

      ptr_ener[i] = ptr[i].mass * (
                ptr[i].vel[0]*ptr[i].vel[0] +
                ptr[i].vel[1]*ptr[i].vel[1] +
                ptr[i].vel[2]*ptr[i].vel[2]); //7flops
    });
    }).wait_and_throw();

    defaultQueue.submit([&](handler& cgh) {
        auto ptr_ener = ener.get_access<access::mode::read_write>(cgh);
        cgh.single_task<class kernel3>([=]() {
        for(int i = 1; i < n; i++)
                ptr_ener[0] += ptr_ener[i];
        });
    }).wait_and_throw();
    auto host_ptr_ener = ener.get_access<access::mode::read_write>();
    _kenergy = 0.5 * host_ptr_ener[0];
    host_ptr_ener[0] = 0;

    auto ts1 = std::chrono::system_clock::now();
    double elapsedseconds = (static_cast<std::chrono::duration<double>>(ts1 - ts0)).count();
    if(!(s%get_sfreq()) )
    {
      nf += 1;
      std::cout << " "
                <<  std::left << std::setw(8)  << s
                <<  std::left << std::setprecision(5) << std::setw(8)  << s*get_tstep()
                <<  std::left << std::setprecision(5) << std::setw(12) << _kenergy
                <<  std::left << std::setprecision(5) << std::setw(12) << elapsedseconds
                <<  std::left << std::setprecision(5) << std::setw(12) << gflops*get_sfreq()/elapsedseconds
                <<  std::endl;
      if(nf > 2)
      {
        av  += gflops*get_sfreq()/elapsedseconds;
        dev += gflops*get_sfreq()*gflops*get_sfreq()/(elapsedseconds*elapsedseconds);
      }

    }

  } //end of the time step loop
  auto t1 = std::chrono::system_clock::now();
  _totTime  = (static_cast<std::chrono::duration<double>>(t1-t0)).count();
  _totFlops = gflops*get_nsteps();

  av/=(double)(nf-2);
  dev= cl::sycl::sqrt(dev/(double)(nf-2)-av*av);

  int nthreads=1;

  std::cout << std::endl;
  std::cout << "# Number Threads     : " << nthreads << std::endl;
  std::cout << "# Total Time (s)     : " << _totTime << std::endl;
  std::cout << "# Average Performance : " << av << " +- " <<  dev << std::endl;
  std::cout << "===============================" << std::endl;

}


void GSimulation :: print_header()
{

  std::cout << " nPart = " << get_npart()  << "; "
            << "nSteps = " << get_nsteps() << "; "
            << "dt = "     << get_tstep()  << std::endl;

  std::cout << "------------------------------------------------" << std::endl;
  std::cout << " "
            <<  std::left << std::setw(8)  << "s"
            <<  std::left << std::setw(8)  << "dt"
            <<  std::left << std::setw(12) << "kenergy"
            <<  std::left << std::setw(12) << "time (s)"
            <<  std::left << std::setw(12) << "GFlops"
            <<  std::endl;
  std::cout << "------------------------------------------------" << std::endl;


}

GSimulation :: ~GSimulation()
{
  delete particles;
}
