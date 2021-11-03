#include <fstream>
#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")
#include <memory>

/*
 呼び出し規約 calling convention
BOOL __stdcall InternetCloseHandle(_In_ HINTERNET hInternet);

BOOL (__stdcall* )(_In_ HINTERNET hInternet)
decltype(InternetCloseHandle)*

*/

namespace RAII {
	typedef std::unique_ptr<std::remove_pointer_t<::HINTERNET>, decltype(InternetCloseHandle)*> HINTERNET;
}
int main()
{
	if (const RAII::HINTERNET hInternet = { InternetOpenA
	( /*_In_opt_ LPCSTR lpszAgent      */"Y211103"
	, /*_In_     DWORD  dwAccessType   */INTERNET_OPEN_TYPE_PRECONFIG
	, /*_In_opt_ LPCSTR lpszProxy      */nullptr
	, /*_In_opt_ LPCSTR lpszProxyBypass*/nullptr
	, /*_In_     DWORD  dwFlags        */0
	), InternetCloseHandle }) {
		if (const RAII::HINTERNET hFile = { InternetOpenUrlA
			( /*_In_                            HINTERNET hInternet      */hInternet.get()
			, /*_In_                            LPCSTR    lpszUrl        */"https://www3.nhk.or.jp/n-data/opendata/coronavirus/nhk_news_covid19_domestic_daily_data.csv"
			, /*_In_reads_opt_(dwHeadersLength) LPCSTR    lpszHeaders    */nullptr
			, /*_In_                            DWORD     dwHeadersLength*/0
			, /*_In_                            DWORD     dwFlags        */0
			, /*_In_opt_                        DWORD_PTR dwContext      */0
			), InternetCloseHandle }) {
			std::ofstream file("test2.csv", std::ios_base::binary);
			char buf[8192];
			DWORD cbRead;
			while (InternetReadFile
			( /*_In_                                                                 HINTERNET hFile                */hFile.get()
			, /*_Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID    lpBuffer             */buf
			, /*_In_                                                                 DWORD     dwNumberOfBytesToRead*/sizeof buf
			, /*_Out_                                                                LPDWORD   lpdwNumberOfBytesRead*/&cbRead
			)) {
				if (cbRead)
					file.write(buf, cbRead);
				else
					break;
			}
		}
	}
}
