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

		// deadline이 0보다 작으면 무기한
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
		// 30~40초
		WaitingToDie((rand() % 10) + 30);
		break;
	}
	case ServiceTestMode::success_never_die: {
		// 성공 출력하고 계속 켜져 있는다.
		WaitingToDie(rand() % 5);

		Simple_FileLog(L"service_log.txt", L"server init success");

		// 죽지 않음
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

	// 서비스가 시작되면 호출될 main 함수
	// 대문자 M
	void Main(void) {
		CService::Begin(g_service_name);

		// 서비스 중 실행할 내용
		Intro();
		RandomAction();

		CService::End();
	}

protected:

	// 서비스 시작시 동작할 함수
	void OnStarted() {
		Simple_FileLog(L"service_log.txt", L"server start");
	}

	// 서비스 종료시 동작할 함수
	void OnStopped() {
		Simple_FileLog(L"service_log.txt", L"server stop");		
	}
};

int wmain(int arg, wchar_t* argv[])
{
	UpdateService service;

	if (arg == 2 && wcscmp(L"-U", argv[1]) == 0) {
		// 서비스를 등록 해제하는 함수
		if (!service.Uninstall(g_service_name)) {
			cout << "fail error code : " << GetLastError() << endl;
			return 0;
		}
		cout << "uninstall success " << endl;
	}
	else {
		// 서비스를 등록하는 함수
		if (!service.Install(g_service_name)) {
			cout << "fail error code : " << GetLastError() << endl;
			// 5 : access denied
			// 1073 : 서비스가 이미 존재
			return 0;
		}
		cout << "install success " << endl;
	}
	
	return 0;
}