#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <execution>
#include <memory>
using namespace std;
using namespace chrono;

#include <boost/compute.hpp>
namespace compute = boost::compute;

#include <boost/multiprecision/cpp_int.hpp>
using LargeInteger = boost::multiprecision::int128_t;

#include <CL/cl.h>

#pragma comment(lib, "OpenCL.lib")

#include "int128.h"
