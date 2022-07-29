#include "stdafx.h"
#include "Service.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

wchar_t g_service_name[256] = L"_TestService";

enum ServiceTestMode
{
	fail_direct,
	fail_slow,
	success_never_die,
};

void Simple_FileLog(const wchar_t* filename, const wchar_t* write_buf)
{
	auto file_ptr = _wfopen(filename, L"a+");
	
	if (fputws(write_buf, file_ptr) < 0) {
		fclose(file_ptr);
		return;
	}

	fclose(file_ptr);
}

void Intro() {
	Simple_FileLog(L"service_log.txt", L"Hello, This is service test for Jenkins");

#if _WIN64
	Simple_FileLog(L"service_log.txt", L"64 bit environment");
#elif _WIN32
	Simple_FileLog(L"service_log.txt", L"32 bit environment");
#endif

#if _DEBUG
	Simple_FileLog(L"service_log.txt", L"debug mode");
#elif NDEBUG
	Simple_FileLog(L"service_log.txt", L"release mode");
#endif
}

void WaitingToDie(int deadline)
{
	int count = 0;
	while (1)
	{
		wchar_t write_file[256] = { 0, };
		wsprintf(write_file, L"count : %d", count);
		Simple_FileLog(L"service_log.txt", write_file);
		std::this_thread::sleep_for(1000ms);

		// deadline�� 0���� ������ ������
		if (deadline <= 0 && count >= deadline)
		{
			break;
		}
		count++;
	}
}

void RandomAction()
{
	//auto rand_val = rand() % 4;
	auto rand_val = ServiceTestMode::success_never_die;

	switch ((ServiceTestMode)rand_val) {
	case ServiceTestMode::fail_direct: {
		WaitingToDie(0);
		break;
	}
	case ServiceTestMode::fail_slow: {
		// 30~40��
		WaitingToDie((rand() % 10) + 30);
		break;
	}
	case ServiceTestMode::success_never_die: {
		// ���� ����ϰ� ��� ���� �ִ´�.
		WaitingToDie(rand() % 5);

		Simple_FileLog(L"service_log.txt", L"server init success");

		// ���� ����
		WaitingToDie(-1);
		break;
	}
	default:
		break;
	}
}

class UpdateService : public CService
{
public:

	// ���񽺰� ���۵Ǹ� ȣ��� main �Լ�
	// �빮�� M
	void Main(void) {
		CService::Begin(g_service_name);

		// ���� �� ������ ����
		Intro();
		RandomAction();

		CService::End();
	}

protected:

	// ���� ���۽� ������ �Լ�
	void OnStarted() {
		Simple_FileLog(L"service_log.txt", L"server start");
	}

	// ���� ����� ������ �Լ�
	void OnStopped() {
		Simple_FileLog(L"service_log.txt", L"server stop");		
	}
};

int wmain(int arg, wchar_t* argv[])
{
	UpdateService service;

	if (arg == 2 && wcscmp(L"-U", argv[1]) == 0) {
		// ���񽺸� ��� �����ϴ� �Լ�
		if (!service.Uninstall(g_service_name)) {
			cout << "fail error code : " << GetLastError() << endl;
			return 0;
		}
		cout << "uninstall success " << endl;
	}
	else {
		// ���񽺸� ����ϴ� �Լ�
		if (!service.Install(g_service_name)) {
			cout << "fail error code : " << GetLastError() << endl;
			// 5 : access denied
			// 1073 : ���񽺰� �̹� ����
			return 0;
		}
		cout << "install success " << endl;
	}
	
	return 0;
}