#pragma once

#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>
#include <Windows.h>
#include <minidumpapiset.h>

#pragma comment(lib, "Dbghelp.lib")

class CrashDump
{
public:
	CrashDump(void)
	{
		_dumpCount = 0;

		_invalid_parameter_handler newHandler;
		_invalid_parameter_handler oldHandler;
		newHandler = MyInvalidParameterHandler;
		oldHandler = _set_invalid_parameter_handler(newHandler);

		_CrtSetReportMode(_CRT_WARN, 0);
		_CrtSetReportMode(_CRT_ASSERT, 0);
		_CrtSetReportMode(_CRT_ERROR, 0);
		_CrtSetReportHook(_custom_Report_Hook);

		_set_purecall_handler(MyPureCallHandler);

		SetDumpHandler();
	}

	static void Crash(void)
	{
		int* p = nullptr;
		*p = 0;
	}

	static LONG WINAPI MyExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer)
	{
		long dumpCount = InterlockedIncrement(&_dumpCount);

		SYSTEMTIME time;
		GetLocalTime(&time);

		wchar_t fileName[MAX_PATH];

		wsprintf(fileName, L"Dump_%d%02d%02d_%02d.%02d.%02d_%d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, dumpCount);
		wprintf(L"\n\n[CRASH ERROR] %d%02d%02d_%02d.%02d.%02d\n\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		wprintf(L"Save Dump File...\n");

		HANDLE dumpFile;
		dumpFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (dumpFile != INVALID_HANDLE_VALUE)
		{
			_MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
			miniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
			miniDumpExceptionInfo.ExceptionPointers = pExceptionPointer;
			miniDumpExceptionInfo.ClientPointers = TRUE;

			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile, MiniDumpWithFullMemory, &miniDumpExceptionInfo, NULL, NULL);
			
			CloseHandle(dumpFile);

			wprintf(L"Create Dump File Success\n");
		}

		return EXCEPTION_EXECUTE_HANDLER;
	}

	static void SetDumpHandler(void)
	{
		SetUnhandledExceptionFilter(MyExceptionFilter);
	}

	static void MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
	{
		Crash();
	}

	static void MyPureCallHandler(void)
	{
		Crash();
	}

	static int _custom_Report_Hook(int repostType, char* msg, int* returnValue)
	{
		Crash();
		return true;
	}

	static long _dumpCount;
};

long CrashDump::_dumpCount;