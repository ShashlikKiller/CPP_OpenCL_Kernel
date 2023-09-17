// Библиотеки для OpenCL:
#pragma comment( lib, "Win32//OpenCL.lib" ) // Подключение библиотеки
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS // Переключение на OpenCL 1.2 вместо 2.0 (исправить костыль)
#pragma warning(disable : 4996) // Убрать ошибку о устаревших методах (исправить костыль)
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#include <CL/cl.hpp> // почему не opencl.hpp - ?

#include "getcpuid.h"

// Библиотеки для вычислений:
#include <time.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <conio.h>
#include <thread>
#include <ppl.h>
#include <array>
#include <sstream>
#include <iostream>
#include <vector>
using namespace Concurrency;
using namespace std;