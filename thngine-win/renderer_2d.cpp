#include "renderer.h"
#include "window.h"
#include <map>

namespace Thngine
{
	namespace Renderer
	{
		ID2D1SolidColorBrush* CreateSolidColorBrush(D2D1_COLOR_F color)
		{
			ID2D1SolidColorBrush* brush;
			if (FAILED(d2dDeviceContext->CreateSolidColorBrush(color, &brush)))
				return NULL;
			return brush;
		}
		void DrawRectangle(int x, int y, int width, int height, ID2D1Brush* brush)
		{
			d2dDeviceContext->DrawRectangle(D2D1::RectF(x, y, x + width, y + height), brush);
		}
	}
}