<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.ntauthentication.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

Define('c_sMCryptLinuxPath', '/usr/local/lib/libmcrypt');
Define('c_sMCryptWindows2000Path', 'c:\winnt');

function DESSetOddParity(&$aKey)
{
	for($i = 0; $i < 8; $i++)
	{
		$c = $aKey[$i] & 254;
		$c = ($c >> 4) ^ ($c & 15);
		$c = ($c >> 2) ^ ($c & 3);
		$aKey[$i] = ($aKey[$i] & 254) | (($c >> 1) ^ ($c & 1)) ^ 1;
	}
}

function SetupDESKey($sKey)
{
	for($i = 0; $i < StrLen($sKey); $i++)
		$aKeyIn[$i] = Ord($sKey[$i]);

	$aKeyOut[0] = $aKeyIn[0];
	$aKeyOut[1] = (($aKeyIn[0] << 7) & 255) | ($aKeyIn[1] >> 1);
	$aKeyOut[2] = (($aKeyIn[1] << 6) & 255) | ($aKeyIn[2] >> 2);
	$aKeyOut[3] = (($aKeyIn[2] << 5) & 255) | ($aKeyIn[3] >> 3);
	$aKeyOut[4] = (($aKeyIn[3] << 4) & 255) | ($aKeyIn[4] >> 4);
	$aKeyOut[5] = (($aKeyIn[4] << 3) & 255) | ($aKeyIn[5] >> 5);
	$aKeyOut[6] = (($aKeyIn[5] << 2) & 255) | ($aKeyIn[6] >> 6);
	$aKeyOut[7] =  ($aKeyIn[6] << 1) & 255;

	DESSetOddParity($aKeyOut);

	$sResult = '';
	for($i = 0; $i < Count($aKeyOut); $i++)
		$sResult .= Chr($aKeyOut[$i]);

	return $sResult;
}

function DES($sKey, $sMessage, $sInit = '12345678')
{
	global $g_iOperatingSystem;
	switch($g_iOperatingSystem)
	{
		case c_iOperatingSystemLinux:
			$sPath = c_sMCryptLinuxPath;
			break;
		case c_iOperatingSystemWindows2000:
			$sPath = c_sMCryptWindows2000Path;
			break;
	}

	$hCrypto = MCrypt_Module_Open(MCRYPT_DES, '', MCRYPT_MODE_ECB, $sPath);
	MCrypt_Generic_Init($hCrypto, SubStr($sKey, 0, 8), $sInit);
	$sEncryptedMessage = MCrypt_Generic($hCrypto, $sMessage);
	MCrypt_Generic_End($hCrypto);

	return $sEncryptedMessage;
}

function LMPassword($sPassword)
{
	if(StrLen($sPassword) > 14)
		$sPassword = '';

	$sPassword = StrToUpper($sPassword);
	for($i = StrLen($sPassword); $i < 14; $i++)
		$sPassword .= phpNul;

	$sMagic = Chr(75) . Chr(71) . Chr(83) . Chr(33) . Chr(64) . Chr(35) . Chr(36) . Chr(37);

	$sKey = SetupDESKey(SubStr($sPassword, 0, 7));
	$sHash1 = DES($sKey, $sMagic);

	$sKey = SetupDESKey(SubStr($sPassword, 7, 7));
	$sHash2 = DES($sKey, $sMagic);

	return $sHash1 . $sHash2;
}

function NTPassword($sPassword)
{
	$sUnicodePassword = '';
	for($i = 0; $i < StrLen($sPassword); $i++)
		$sUnicodePassword .= $sPassword[$i] . phpNul;

	return MD4($sUnicodePassword);
}

function TestIncludeNTAuthentication()
{
	$aTests[] = 'return MathStrToHex(LMPassword("1234567890abcdef")) == "aad3b435b51404eeaad3b435b51404ee";';
	$aTests[] = 'return MathStrToHex(NTPassword("1234567890abcdef")) == "c7e479f74e5c7c1ad963b7038f19b96d";';

	return Test('IncludeNTAuthentication', $aTests);
}

?>