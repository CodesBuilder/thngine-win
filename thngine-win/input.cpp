#include "input.h"
#include <set>

namespace Thngine
{
	namespace Input
	{
		std::set<UINT32> pressedKey;
		void SetKeyDown(UINT32 key, bool down)
		{
			if (down)
				pressedKey.insert(key);
			else
				pressedKey.erase(key);
		}
		
		bool IsKeyDown(UINT32 key)
		{
			if (pressedKey.find(key) == pressedKey.end())
				return false;
			return true;
		}

		void Reset()
		{
			pressedKey.clear();
		}
	}
}