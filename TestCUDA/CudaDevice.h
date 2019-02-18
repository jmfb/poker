#pragma once

class CudaDevice
{
public:
	static void Synchronize();
	static int GetAttribute(int device, cudaDeviceAttr attribute);
};
