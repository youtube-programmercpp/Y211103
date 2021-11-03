#define	_CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")
#include <stdio.h>
int main()
{
	const HINTERNET hInternet = InternetOpenA
	( /*_In_opt_ LPCSTR lpszAgent      */"Y211103"
	, /*_In_     DWORD  dwAccessType   */INTERNET_OPEN_TYPE_PRECONFIG
	, /*_In_opt_ LPCSTR lpszProxy      */NULL
	, /*_In_opt_ LPCSTR lpszProxyBypass*/NULL
	, /*_In_     DWORD  dwFlags        */0
	);
	if (hInternet) {
		const HINTERNET hFile = InternetOpenUrlA
		( /*_In_                            HINTERNET hInternet      */hInternet
		, /*_In_                            LPCSTR    lpszUrl        */"https://www3.nhk.or.jp/n-data/opendata/coronavirus/nhk_news_covid19_domestic_daily_data.csv"
		, /*_In_reads_opt_(dwHeadersLength) LPCSTR    lpszHeaders    */NULL
		, /*_In_                            DWORD     dwHeadersLength*/0
		, /*_In_                            DWORD     dwFlags        */0
		, /*_In_opt_                        DWORD_PTR dwContext      */0
		);
		if (hFile) {
			FILE* const fp = fopen("test.csv", "wb");
			if (fp) {
				// CSV ÉtÉ@ÉCÉãÇÃì«Ç›çûÇ›
				char buf[8192];
				DWORD cbRead;
				while (InternetReadFile
				( /*_In_                                                                 HINTERNET hFile                */hFile
				, /*_Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID    lpBuffer             */buf
				, /*_In_                                                                 DWORD     dwNumberOfBytesToRead*/sizeof buf
				, /*_Out_                                                                LPDWORD   lpdwNumberOfBytesRead*/&cbRead
				)) {
					if (cbRead)
						fwrite(buf, cbRead, 1, fp);
					else
						break;
				}
				fclose(fp);
			}

			InternetCloseHandle(hFile);
		}
		InternetCloseHandle(hInternet);
	}
}
