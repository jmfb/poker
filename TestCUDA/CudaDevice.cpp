#include "pch.h"
#include "CudaDevice.h"
#include "CudaError.h"

void CudaDevice::Synchronize()
{
	auto result = ::cudaDeviceSynchronize();
	CudaError::Check(__FUNCTION__, "cudaDeviceSynchronize", result);
}

int CudaDevice::GetAttribute(int device, cudaDeviceAttr attribute)
{
	int value = 0;
	auto result = ::cudaDeviceGetAttribute(&value, attribute, device);
	CudaError::Check(__FUNCTION__, "cudaDeviceGetAttribute", result);
	return value;
}

