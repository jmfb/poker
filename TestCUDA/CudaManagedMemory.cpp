#include "pch.h"
#include "CudaManagedMemory.h"
#include "CudaError.h"

CudaManagedMemory::CudaManagedMemory(nullptr_t)
{
}

CudaManagedMemory::CudaManagedMemory(int size)
{
	Allocate(size);
}

CudaManagedMemory::~CudaManagedMemory() noexcept(false)
{
	try
	{
		Release();
	}
	catch (...)
	{
		if (exceptionCount.IsSafeToThrow())
			throw;
	}
}

void CudaManagedMemory::Allocate(int size)
{
	Release();
	this->size = size;
	auto result = ::cudaMallocManaged(&data, size, cudaMemAttachGlobal);
	CudaError::Check(__FUNCTION__, "cudaMallocManaged", result);
}

void CudaManagedMemory::Release()
{
	if (data == nullptr)
		return;
	size = 0;
	auto result = ::cudaFree(data);
	CudaError::Check(__FUNCTION__, "cudaFree", result);
	data = nullptr;
}
