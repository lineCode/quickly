#pragma warning(disable:4995)
#include "stdafx.h"

SETTING Setting;

BOOL GetFormedStringSize64(unsigned __int64 i64Bytes, LPTSTR pszFormed, int iBuffSize, BOOL bOnlyKB)
{
	BOOL bRet = FALSE;
	int iVLength = 0;
	int iCurrentUnit = 0;
	const int MAX_UNITS = 9;
	unsigned __int64 V, VL = 0;

	NUMBERFMT nFmt = { 0, 0, 3, _T("."), _T(","), 1 };
	const TCHAR* szUnits[MAX_UNITS] = { 
		_T("Bytes")
		, _T("KB")
		, _T("MB")
		, _T("GB")
		, _T("TB")
		, _T("PB")
		, _T("EB")
		, _T("ZB")
		, _T("YB") };

		if ( pszFormed == NULL )
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		V = i64Bytes;

		while ( V > 9999 )
		{
			if ( iCurrentUnit == 1 && bOnlyKB )
				break;
			else if ( iCurrentUnit < 8 )
			{
				V /= 1024;
				iCurrentUnit++;
			}
			else
				break;
		}

		VL = V;
		iVLength =0;
		while ( VL != 0 )
		{
			VL /= 10;
			iVLength++;
		}

		TCHAR* pszNumber = new TCHAR[ iVLength + 1 ];
		TCHAR* pszNumberFormat = new TCHAR[ iVLength + (iVLength/3) + 1 ];
		if ( pszNumber != NULL && pszNumberFormat != NULL )
		{
			StringCchPrintf(pszNumber, iVLength + 1, _T("%I64u"), V);
			::GetNumberFormat( NULL, NULL, pszNumber, &nFmt, pszNumberFormat, iVLength + (iVLength/3) + 1 );
			StringCchPrintf(pszFormed, iBuffSize, _T("%s%s"), pszNumberFormat, szUnits[iCurrentUnit] );
			delete[] pszNumber;
			delete[] pszNumberFormat;
			bRet = TRUE;
		}
		else
		{
			if ( pszNumber != NULL ) delete[] pszNumber;
			if ( pszNumberFormat != NULL ) delete[] pszNumberFormat;
			bRet = FALSE;
		}

		return bRet;
}

BOOL GetFormedStringSize(unsigned int iBytes, LPTSTR pszFormed, int iBuffSize, BOOL bOnlyKB )
{
	unsigned __int64 i64temp = iBytes;
	return GetFormedStringSize64( i64temp, pszFormed, iBuffSize, bOnlyKB);
}