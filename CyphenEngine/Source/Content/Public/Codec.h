#pragma once

#include <vector>

#include "Core/Public/CString.h"
#include "Core/Public/CPrimitiveTypes.h"

struct Resource;

// ============================================================================
// Codec
// ----------------------------------------------------------------------------
// Content file byte buffer를 엔진 Resource 계열 자료형으로 변환하는 dispatch
// facade입니다.
//
// 책임:
//   - sourcePath의 확장자 규칙을 기준으로 적절한 content codec 계열에 위임
//   - 호출자가 준비한 Resource 구체 자료형에 decode 결과 기록
//
// 비책임:
//   - File I/O
//   - ResourceId 발급
//   - Resource lifetime / cache
//   - GPU upload
//   - Renderer command 생성
// ============================================================================

class Codec final
{
public:
	static bool Decode(const CString& sourcePath, const std::vector<uint8>& bytes, Resource& outResource);

private:
	Codec() = delete;
	~Codec() = delete;

	Codec(const Codec& other) = delete;
	Codec& operator=(const Codec& other) = delete;

	Codec(Codec&& other) = delete;
	Codec& operator=(Codec&& other) = delete;
};
