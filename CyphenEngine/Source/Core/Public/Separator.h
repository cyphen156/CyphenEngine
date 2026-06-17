#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"

// ============================================================================
// Separator / Separators - 문자열 구분자 정책 계층.
//
// Separator = 구분자 식별값. Separators = 정책 테이블 + 변환.
// Path 전용이 아니라 Core 문자열 정책이며, Path가 이를 소비합니다.
//
// 기본: Engine='/', Windows='\', Unix='/'.
// 기본 구분자는 삭제하지 않습니다.
// 사용자 구분자는 Refresh로 기본 구분자 뒤에 추가합니다.
// 실시간 제거는 지원하지 않습니다.
//
// 주의:
//     내부 테이블은 공유 가변 전역입니다.
//     Reset / Refresh는 설정 변경 시점(startup 등 단일 스레드)에서만 호출합니다.
//     워커 스레드 실행 중 Refresh는 Resolve / IsRegistered / Convert와 data race입니다.
// ============================================================================

using Separator = uint32;

class Separators final
{
public:
	static constexpr Separator Engine = 0;
	static constexpr Separator Windows = 1;
	static constexpr Separator Unix = 2;
	static constexpr Separator UserBegin = 3;

	static void Reset();
	static void Refresh(const std::vector<CChar>& userSeparators);

	static bool IsValid(Separator separator);
	static CChar Resolve(Separator separator);
	static bool IsRegistered(CChar separator);

	static CString Convert(const CString& text, Separator sourceSeparator, Separator targetSeparator);

	static CString Convert(const CString& text, CChar sourceSeparator, CChar targetSeparator);

private:
	Separators() = delete;
	~Separators() = delete;

	Separators(const Separators& other) = delete;
	Separators& operator=(const Separators& other) = delete;

	Separators(Separators&& other) = delete;
	Separators& operator=(Separators&& other) = delete;
};
