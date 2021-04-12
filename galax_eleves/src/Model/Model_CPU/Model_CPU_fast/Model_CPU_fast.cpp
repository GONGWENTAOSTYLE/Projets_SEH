#ifdef GALAX_MODEL_CPU_FAST
#include <iostream>
#include <cmath>

#include "Model_CPU_fast.hpp"

#include <mipp.h>
#include <omp.h>

Model_CPU_fast
::Model_CPU_fast(const Initstate& initstate, Particles& particles)
: Model_CPU(initstate, particles)
{
}

void Model_CPU_fast
::step()
{
    std::fill(accelerationsx.begin(), accelerationsx.end(), 0);
    std::fill(accelerationsy.begin(), accelerationsy.end(), 0);
    std::fill(accelerationsz.begin(), accelerationsz.end(), 0);


    //#OMP  version
/*     //#pragma omp parallel for
	for (int i = 0; i < n_particles; i++)
	{
		for (int j = 0; j < i; j++)
		{

			const float diffx = particles.x[j] - particles.x[i];
			const float diffy = particles.y[j] - particles.y[i];
			const float diffz = particles.z[j] - particles.z[i];

			float dij = diffx * diffx + diffy * diffy + diffz * diffz;

			if (dij < 1.0)
			{
				dij = 10.0;
			}
			else
			{
				dij = std::sqrt(dij);
				dij = 10.0 / (dij * dij * dij);
			}

			accelerationsx[i] += diffx * dij * initstate.masses[j];
			accelerationsy[i] += diffy * dij * initstate.masses[j];
			accelerationsz[i] += diffz * dij * initstate.masses[j];
			accelerationsx[j] -= diffx * dij * initstate.masses[i];
			accelerationsy[j] -= diffy * dij * initstate.masses[i];
			accelerationsz[j] -= diffz * dij * initstate.masses[i];
			
		}
	} */
/* 	#pragma omp parallel for
	for (int i = 0; i < n_particles; i++)
	{
		for (int j = 0; j < n_particles; j++)
		{
			if(i != j)
			{
				const float diffx = particles.x[j] - particles.x[i];
				const float diffy = particles.y[j] - particles.y[i];
				const float diffz = particles.z[j] - particles.z[i];

				float dij = diffx * diffx + diffy * diffy + diffz * diffz;
				
				if (dij < 1.0)
				{
					dij = 10.0;
				}
				else
				{
					dij = std::sqrt(dij);
					dij = 10.0 / (dij * dij * dij);
				}
				//std::cout <<"dij=" <<dij << std::endl;
				accelerationsx[i] += diffx * dij * initstate.masses[j];
				accelerationsy[i] += diffy * dij * initstate.masses[j];
				accelerationsz[i] += diffz * dij * initstate.masses[j];
			}
		}
	} */

// OMP + MIPP version
	const mipp::Reg<float> value =1.0;
    const mipp::Reg<float> zero =0.0;
    const mipp::Reg<float> G =10.0;	
    using T = float;
	constexpr int N = mipp::N<T>();
    auto vecLoopSize = (n_particles / N)* N;
	#pragma omp parallel for schedule (guided,15)
    for (int i = 0; i < vecLoopSize; i += N)
    {
        // load registers body i
        const mipp::Reg<float> rposx_i = &particles.x[i];
        const mipp::Reg<float> rposy_i = &particles.y[i];
        const mipp::Reg<float> rposz_i = &particles.z[i];
        mipp::Reg<float> raccx_i = &accelerationsx[i];
        mipp::Reg<float> raccy_i = &accelerationsy[i];
        mipp::Reg<float> raccz_i = &accelerationsz[i];
 
		for(int j = 0; j < vecLoopSize; j ++) 
        {
            const mipp::Reg<float> rposx_j = &particles.x[j];
            const mipp::Reg<float> rposy_j = &particles.y[j];
            const mipp::Reg<float> rposz_j = &particles.z[j];
            mipp::Reg<float> masses_j = &initstate.masses[j];
            if(i != j)
			{
				const mipp::Reg<float> diffx = rposx_j - rposx_i;
				const mipp::Reg<float> diffy = rposy_j - rposy_i;
				const mipp::Reg<float> diffz = rposz_j - rposz_i;

				mipp::Reg<float> dij = diffx * diffx + diffy * diffy + diffz * diffz;
                mipp::Msk<N> msk = (dij < value);
                dij = mipp::mask<float, mipp::mul>(msk, dij, dij, zero);
                dij = mipp::mask<float, mipp::add>(msk, dij, dij, value);
                dij = mipp::sqrt(dij);
                
                dij = G / (dij * dij * dij);

				raccx_i += diffx * dij * masses_j;
				raccy_i += diffy * dij * masses_j;
				raccz_i += diffz * dij * masses_j;
				
			}
		}
		raccx_i.store(&accelerationsx[i]);
		raccy_i.store(&accelerationsy[i]);
		raccz_i.store(&accelerationsz[i]);
    }
     for (int i = vecLoopSize; i < n_particles; i++) {

        for (int j = 0; j < n_particles; j++) {
            if(i != j)
			{
				const float diffx = particles.x[j] - particles.x[i];
				const float diffy = particles.y[j] - particles.y[i];
				const float diffz = particles.z[j] - particles.z[i];

				float dij = diffx * diffx + diffy * diffy + diffz * diffz;

				if (dij < 1.0)
				{
					dij = 10.0;
				}
				else
				{
					dij = std::sqrt(dij);
					dij = 10.0 / (dij * dij * dij);
				}

				accelerationsx[i] += diffx * dij * initstate.masses[j];
				accelerationsy[i] += diffy * dij * initstate.masses[j];
				accelerationsz[i] += diffz * dij * initstate.masses[j];
			}
		}
    
    }

    for (int i = 0; i < vecLoopSize; i++) {

        for (int j = vecLoopSize; j < n_particles; j++) {
            if(i != j)
			{
				const float diffx = particles.x[j] - particles.x[i];
				const float diffy = particles.y[j] - particles.y[i];
				const float diffz = particles.z[j] - particles.z[i];

				float dij = diffx * diffx + diffy * diffy + diffz * diffz;

				if (dij < 1.0)
				{
					dij = 10.0;
				}
				else
				{
					dij = std::sqrt(dij);
					dij = 10.0 / (dij * dij * dij);
				}

				accelerationsx[i] += diffx * dij * initstate.masses[j];
				accelerationsy[i] += diffy * dij * initstate.masses[j];
				accelerationsz[i] += diffz * dij * initstate.masses[j];
			}
		}
    
    } 

    for (int i = 0; i < n_particles; i++)
    {
        velocitiesx[i] += accelerationsx[i] * 2.0f;
        velocitiesy[i] += accelerationsy[i] * 2.0f;
        velocitiesz[i] += accelerationsz[i] * 2.0f;
        particles.x[i] += velocitiesx   [i] * 0.1f;
        particles.y[i] += velocitiesy   [i] * 0.1f;
        particles.z[i] += velocitiesz   [i] * 0.1f;
    }
}

#endif // GALAX_MODEL_CPU_FAST
