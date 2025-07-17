#include "pch.h"
#include "Modules/Renderer/Public/Renderer.h"
#include "Common/Public/Logger.h"

void Renderer::InitRenderer(RendererType type)
{
	switch (type)
	{
	case RendererType::DX11:
	{
		break;
	}	

	case RendererType::DX12:
		break;

	case RendererType::VULKAN:
		break;

	case RendererType::METAL:
		break;

	case RendererType::GDI_PLUS:
		break;

	default:
		LOG_INTERNAL(LogLevel::Error, "Unsupported renderer type.");
		break;
	}
}

void Renderer::Shutdown()
{
}

RendererType Renderer::GetRendererType()
{
	return RendererType();
}

const TSTRING& Renderer::GetRendererName()
{
	// TODO: 여기에 return 문을 삽입합니다.
}

const TSTRING& Renderer::GetRendererVersion()
{
	// TODO: 여기에 return 문을 삽입합니다.
}

const TSTRING& Renderer::GetRendererAPIVersion()
{
	// TODO: 여기에 return 문을 삽입합니다.
}

bool Renderer::IsInitialized()
{
	return false;
}
