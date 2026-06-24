#include "pch.h"

#include "Content/Public/Codec.h"
#include "Content/Private/Image/ImageCodec.h"
#include "Resource/Public/Resource.h"

bool Codec::Decode(const CString& sourcePath, const std::vector<uint8>& bytes, Resource& outResource)
{
	if (bytes.empty())
	{
		return false;
	}

	switch (outResource.kind)
	{
	case ResourceKind::Texture2D:
		return ImageCodec::Decode(sourcePath, bytes, outResource);

	default:
		return false;
	}
}
