#include "CPP_OpenCL_Kernel.h"

// Ссылки:
// https://gist.github.com/ddemidov/8079172 - компиляция ядра
// https://habr.com/ru/articles/146823/ - гайд на opencl
// https://habr.com/ru/articles/448216/ - сборка visual studio
// http://www.kimrt.ru/courses/stm/self_work/OpenCL_project_setup.pdf - еще пример компиляции ядра
// https://vc.ru/dev/159198-kak-uskorit-vychisleniya-i-povysit-proizvoditelnost-programm-s-pomoshchyu-principov-massivnogo-parallelizma-i-opencl - еще гайд
// https://github.com/Akson/OpenCL_Test1 - пример кода
// Серия гайдов на opencl:
// Часть 1 - https://habr.com/ru/companies/yandex_praktikum/articles/580118/
// Часть 2 - https://habr.com/ru/companies/yandex_praktikum/articles/591541/
// Часть 3 - https://habr.com/ru/companies/yandex_praktikum/articles/661543/
// Презентация от одного из инженеров - https://speakerdeck.com/ddemidov/proghrammirovaniie-opencl-s-ispol-zovaniiem-bibliotiek-c-plus-plus?slide=56
// Видео туториал - https://www.youtube.com/watch?v=f8jnAuFMnPY&list=PLzy5q1NUJKCJocUKsRxZ0IPz29p38xeM-&index=11&ab_channel=WesleyShillingford
//


float MathCalculations(float a, float b);

const int DATA_SIZE = 20 * 1024 * 1024;
const int TESTS_NUMBER = 200; // количество прогонов
float* pInputVector1; // Массивы чисел
float* pInputVector2;
float* pOutputVector; // Выходной массив для проверки значений
float* pOutputVectorHost;
double hostPerformanceTimeMS = 0;

std::vector<double> timeValues; // Вектор для 

/// <summary>
/// Вывод статистики по затраченному времени на вычисление на девайсе
/// </summary>
void PrintTimeStatistic()
{
	std::sort(timeValues.begin(), timeValues.end()); // сортировка значений вектора для более удобных действий с ним
	double totalTime = std::accumulate(timeValues.begin(), timeValues.end(), 0.0);
	double averageTime = totalTime / timeValues.size();
	double minTime = timeValues[0];
	double maxTime = timeValues[timeValues.size() - 1];
	double medianTime = timeValues[timeValues.size() / 2];
	cout << "Calculation time statistic: (" << timeValues.size() << " runs)" << endl;
	//cout << "Med: " << medianTime << " ms (" << hostPerformanceTimeMS / medianTime << "X faster then host)" << endl;
	cout << "Avg: " << averageTime << " ms" << endl;
	//cout << "Min: " << minTime << " ms" << endl;
	//cout << "Max: " << maxTime << " ms" << endl << endl;
}

/// <summary>
/// Генерация случайных чисел в массивах для вычислений
/// </summary>
void GenerateTestData()
{
	std::cout << "Generating data..." << endl;
	pInputVector1 = new float[DATA_SIZE];
	pInputVector2 = new float[DATA_SIZE];
	pOutputVector = new float[DATA_SIZE];
	pOutputVectorHost = new float[DATA_SIZE];

	srand((unsigned int)time(NULL));
	for (int i = 0; i < DATA_SIZE; i++)
	{
		pInputVector1[i] = rand() * 1000.0f / RAND_MAX;
		pInputVector2[i] = rand() * 1000.0f / RAND_MAX;
	}
}

void STDThreadCalculationFunction(int start, int end)
{
	for (int iJob = start; iJob < end; iJob++)
	{
		//Perform calculations
		pOutputVector[iJob] = MathCalculations(pInputVector1[iJob], pInputVector2[iJob]);
	}
}

void PerformCalculationsOnHost()
{
	cout << "Device: Host" << endl << endl;

	//Some performance measurement
	timeValues.clear();
	__int64 start_count;
	__int64 end_count;
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	for (int iTest = 0; iTest < (TESTS_NUMBER / 10); iTest++)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start_count);
		for (int iJob = 0; iJob < DATA_SIZE; iJob++)
		{
			//Perform calculations
			pOutputVectorHost[iJob] = MathCalculations(pInputVector1[iJob], pInputVector2[iJob]);
		}
		QueryPerformanceCounter((LARGE_INTEGER*)&end_count);
		double time = 1000 * (double)(end_count - start_count) / (double)freq;
		timeValues.push_back(time);
	}
	hostPerformanceTimeMS = std::accumulate(timeValues.begin(), timeValues.end(), 0.0) / timeValues.size();

	PrintTimeStatistic();
}

void PerformCalculationsOnHostParallelFor()
{
	cout << endl << "-------------------------------------------------" << endl;
	cout << "Device: Host parallel_for" << endl << endl;

	//Some performance measurement
	timeValues.clear();
	__int64 start_count;
	__int64 end_count;
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	for (int iTest = 0; iTest < (TESTS_NUMBER / 5); iTest++)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start_count);
		parallel_for(size_t(0), size_t(DATA_SIZE), [&](size_t iJob)
			{
				//Perform calculations
				pOutputVector[iJob] = MathCalculations(pInputVector1[iJob], pInputVector2[iJob]);
			});
		QueryPerformanceCounter((LARGE_INTEGER*)&end_count);
		double time = 1000 * (double)(end_count - start_count) / (double)freq;
		timeValues.push_back(time);
	}

	PrintTimeStatistic();
}

void PerformCalculationsOnHostSTDThread1() // 1 thr
{
	cout << endl << "-------------------------------------------------" << endl;
	cout << "Device: Host std::thread 1 " << endl << endl;

	//Some performance measurement
	timeValues.clear();
	__int64 start_count;
	__int64 end_count;
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	int threadsNumber = 1;
	cout << "Threads number: " << threadsNumber << endl << endl;
	int jobsPerThread = DATA_SIZE / threadsNumber;

	for (int iTest = 0; iTest < (TESTS_NUMBER / 5); iTest++)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start_count);

		int curStartJob = 0;
		std::vector<std::thread> threadVector;
		for (int iThread = 0; iThread < threadsNumber; iThread++)
		{
			threadVector.push_back(std::thread(STDThreadCalculationFunction, curStartJob, min(curStartJob + jobsPerThread, DATA_SIZE)));
			curStartJob += jobsPerThread;
		}

		for (auto thread = threadVector.begin(); thread != threadVector.end(); thread++)
			thread->join();

		QueryPerformanceCounter((LARGE_INTEGER*)&end_count);
		double time = 1000 * (double)(end_count - start_count) / (double)freq;
		timeValues.push_back(time);
	}

	PrintTimeStatistic();
}

void PerformTestOnDevice(cl::Device device)
{
	cout << endl << "-------------------------------------------------" << endl;
	cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << endl << endl;

	//For the selected device create a context
	vector<cl::Device> contextDevices;
	contextDevices.push_back(device);
	cl::Context context(contextDevices);

	// Создаем очередь для девайса
	cl::CommandQueue queue(context, device);

	//Clean output buffers
	fill_n(pOutputVector, DATA_SIZE, 0);

	//Create memory buffers
	cl::Buffer clmInputVector1 = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(float), pInputVector1);
	cl::Buffer clmInputVector2 = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(float), pInputVector2);
	cl::Buffer clmOutputVector = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(float), pOutputVector);

	//Load OpenCL source code
	std::ifstream sourceFile("OpenCLFile1.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cout << "Source code: " << sourceCode << endl;

	//Build OpenCL program and make the kernel
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	cl::Program program = cl::Program(context, source);
	cout << "building the kernel... " << endl;
		try 
		{
			program.build(contextDevices);
		}
		catch (const cl::Error& err) // отлов ошибкиe с выводом исключения
		{
			std::cerr
				<< "OpenCL compilation error" << std::endl
				<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevices[0])
				<< std::endl;
			throw err;
	}
	cout << "building completed." << endl;

	cl::Kernel kernel(program, "TestKernel");

	//Set arguments to kernel
	int iArg = 0;
	kernel.setArg(iArg++, clmInputVector1);
	kernel.setArg(iArg++, clmInputVector2);
	kernel.setArg(iArg++, clmOutputVector);
	kernel.setArg(iArg++, DATA_SIZE);

	//Some performance measurement
	timeValues.clear();
	__int64 start_count;
	__int64 end_count;
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	//Run the kernel on specific ND range
	for (int iTest = 0; iTest < TESTS_NUMBER; iTest++)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start_count);

		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(DATA_SIZE), cl::NDRange(128)); // запуск ядра 
		queue.finish();

		QueryPerformanceCounter((LARGE_INTEGER*)&end_count);
		double time = 1000 * (double)(end_count - start_count) / (double)freq;
		timeValues.push_back(time);
	}
	PrintTimeStatistic();
	// Read buffer C into a local list
	queue.enqueueReadBuffer(clmOutputVector, CL_TRUE, 0, DATA_SIZE * sizeof(float), pOutputVector);
}

void CheckResults()
{
	double avgRelAbsDiff = 0;
	double maxRelAbsDiff = 0;
	for (int iJob = 0; iJob < DATA_SIZE; iJob++)
	{
		double absDif = abs(pOutputVectorHost[iJob] - pOutputVector[iJob]);
		double relAbsDif = abs(absDif / pOutputVectorHost[iJob]);
		avgRelAbsDiff += relAbsDif;
		maxRelAbsDiff = max(maxRelAbsDiff, relAbsDif);
		pOutputVector[iJob] = 0;
	}
	avgRelAbsDiff /= DATA_SIZE;

	cout << "Errors:" << endl;
	cout << "avgRelAbsDiff = " << avgRelAbsDiff << endl;
	cout << "maxRelAbsDiff = " << maxRelAbsDiff << endl;
}

int main(int argc, char* argv[])
{
	std::cout << GetCpuVendorString() << endl;
	//std::cout << GetCpuVendorSubstring() << endl;
	GenerateTestData();
	//PerformCalculationsOnHost();
	//PerformCalculationsOnHostSeparateFunction();
	//CheckResults();
	//PerformCalculationsOnHostParallelFor();
	//CheckResults();
	//PerformCalculationsOnHostSTDThread();
	CheckResults();
	//PerformCalculationsOnHostSTDThread1();
	//CheckResults();

	//Get all available platforms
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	for (unsigned short iPlatform = 0; iPlatform < platforms.size(); iPlatform++)
	{
		//Get all available devices on selected platform
		std::vector<cl::Device> devices;
		platforms[iPlatform].getDevices(CL_DEVICE_TYPE_ALL, &devices);
		cout << iPlatform << endl; // закомментить

		//Perform test on each device
		for (unsigned int iDevice = 0; iDevice < devices.size(); iDevice++)
		{
			try
			{
				PerformTestOnDevice(devices[iDevice]);
			}
			catch (cl::Error error)
			{
				std::cout << error.what() << "(" << error.err() << ")" << std::endl;
			}
			CheckResults();
		}
	}

	std::cout << "Program is ended correctly." << endl;

	//Clean buffers
	delete[](pInputVector1);
	delete[](pInputVector2);
	delete[](pOutputVector);
	delete[](pOutputVectorHost);

	cin.get();
	//_getch();

	return 0;
}