#pragma once

class CudaError : public runtime_error
{
public:
	CudaError(const char* function, const char* location, cudaError_t error);
	CudaError(const char* function, const char* location, CUresult error);
	CudaError(const CudaError& rhs) = default;
	CudaError(CudaError&& rhs) = default;
	~CudaError() = default;

	CudaError& operator=(const CudaError& rhs) = default;
	CudaError& operator=(CudaError&& rhs) = default;

	static string Format(const char* function, const char* location, cudaError_t error);
	static string Format(const char* function, const char* location, CUresult error);
	static void Check(const char* function, const char* location, cudaError_t error);
	static void Check(const char* function, const char* location, CUresult error);
};
