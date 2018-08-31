#include "stdafx.h"
#include "cUtil.h"
#include <string>
#include <Windows.h> 


using namespace std;

CUtil::CUtil()
{
}

CUtil::~CUtil()
{
}



str_utf16 * CUtil::GetUTF16FromANSI(const str_ansi * pANSI)
{
	const int nCharacters = pANSI ? int(strlen(pANSI)) : 0;
	str_utf16 * pUTF16 = new str_utf16[nCharacters + 1];

#ifdef _WIN32
	memset(pUTF16, 0, sizeof(str_utf16) * (nCharacters + 1));
	if (pANSI)
		MultiByteToWideChar(CP_ACP, 0, pANSI, -1, pUTF16, nCharacters);
#else
	for (int z = 0; z < nCharacters; z++)
		pUTF16[z] = (str_utf16)((str_utf8)pANSI[z]);
	pUTF16[nCharacters] = 0;
#endif

	return pUTF16;
}


str_ansi * CUtil::GetANSIFromUTF16(const str_utf16 * pUTF16)
{
	const int nCharacters = pUTF16 ? int(wcslen(pUTF16)) : 0;
#ifdef _WIN32
	int nANSICharacters = (2 * nCharacters);
	str_ansi * pANSI = new str_ansi[nANSICharacters + 1];
	memset(pANSI, 0, (nANSICharacters + 1) * sizeof(str_ansi));
	if (pUTF16)
		WideCharToMultiByte(CP_ACP, 0, pUTF16, -1, pANSI, nANSICharacters, NULL, NULL);
#else
	str_utf8 * pANSI = new str_utf8[nCharacters + 1];
	for (int z = 0; z < nCharacters; z++)
		pANSI[z] = (pUTF16[z] >= 256) ? '?' : (str_utf8)pUTF16[z];
	pANSI[nCharacters] = 0;
#endif

	return (str_ansi *)pANSI;
}