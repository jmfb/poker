#pragma once

#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <execution>
using namespace std;
using namespace chrono;

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#pragma comment(lib, "cuda.lib")
