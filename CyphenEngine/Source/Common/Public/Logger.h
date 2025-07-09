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
private:
    static void Warning(const TSTRING& message, const char* file = nullptr, 
        int line = 0, const char* func = nullptr);

    static void Error(const TSTRING& message, const char* file = nullptr, 
        int line = 0, const char* func = nullptr);

    static void FatalError(const TSTRING& message, const char* file = nullptr, 
        int line = 0, const char* func = nullptr);
};