#pragma once

#include <climits>
#include <cstdint>

// ============================================================================
// CPrimitiveTypes
// ----------------------------------------------------------------------------
// CyphenEngine 고정 폭 primitive 타입 정책.
//
// 엔진 공개 API는 std::int*_t / std::uint*_t를 직접 노출하지 않고,
// CyphenEngine primitive alias를 사용합니다.
//
// 규칙:
//     CyphenEngine은 8비트 byte 환경을 요구합니다.
//     uint8은 바이너리 데이터를 표현하는 raw byte 타입입니다.
//     File byte buffer는 uint8을 사용합니다.
//     uint8은 CChar 정책과 독립적입니다.
// ============================================================================

static_assert(CHAR_BIT == 8, "CyphenEngine requires 8-bit bytes.");

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

static_assert(sizeof(int8)* CHAR_BIT == 8, "int8 must be 8 bits.");
static_assert(sizeof(int16)* CHAR_BIT == 16, "int16 must be 16 bits.");
static_assert(sizeof(int32)* CHAR_BIT == 32, "int32 must be 32 bits.");
static_assert(sizeof(int64)* CHAR_BIT == 64, "int64 must be 64 bits.");

static_assert(sizeof(uint8)* CHAR_BIT == 8, "uint8 must be 8 bits.");
static_assert(sizeof(uint16)* CHAR_BIT == 16, "uint16 must be 16 bits.");
static_assert(sizeof(uint32)* CHAR_BIT == 32, "uint32 must be 32 bits.");
static_assert(sizeof(uint64)* CHAR_BIT == 64, "uint64 must be 64 bits.");
