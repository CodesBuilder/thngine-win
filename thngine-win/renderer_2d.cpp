#include "renderer.h"
#include "window.h"
#include <map>

namespace Thngine
{
	namespace Renderer
	{
		static ID2D1SolidColorBrush* CreateSolidColorBrush(D2D1_COLOR_F color);

		/// <summary>
		/// Create a solid color brush.
		/// </summary>
		/// <param name="color">Color to create</param>
		/// <returns>Instance of the new brush.</returns>
		static ID2D1SolidColorBrush* CreateSolidColorBrush(D2D1_COLOR_F color)
		{
			ID2D1SolidColorBrush* brush;
			if (FAILED(d2dDeviceContext->CreateSolidColorBrush(color, &brush)))
				return NULL;
			return brush;
		}

		/// <summary>
		/// Draw a rectangle with solid color.
		/// </summary>
		/// <param name="x">X origin</param>
		/// <param name="y">Y origin</param>
		/// <param name="width">Rectangle width</param>
		/// <param name="height">Rectangle height</param>
		/// <param name="color">Color to draw</param>
		void DrawRectangle(int x, int y, int width, int height, D2D1_COLOR_F color)
		{
			ComPtr<ID2D1SolidColorBrush> brush = CreateSolidColorBrush((color));

			d2dDeviceContext->DrawRectangle(D2D1::RectF(x, y, x + width, y + height), brush.Get());
		}
	}
}