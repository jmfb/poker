#pragma once
#include "ExceptionCount.h"

class CudaMemory
{
public:
	CudaMemory() = default;
	CudaMemory(nullptr_t);
	CudaMemory(int size);
	CudaMemory(int size, const void* source);
	CudaMemory(const CudaMemory& rhs) = delete;
	CudaMemory(CudaMemory&& rhs) = default;
	~CudaMemory() noexcept(false);

	CudaMemory& operator=(const CudaMemory& rhs) = delete;
	CudaMemory& operator=(CudaMemory&& rhs) = default;

	void Allocate(int size);
	void Release();
	void CopyToHost(void* destination, int size) const;
	void CopyFromHost(const void* source, int size);
	void CopyFrom(const CudaMemory& source);
	void CopyTo(CudaMemory& destination) const;

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
