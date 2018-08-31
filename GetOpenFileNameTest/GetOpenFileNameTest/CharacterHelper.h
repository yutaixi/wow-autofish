#ifndef __CHARACTER_HELPER__
#define __CHARACTER_HELPER__
#include <string>
#include <Windows.h>
using namespace std;
typedef char                                        str_ansi;
typedef unsigned char                               str_utf8;
typedef wchar_t                                     str_utf16;
 
class CAPECharacterHelper
{
public:
	static str_ansi * GetANSIFromUTF8(const str_utf8 * pUTF8);
	static str_ansi * GetANSIFromUTF16(const str_utf16 * pUTF16);
	static str_utf16 * GetUTF16FromANSI(const str_ansi * pANSI);
	static str_utf16 * GetUTF16FromUTF8(const str_utf8 * pUTF8);
	static str_utf8 * GetUTF8FromANSI(const str_ansi * pANSI);
	static str_utf8 * GetUTF8FromUTF16(const str_utf16 * pUTF16);
	static string* byteToHexStr(byte byte_arr[], int arr_len);
	static void HexStrToByte(string str_arr, byte byte_arr[]);
	static string connectStr(string str1, string str2);
	static str_utf16* getUTF16FromBool(bool src);
	static str_utf16* getUTF16FromBOOL(BOOL src);
};
#endif // !__CHARACTER_HELPER__

