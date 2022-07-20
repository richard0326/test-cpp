#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main()
{
	int count = 0;
	while (1)
	{
		cout << "Hello Jenkins" << endl;

#if _WIN64
		cout << "64 bit environment" << endl;
#elif _WIN32
		cout << "32 bit environment" << endl;
#endif

#if _DEBUG
		cout << "debug mode" << endl;
#elif NDEBUG
		cout << "release mode" << endl;
#endif

		std::this_thread::sleep_for(1000ms);
		if (count >= 100)
		{
			break;
		}
		count++;
	}
	return 0;
}