#include "pch.h"
#include "CudaMemory.h"
#include "CudaError.h"

CudaMemory::CudaMemory(nullptr_t)
{
}

CudaMemory::CudaMemory(int size)
{
	Allocate(size);
}

CudaMemory::CudaMemory(int size, const void* source)
{
	Allocate(size);
	CopyFromHost(source, size);
}

CudaMemory::~CudaMemory() noexcept(false)
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

void CudaMemory::Allocate(int size)
{
	Release();
	if (size < 1)
		throw runtime_error{ "CudaMemory::Allocate - size must be greater than or equal to 1: " + to_string(size) };
	this->size = size;
	auto result = ::cudaMalloc(&data, size);
	CudaError::Check(__FUNCTION__, "cudaMalloc", result);
}

void CudaMemory::Release()
{
	if (data == nullptr)
		return;
	auto result = ::cudaFree(data);
	size = 0;
	data = nullptr;
	CudaError::Check(__FUNCTION__, "cudaFree", result);
}

void CudaMemory::CopyToHost(void* destination, int size) const
{
	if (data == nullptr)
		throw runtime_error{ "Device source pointer is null." };
	if (destination == nullptr)
		throw runtime_error{ "Host destination pointer is null." };
	if (size != this->size)
		throw runtime_error{ "Source/destination size mismatch." };
	auto result = ::cudaMemcpy(destination, data, size, cudaMemcpyDeviceToHost);
	CudaError::Check(__FUNCTION__, "cudaMemcpy", result);
}

void CudaMemory::CopyFromHost(const void* source, int size)
{
	if (data == nullptr)
		throw runtime_error{ "Device destination pointer is null." };
	if (source == nullptr)
		throw runtime_error{ "Host source pointer is null." };
	if (size != this->size)
		throw runtime_error{ "Source/destination size mismatch." };
	auto result = ::cudaMemcpy(data, source, size, cudaMemcpyHostToDevice);
	CudaError::Check(__FUNCTION__, "cudaMemcpy", result);
}

void CudaMemory::CopyFrom(const CudaMemory& source)
{
	if (data == nullptr)
		throw runtime_error{ "Device destination pointer is null." };
	if (source.data == nullptr)
		throw runtime_error{ "Device source pointer is null." };
	if (size != source.size)
		throw runtime_error{ "Source/destination size mismatch." };
	auto result = ::cudaMemcpy(data, source.data, size, cudaMemcpyDeviceToDevice);
	CudaError::Check(__FUNCTION__, "cudaMemcpy", result);
}

void CudaMemory::CopyTo(CudaMemory& destination) const
{
	destination.CopyFrom(*this);
}
