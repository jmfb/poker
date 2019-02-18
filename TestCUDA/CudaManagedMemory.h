#pragma once
#include "ExceptionCount.h"

class CudaManagedMemory
{
public:
	CudaManagedMemory() = default;
	CudaManagedMemory(nullptr_t);
	CudaManagedMemory(int size);
	~CudaManagedMemory() noexcept(false);

	void Allocate(int size);
	void Release();

	template <typename Pointer>
	Pointer Get()
	{
		static_assert(is_pointer_v<Pointer>, "Pointer type is required.");
		return reinterpret_cast<Pointer>(data);
	}

	template <typename Pointer>
	Pointer Get() const
	{
		static_assert(is_pointer_v<Pointer>, "Pointer type is required.");
		static_assert(is_const_v<Pointer>, "Pointer cast must be const.");
		return reinterpret_cast<Pointer>(data);
	}

private:
	ExceptionCount exceptionCount;
	int size = 0;
	void* data = nullptr;
};
