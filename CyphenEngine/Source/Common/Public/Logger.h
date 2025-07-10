#pragma once

class Logger
{
public:
    static void Log(const TSTRING& message);
    static void Warning(const TSTRING& message);
    static void Error(const TSTRING& message);

    // 빌드 타임 유저 사용 강제 로그 출력
    static void ForceLog(const TSTRING& message);
    static void ForceWarning(const TSTRING& message);
    static void ForceError(const TSTRING& message);

    // 핫리로드, 빌드 실패 등 엔진 사용 로그 출력
public:
    static void InternalLog(LogLevel level, const TSTRING& message, const char* file, int line, const char* func);
};