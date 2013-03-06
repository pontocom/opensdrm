<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.string.php
// Date: 22.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

function BooleanToStr($bBoolean)
{
	return ($bBoolean === TRUE) ? 'TRUE' : 'FALSE';
}

function BooleanToHTML($bBoolean)
{
	if($bBoolean === TRUE)
		return '<font color="#00AA00">TRUE</font>';
	else
		return '<font color="#FF0000">FALSE</font>';
}

function StrLeft($sString, $iCount)
{
	return SubStr($sString, 0, $iCount);
}

function StrRight($sString, $iCount)
{
	return $iCount == 0 ? '' : SubStr($sString, -$iCount);
}

function StrReplace($sString, $sFrom, $sTo)
{
	return StrLen($sFrom) > 0 ? StrTr($sString, array($sFrom => $sTo)) : '';
}

function StrReplaceAll($sString, $sFrom, $sTo)
{
	//Recursive?
	if(StrPos($sTo, $sFrom) !== FALSE)
		return $sString;

	while(StrPos($sString, $sFrom) !== FALSE)
		$sString = StrReplace($sString, $sFrom, $sTo);
	return $sString;
}

function StrRemoveChars($sString, $sChars)
{
	for($i = 0; $i < StrLen($sChars); $i++)
		$sString = StrReplace($sString, SubStr($sChars, $i, 1), '');
	return $sString;
}

function Ellipse($sString, $iSize = 50, $sEllipse = '...')
{
	if(StrLen($sString) > $iSize)
		return StrLeft($sString, $iSize) . $sEllipse;
	else
		return $sString;
}

function StrPadLeft($sString, $iSize, $sPad)
{
	return Str_Pad($sString, $iSize, $sPad, STR_PAD_LEFT);
}

function StrPadRight($sString, $iSize, $sPad)
{
	return Str_Pad($sString, $iSize, $sPad, STR_PAD_RIGHT);
}

/*
function MyLevenshtein($sWord1, $sWord2)
{
	$n = StrLen($sWord1);
  $m = StrLen($sWord2);

	for($i = 0; $i <= $n; $i++)
		$x[$i][0] = $i;
	for($j = 0; $j <= $m; $j++)
		$x[0][$j] = $j;

	for($i = 1; $i <= $n; $i++)
	{
		$sChar1 = SubStr($sWord1, $i - 1, 1);
		for($j = 1; $j <= $m; $j++)
		{
			$sChar2 = SubStr($sWord2, $j - 1, 1);
			if($sChar1 == $sChar2)
				$iCost = 0;
			else
				$iCost = 1;

			$x[$i][$j] = Min($x[$i-1][$j]+1, $x[$i][$j-1]+1, $x[$i-1][$j-1] + $iCost);
		}
	}

	return $x[$n][$m];
}
*/

function StrRandom($iLength = 4, $sRandomValue = '', $aSyllables = array())
{
	if(StrLen($sRandomValue) == 0)
		$sRandomValue = CurrentTimeStamp();

	if(Count($aSyllables) == 0)
		$aSyllables = Array_Map('Chr', Range(97, 122));
	
	///*a97z122		*/

	SRand(CRC32($sRandomValue));

	$sResult = '';
	for($i = 0; $i < $iLength; $i++)
		$sResult .= $aSyllables[Rand(0, Count($aSyllables) - 1)];

	return $sResult;
}

function StrRandomSentence($iLength = 4096)
{
	$sSentence = '';
	while(StrLen($sSentence) < $iLength)
		$sSentence .= StrRandom(Rand(4, 12)) . ' ';

	return StrLeft(Trim($sSentence), $iLength);
}

function StrRandomPassword($iLength = 4, $sRandomValue = '')
{
	$aSyllables = array('ba', 'be', 'bi', 'bo', 'bu', 'da', 'de', 'di', 'do', 'du',
		'fa', 'fe', 'fi', 'fo', 'fu', 'ga', 'ge', 'gi', 'go' ,'gu', 'ha', 'he', 'hi', 'ho', 'hu',
		'ka', 'ke', 'ki', 'ko', 'ku', 'la', 'le', 'li', 'lo', 'lu', 'ma', 'me', 'mi', 'mo', 'mu',
		'na', 'ne', 'ni', 'no', 'nu', 'ra', 're', 'ri', 'ro', 'ru', 'sa', 'se', 'si', 'so', 'su',
		'ta', 'te', 'ti' ,'to', 'tu', 'wa', 'we', 'wi', 'wo', 'wu', 'za', 'ze', 'zi', 'zo', 'zu');

	return StrLeft(StrRandom($iLength, $sRandomValue, $aSyllables), $iLength);
}

function StrPrintable($sString)
{
	return PReg_Replace('/[^a-z0-9]/is', '.', $sString);
}

function StrEqualize(&$sString1, &$sString2, $sChar)
{
	$iChars = Max(StrLen($sString1), StrLen($sString2));
	
	$sString1 = StrPadLeft($sString1, $iChars, $sChar);
	$sString2 = StrPadLeft($sString2, $iChars, $sChar);
}

function StrQuote($sString)
{
	return phpQuote . $sString . phpQuote;
}


function TestIncludeString()
{
	$aTests[] = 'return BooleanToStr(TRUE) == "TRUE";';
	$aTests[] = 'return BooleanToStr(FALSE) == "FALSE";';
	$aTests[] = 'return BooleanToStr("xyz") == "FALSE";';
	$aTests[] = 'return BooleanToHTML(TRUE) == "<font color=" . phpQuote . "#00AA00" . phpQuote . ">TRUE</font>";';
	$aTests[] = 'return StrLeft("abcdef", 0) == "";';
	$aTests[] = 'return StrLeft("abcdef", 3) == "abc";';
	$aTests[] = 'return StrLeft("ab", 3) == "ab";';
	$aTests[] = 'return StrRight("abcdef", 0) == "";';
	$aTests[] = 'return StrRight("abcdef", 3) == "def";';
	$aTests[] = 'return StrRight("ab", 3) == "ab";';
	$aTests[] = 'return StrReplace("abcdef", "cd", "XY") == "abXYef";';
	$aTests[] = 'return StrReplace("abcdef", "cd", "UVWXYZ") == "abUVWXYZef";';
	$aTests[] = 'return StrReplace("abcdef", "cd", "") == "abef";';
	$aTests[] = 'return StrReplace("abcdef", "", "XY") == "";';
	$aTests[] = 'return StrReplace("-- -- ----- ---", "--", "-") == "- - --- --";';
	$aTests[] = 'return StrReplaceAll("-- -- ----- ---", "--", "-") == "- - - -";';
	$aTests[] = 'return StrReplaceAll("abcdef", "ab", "XY") == "XYcdef";';
	$aTests[] = 'return StrReplaceAll("abcdef", "ab", "abb") == "abcdef";';
	$aTests[] = 'return Ellipse("abcdefgh", 3) == "abc...";';
	$aTests[] = 'return Ellipse("abcdefgh", 20) == "abcdefgh";';
	$aTests[] = 'return Ellipse("abcdefgh", 3, "~~~") == "abc~~~";';
	$aTests[] = 'return StrRemoveChars("aaaabbbcc", "") == "aaaabbbcc";';
	$aTests[] = 'return StrRemoveChars("aaaabbbcc", "a") == "bbbcc";';
	$aTests[] = 'return StrRemoveChars("aaaabbbcc", "abc") == "";';
	$aTests[] = 'return StrPadLeft("1234", 5, "0") == "01234";';
	$aTests[] = 'return StrPadLeft("1234", 7, "0") == "0001234";';
	$aTests[] = 'return StrPadRight("1234", 5, "56789") == "12345";';
	$aTests[] = 'return StrPadRight("1234", 7, "56789") == "1234567";';
	$aTests[] = 'return StrPadRight("1234", 7, "56") == "1234565";';
	$aTests[] = 'return StrLen(StrRandom(8, "MyRandomData")) == 8;';
	$aTests[] = 'return StrLen(StrRandomPassword(8, "MyRandomData")) == 8;';
	$aTests[] = 'return StrPrintable("abcäöüabc") == "abc...abc";';
	$aTests[] = 'return StrPrintable(phpCr . phpLf . phpNul) == "...";';
	$aTests[] = 'return StrPrintable("123xyz") == "123xyz";';
	$aTests[] = '$s1 = "MyStr"; $s2 = "My"; StrEqualize($s1, $s2, "x"); return $s2 == "xxxMy";';
	$aTests[] = '$s1 = "My"; $s2 = "MyStr"; StrEqualize($s1, $s2, "x"); return $s1 == "xxxMy";';

	return Test('IncludeString', $aTests);
}

?>