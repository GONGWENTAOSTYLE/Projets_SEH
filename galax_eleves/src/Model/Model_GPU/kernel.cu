#ifdef GALAX_MODEL_GPU

#include "cuda.h"
#include "kernel.cuh"
#include <mipp.h>
#define DIFF_T (0.1f)
#define EPS (1.0f)

inline __host__ __device__ float3 sub(float3 a, float3 b)
{
    return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline __host__ __device__ float3 add(float3 a, float3 b)
{
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}
 
inline __host__ __device__ float3 multi1(float3 a, float3 b)
{
    return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline __host__ __device__ float3 multi2(float3 a, float b)
{
    return make_float3(a.x * b, a.y * b, a.z * b);
}

__global__ void compute_acc(float3 * positionsGPU, float3 * velocitiesGPU, float3 * accelerationsGPU, float* massesGPU, int n_particles)
{
	unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

	for (int j = 0; j < n_particles; j++)
	{
		if(i != j)
		{
			const float3 diff = sub(positionsGPU[j] , positionsGPU[i]);

			float3 dij3 = multi1(diff,diff);
			float dij = dij3.x + dij3.y + dij3.z;
			
			if (dij < 1.0)
			{
				dij = 10.0;
			}
			else
			{
				dij = std::sqrt(dij);
				dij = 10.0 / (dij * dij * dij);
			}
			float3 n = multi2(diff, dij);
			float3 m = multi2(n,massesGPU[j]);
			accelerationsGPU[i] = add(accelerationsGPU[i],m);
			
		}
	}
	/*
	const mipp::Reg<float> rpos_i = &positionsGPU[i];
    mipp::Reg<float> racc_i = &accelerationsGPU[i];
	const mipp::Reg<float> value =1.0;
    const mipp::Reg<float> zero =0.0;
    const mipp::Reg<float> G =10.0;	
    using T = float;
	constexpr int N = mipp::N<T>();
    auto vecLoopSize = (n_particles / N)* N;
	for (int j = 0; j < vecLoopSize; j += N)
	{
		const mipp::Reg<float3> rpos_j = &positionsGPU[j];
		mipp::Reg<float> masses_j = &massesGPU[j];
		if(i != j)
		{
 
			const mipp::Reg<float3>  diff = sub(rpos_j, rpos_i);
			const mipp::Reg<float3> dij3 = multi1(diff,diff);
			const mipp::Reg<float> dij = dij3.x + dij3.y + dij3.z;
			mipp::Msk<N> msk = (dij < value);
			dij = mipp::mask<float, mipp::mul>(msk, dij, dij, zero);
			dij = mipp::mask<float, mipp::add>(msk, dij, dij, value);
			dij = mipp::sqrt(dij);

			dij = G / (dij * dij * dij);
			
			float3 n = multi2(diff, dij);
			float3 m = multi2(n,masses_j);
			racc_i= add(racc_i,m);
			
		}
	}
	racc_i.store(&accelerationsGPU[i]);
	*/
}

__global__ void maj_pos(float3 * positionsGPU, float3 * velocitiesGPU, float3 * accelerationsGPU)
{
	unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
	velocitiesGPU[i] = add(velocitiesGPU[i], multi2(accelerationsGPU[i], 2.0f));
	positionsGPU[i] = add(positionsGPU[i], multi2(velocitiesGPU[i], 0.1f));

}

void update_position_cu(float3* positionsGPU, float3* velocitiesGPU, float3* accelerationsGPU, float* massesGPU, int n_particles)
{
	int nthreads = 128;
	int nblocks =  (n_particles + (nthreads -1)) / nthreads;

	compute_acc<<<nblocks, nthreads>>>(positionsGPU, velocitiesGPU, accelerationsGPU, massesGPU, n_particles);
	maj_pos    <<<nblocks, nthreads>>>(positionsGPU, velocitiesGPU, accelerationsGPU);
}


#endif // GALAX_MODEL_GPU