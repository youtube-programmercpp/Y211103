// C++でEXEをダウンロードするサンプル Copyright(C) 2023 https://www.youtube.com/programmercpp
#include <windows.h>                //Windows APIを使用するための記述
#include <wininet.h>                //WinINetを使用するための記述(1/2)
#pragma comment(lib, "wininet.lib") //WinINetを使用するための記述(2/2)
#include <stdexcept>                //std::runtime_errorを使用するための記述
#include <memory>                   //std::unique_ptrを使用するための記述
#include <string>                   //std::to_stringを使用するための記述
#include <fstream>                  //std::ofstreamを使用するための記述
#include <iostream>                 //std::cerrを使用するための記述

//ソースファイル名・行番号・エラーメッセージの文字列リテラルを生成するマクロ
#define	FileLineLiteral(literal_message_text)	__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): " literal_message_text

//典型的なWindows API関数の成否を確認するための関数
template<class T>T VerifyWin32Bool_(T&& r, const char* message)
{
	if (r)
		return std::move(r);//成功したので戻り値を返す
	else
		throw std::runtime_error(message + std::to_string(GetLastError()));//失敗したので例外を投入する
}

//典型的なWindows API関数の成否を確認するためのマクロ
#define	VerifyWin32Bool(expression)	VerifyWin32Bool_(expression, FileLineLiteral(#expression " failed with "))

//ダウンロード関数
void Download
( const std::unique_ptr<std::remove_pointer_t<HINTERNET>, decltype(InternetCloseHandle)*> &  hRequest //リモートファイル
, std::ofstream                                                                           && file     //ローカルファイル
)
{
	for (;;) {
		DWORD cbBuf        ;//読み込んだデータ量（バイト数）
		char  rgchBuf[4096];//読み込んだデータの内容
		//リモートファイルからデータを読み込む
		(void)VerifyWin32Bool(::InternetReadFile
		( /*_In_                                                                 HINTERNET hFile                */       hRequest.get()
		, /*_Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID    lpBuffer             */       rgchBuf 
		, /*_In_                                                                 DWORD     dwNumberOfBytesToRead*/sizeof rgchBuf 
		, /*_Out_                                                                LPDWORD   lpdwNumberOfBytesRead*/       &cbBuf  
		));
		if (cbBuf) {
			//ローカルファイルに書き出す
			if (file.write(rgchBuf, cbBuf))
				continue;//成功したので継続
			else
				throw std::runtime_error(FileLineLiteral("std::ofstream::write failed"));//失敗したので例外を投入
		}
		else
			break;//読み込み終了
	}
}

//サンプルmain関数
int main()
{
	try {
		//準備
		const std::unique_ptr<std::remove_pointer_t<HINTERNET>, decltype(InternetCloseHandle)*> hInternet = { VerifyWin32Bool(InternetOpenA
		( /*_In_opt_ LPCSTR lpszAgent      */"downloader"
		, /*_In_     DWORD  dwAccessType   */INTERNET_OPEN_TYPE_PRECONFIG
		, /*_In_opt_ LPCSTR lpszProxy      */nullptr
		, /*_In_opt_ LPCSTR lpszProxyBypass*/nullptr
		, /*_In_     DWORD  dwFlags        */0
		)), InternetCloseHandle };
		//URL(リモートファイル)を開くための関数を用意
		const auto InternetOpenUrlA = [&hInternet]
		( _In_                            LPCSTR    lpszUrl        
		)->std::unique_ptr<std::remove_pointer_t<HINTERNET>, decltype(InternetCloseHandle)*>
		{
			return { VerifyWin32Bool(::InternetOpenUrlA
			( /*_In_                            HINTERNET hInternet      */hInternet.get()
			, /*_In_                            LPCSTR    lpszUrl        */lpszUrl        
			, /*_In_reads_opt_(dwHeadersLength) LPCSTR    lpszHeaders    */nullptr  
			, /*_In_                            DWORD     dwHeadersLength*/0
			, /*_In_                            DWORD     dwFlags        */0
			, /*_In_opt_                        DWORD_PTR dwContext      */0
			)), InternetCloseHandle };
		};
		//ダウンロード実行（サンプル）
		Download(InternetOpenUrlA("https://aka.ms/vs/17/release/vc_redist.x86.exe"), std::ofstream("vc_redist.x86.exe", std::ios_base::binary));
		Download(InternetOpenUrlA("https://aka.ms/vs/17/release/vc_redist.x64.exe"), std::ofstream("vc_redist.x64.exe", std::ios_base::binary));
	}
	catch (const std::exception& e) {
		//エラーが発生した時は出力ウィンドウをstd::cerrにメッセージを出力する
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
}
