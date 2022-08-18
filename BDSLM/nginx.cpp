#include "pch.h"
#include "conf.h"

using namespace std;

int writeNgxConf(unsigned short int port) {
    string fstr("listen      " + std::to_string(port) + ";\nlisten        [::]:" + std::to_string(port) + ";\n"+"set $apiPort "+ std::to_string(config.apiServerPort) +";");
    try
    {
        std::fstream fout("plugins/BDSLM/nginx/conf/port.conf", std::ios::out | std::ios::trunc);
        fout << fstr << std::endl;
        fout.close();
    }
    catch (std::exception& e)
    {
        Message::logger( "error happened: %s" , e.what());
    }
    return 0;
}

inline bool exists_ngxPidFile(const std::string& path) {
    std::fstream ngxPidStream;
    ngxPidStream.open(path, std::ios::in);
    if (ngxPidStream) {
        return true;
    }
    else {
        return false;
    }
}

int startNginx() {
    writeNgxConf(config.webServerPort);
    char cWindowsDirectory[MAX_PATH]{};
    LPTSTR cWinDir = new TCHAR[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cWinDir);
    char* sConLin = (char*)"./plugins/BDSLM/nginx/nginx.exe -p ./plugins/BDSLM/nginx/";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    if (CreateProcessA(
        NULL,
        sConLin,
        NULL,
        NULL,
        false, 
        0,
        NULL,
        NULL,
        &si,
        &pi
    ))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    else {
        Message::logger("Failed to create process");
        return false;
    }
}

int stopNginx() {
    if (exists_ngxPidFile("./plugins/BDSLM/nginx/logs/nginx.pid")) {
        char cWindowsDirectory[MAX_PATH]{};
        LPTSTR cWinDir = new TCHAR[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, cWinDir);
        char* sConLin = (char*)"./plugins/BDSLM/nginx/nginx.exe -p ./plugins/BDSLM/nginx/ -s quit";

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        if (CreateProcessA(
            NULL,
            sConLin,
            NULL,
            NULL,
            false,
            0,
            NULL,
            NULL,
            &si,
            &pi
        ))
        {
            WaitForSingleObject(pi.hProcess, 2000);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return false;
        }
        else {
            Message::logger("failed to create process");
            return true;
        }
    }
    else {
        return false;
    }
}