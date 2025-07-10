#pragma once

class Logger
{
public:
    static void Log(const TSTRING& message);
    static void Warning(const TSTRING& message);
    static void Error(const TSTRING& message);

    // ���� Ÿ�� ���� ��� ���� �α� ���
    static void ForceLog(const TSTRING& message);
    static void ForceWarning(const TSTRING& message);
    static void ForceError(const TSTRING& message);

    // �ָ��ε�, ���� ���� �� ���� ��� �α� ���
public:
    static void InternalLog(LogLevel level, const TSTRING& message, const char* file, int line, const char* func);
};