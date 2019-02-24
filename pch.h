#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>
#include <array>
#include <set>
#include <map>
#include <random>
#include <algorithm>
#include <optional>
#include <utility>
#include <tuple>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <execution>
#include <cstdint>
#include <limits>
using namespace std;
using namespace chrono;

#include <boost/multiprecision/cpp_int.hpp>
using LargeInteger = boost::multiprecision::int128_t;

#include <boost/compute.hpp>
namespace compute = boost::compute;

#include <boost/iterator/counting_iterator.hpp>
using boost::iterators::make_counting_iterator;

#pragma comment(lib, "OpenCL.lib")

#include <intrin.h>
