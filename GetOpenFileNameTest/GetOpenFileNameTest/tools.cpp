#include "stdafx.h"
#include "tools.h"
int calculateRectSize(RECT *rect)
{
	if (rect)
	{
		return (rect->right - rect->left)*(rect->bottom - rect->top);
	}
	return 0;
}