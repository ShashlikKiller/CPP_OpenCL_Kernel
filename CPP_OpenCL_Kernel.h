// ���������� ��� OpenCL:
#pragma comment( lib, "Win32//OpenCL.lib" ) // ����������� ����������
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS // ������������ �� OpenCL 1.2 ������ 2.0 (��������� �������)
#pragma warning(disable : 4996) // ������ ������ � ���������� ������� (��������� �������)
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#include <CL/cl.hpp> // ������ �� opencl.hpp - ?

#include "getcpuid.h"

// ���������� ��� ����������:
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