#include <stdio.h>
#include <windows.h>
#include <winhttp.h>
#include <stdlib.h>

__declspec(dllexport) unsigned char* https_get(const char* url, DWORD* outLength) {
    *outLength = 0;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, url, -1, NULL, 0);
    if (wlen == 0)
        return NULL;
    wchar_t* wurl = (wchar_t*)malloc(wlen * sizeof(wchar_t));
    if (!wurl)
        return NULL;
    MultiByteToWideChar(CP_UTF8, 0, url, -1, wurl, wlen);

    URL_COMPONENTS uc = { sizeof(uc) };
    wchar_t hostName[256] = { 0 };
    wchar_t urlPath[1024] = { 0 };

    uc.lpszHostName = hostName;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = urlPath;
    uc.dwUrlPathLength = 1024;

    if (!WinHttpCrackUrl(wurl, 0, 0, &uc)) {
        free(wurl);
        return NULL;
    }

    HINTERNET hSession = WinHttpOpen(L"sodoff-https/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession) {
        free(wurl);
        return NULL;
    }

    DWORD secureProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));

    HINTERNET hConnect = WinHttpConnect(hSession, hostName, uc.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"GET",
        urlPath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    BOOL bResults = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0);
    if (!bResults) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    unsigned char* buffer = NULL; DWORD totalSize = 0, size = 0;
    while (WinHttpQueryDataAvailable(hRequest, &size) && size) {
        unsigned char* newBuffer = realloc(buffer, totalSize + size + 1);
        if (!newBuffer) {
            free(buffer);
            buffer = NULL;
            break;
        }

        buffer = newBuffer;
        DWORD read = 0;
        if (!WinHttpReadData(hRequest, buffer + totalSize, size, &read)) break;
        totalSize += read;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(wurl);

    if (buffer)
        *outLength = totalSize;
    return buffer;
}

__declspec(dllexport) unsigned char* https_post(const char* url, const char* form, DWORD* outLength) {
    *outLength = 0;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, url, -1, NULL, 0);
    if (!wlen)
        return NULL;
    wchar_t* wurl = malloc(wlen * sizeof(*wurl));
    if (!wurl)
        return NULL;
    MultiByteToWideChar(CP_UTF8, 0, url, -1, wurl, wlen);

    URL_COMPONENTS uc = { sizeof(uc) };
    wchar_t hostName[256] = { 0 };
    wchar_t urlPath[1024] = { 0 };
    uc.lpszHostName = hostName;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = urlPath;
    uc.dwUrlPathLength = 1024;
    if (!WinHttpCrackUrl(wurl, 0, 0, &uc)) {
        free(wurl);
        return NULL;
    }

    HINTERNET hSession = WinHttpOpen(L"sodoff-https/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession) {
        free(wurl);
        return NULL;
    }
    DWORD secureProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));

    HINTERNET hConnect = WinHttpConnect(hSession, hostName, uc.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"POST",
        urlPath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    wchar_t headers[] = L"Content-Type: application/x-www-form-urlencoded\r\n";
    DWORD formLength = form ? (DWORD)strlen(form) : 0;

    BOOL bResults = WinHttpSendRequest(hRequest,
        headers,
        -1,
        (LPVOID)form,
        formLength,
        formLength,
        0);
    if (!bResults) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        free(wurl);
        return NULL;
    }

    unsigned char* buffer = NULL;
    DWORD totalSize = 0, size = 0;
    while (WinHttpQueryDataAvailable(hRequest, &size) && size) {
        unsigned char* newBuffer = realloc(buffer, totalSize + size + 1);
        if (!newBuffer) {
            free(buffer);
            buffer = NULL;
            break;
        }
        buffer = newBuffer;
        DWORD read = 0;
        if (!WinHttpReadData(hRequest, buffer + totalSize, size, &read))
            break;
        totalSize += read;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(wurl);

    if (buffer)
        *outLength = totalSize;
    return buffer;
}

__declspec(dllexport) void free_buffer(unsigned char* buffer) {
    if (buffer)
        free(buffer);
}
