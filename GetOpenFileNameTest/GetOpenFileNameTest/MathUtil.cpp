#include "stdafx.h"
#include "MathUtil.h"
#include <math.h>

MathUtil::MathUtil()
{
}

MathUtil::~MathUtil()
{
}
 
double MathUtil::Round(double dVal, short iPlaces)
{
	double dRetval;
	double dMod = 0.0000001;
	if (dVal<0.0) dMod = -0.0000001;
	dRetval = dVal;
	dRetval += (5.0 / pow(10.0, iPlaces + 1.0));
	dRetval *= pow(10.0, iPlaces);
	dRetval = floor(dRetval + dMod);
	dRetval /= pow(10.0, iPlaces);
	return(dRetval);
}