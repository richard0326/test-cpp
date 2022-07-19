#include <iostream>

using namespace std;

int main()
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
	return 0;
}