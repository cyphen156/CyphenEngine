#pragma once

class Logger;

// Global Macro
// ============================================================================
// Logging Macro
// ----------------------------------------------------------------------------
// 로그 메시지를 출력할 때 사용하는 매크로입니다.
// 자동으로 파일명, 라인 번호, 함수명을 함께 출력합니다.
//
// 사용 예시:
//   LOG_INTERNAL(LogLevel::Info, "엔진 초기화 완료");
//   LOG_INTERNAL_C(LogLevel::Error, CTEXT("오류 발생: ") + 오류메시지);
//
// - LOG_INTERNAL(level, msg)
//   : ASCII 문자열(리터럴)을 로그로 출력할 때 사용
//
// - LOG_INTERNAL_C(level, cstr)
//   : CString 또는 CTEXT(str) 기반 유니코드 문자열 출력 시 사용
//
// 주의:
//   LOG_INTERNAL_C는 문자열 결합 시 반드시 CString으로 캐스팅해야 합니다.
//
// ============================================================================
#define LOG_INTERNAL(level, msg)	Logger::InternalLog(level, CTEXT(msg), __FILE__, __LINE__, __func__)
#define LOG_INTERNAL_C(level, cstr)	Logger::InternalLog(level, cstr, __FILE__, __LINE__, __func__)

