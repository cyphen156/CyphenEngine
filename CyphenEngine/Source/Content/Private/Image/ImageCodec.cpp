#include "pch.h"

#include "Content/Private/Image/ImageCodec.h"

#include "Content/Private/Image/Jpeg/JpegCodec.h"
#include "Core/Public/Path.h"
#include "Resource/Public/Resource.h"

namespace
{
	enum class ImageCodecKind : uint32
	{
		Unknown = 0,
		Jpeg = 1
	};

	struct ImageCodecExtensionEntry
	{
		const CChar* extension = nullptr;
		ImageCodecKind kind = ImageCodecKind::Unknown;
	};

	const ImageCodecExtensionEntry ImageCodecExtensionTable[] =
	{
		{ CTEXT(".jpg"), ImageCodecKind::Jpeg },
		{ CTEXT(".jpeg"), ImageCodecKind::Jpeg }
	};

	ImageCodecKind ResolveImageCodecKind(const CString& sourcePath)
	{
		const CString extension = Path::GetExtensionLower(sourcePath);

		for (const ImageCodecExtensionEntry& entry : ImageCodecExtensionTable)
		{
			if (extension == entry.extension)
			{
				return entry.kind;
			}
		}

		return ImageCodecKind::Unknown;
	}
}

bool ImageCodec::Decode(const CString& sourcePath, const std::vector<uint8>& bytes, Resource& outResource)
{
	if (bytes.empty() || outResource.kind != ResourceKind::Texture2D)
	{
		return false;
	}

	switch (ResolveImageCodecKind(sourcePath))
	{
	case ImageCodecKind::Jpeg:
		return JpegCodec::Decode(bytes, outResource);

	default:
		return false;
	}
}
