<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.rtf.php
// Date: 24.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

function RTFTag($sData, $sTag = '')
{
	$sData = StrReplace($sData, '\\', '\\\\');
	$sData = StrReplace($sData, phpTab, '\\tab ');
	$sData = StrReplace($sData, phpLf, '');
	$sData = StrReplace($sData, phpCr, '\\par ');
	$sData = StrReplace($sData, '{', '\\{');
	$sData = StrReplace($sData, '}', '\\}');

	if(IsData($sTag))
		return '{\\' . $sTag. ' ' . $sData . '}';
	else
		return $sData;
}

class CRTF
{
	var $m_sRTF;	

	function RTF()
	{
		$this->m_sRTF = '';
	}

	function Add($sData)
	{
		$this->m_sRTF .= $sData;
	}

	function RTF()
	{
		return '{\\rtf1\\ansi\\ansicpg1252 ' . $this->m_sRTF . '}';
	}
}

/*

b     Bold
i     Italic
ul    Underline
super SuperScript
sub   SubScript
scaps SmallCaps

*/

/*
$cRTF = new CRTF;
$sData = RTFTag('a', 'i') . RTFTag('2', 'super') . RTFTag('+') . RTFTag('b', 'i') . RTFTag('2', 'super') . RTFTag('=') . RTFTag('c', 'i') . RTFTag('2', 'super');
$cRTF->Add($sData);
CreateFile('MyRichTestFormat.rtf', $cRTF->RTF());
*/

function TestClassRTF()
{
	$aTests[] = '$cRTF = new CRTF; return StrToUpper($cRTF->RTF()) == "{\RTF1\ANSI\ANSICPG1252 }";';

	return Test('ClassRTF', $aTests);
}

?>