#pragma once

#include <Windows.h>

namespace Thngine
{
	namespace Input
	{
		void SetKeyDown(UINT32 key, bool down);
		bool IsKeyDown(UINT32 key);
		void Reset();
	}
}