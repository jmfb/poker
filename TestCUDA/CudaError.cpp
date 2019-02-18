#include "pch.h"
#include "CudaError.h"

CudaError::CudaError(const char* function, const char* location, cudaError_t error)
	: runtime_error{ Format(function, location, error) }
{
}

CudaError::CudaError(const char* function, const char* location, CUresult error)
	: runtime_error{ Format(function, location, error) }
{
}

string CudaError::Format(const char* function, const char* location, cudaError_t error)
{
	ostringstream out;
	out << function << "@" << location << " failed (" << error << ","
		<< ::cudaGetErrorName(error) << "): " << ::cudaGetErrorString(error);
	return out.str();
}

string CudaError::Format(const char* function, const char* location, CUresult error)
{
	ostringstream out;
	out << function << "@" << location << " failed (" << error << ",";

	const char* name = nullptr;
	if (::cuGetErrorName(error, &name) == CUDA_SUCCESS)
		out << name;
	else
		out << "cuGetErrorName failed";
	out << "): ";

	const char* description = nullptr;
	if (::cuGetErrorString(error, &description) == CUDA_SUCCESS)
		out << description;
	else
		out << "cuGetErrorString failed";

	return out.str();
}

void CudaError::Check(const char* function, const char* location, cudaError_t error)
{
	if (error != cudaSuccess)
		throw CudaError{ function, location, error };
}

void CudaError::Check(const char* function, const char* location, CUresult error)
{
	if (error != CUDA_SUCCESS)
		throw CudaError{ function, location, error };
}
