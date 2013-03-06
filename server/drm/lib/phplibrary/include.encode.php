<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.encode.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
Encoding Against Sniffers

You

1. Encrypt with secure AES
2. Encode with delightful 'Encode'
3. Compress
4. Encode with Big Brother's well known algorithm

Attacker

1. Check - Ahh - The stupid person has used my algorithm
2. Check - Ahhhh - Only compressed
3. Check - Simple HTML - Let it be

*/

$g_aEncoding = array(
	'<h1>Title</h1>', '<h2>SubTitle</h2>', '<h3>MikroTitle</h3>', '<h4>NanoTitle</h4>', '<h5>PicoTitle</h5>', '<h6>FemtoTitle</h6>',
	'<font color="red">Important</font>', '<a href="Hyperlink">Hyperlink</a>', '<code>Code</code>', '<pre>PreFormat</pre>', '<p>Paragraph</p>', '<center>Center</center>', '<blockquote>BlockQuote</blockquote>', '<dir>Directory</dir>',
	'<b>Bold</b>', '<i>Italic</i>', '<u>Underline</u>', '<big>Big</big>', '<cite>Cite</cite>', '<sub>Sub</sub>', '<sup>Sup</sup>',
	'<script language="JavaScript"></script>',
	'<hr>', '<br>',
	'<!--  -->', '&amp;', '&copy;', '&reg;', '&trade;', '&nbsp;',
	'&alpha;', '&beta;', '&gamma;', '&delta;',
	'&Alpha;', '&Beta;', '&Gamma;', '&Delta;',
	'<ul><li>UnorderedList</li></ul>', '<ol><li>OrderedList</li></ol>', '<dl><dd>DefinitionList</dd></dl>',
	'<form></form>', '<input type="text">', '<select></select>', '<input type="radio">', '<input type="checkbox">', '<textarea></textarea>', '<input type="reset">', '<input type="submit">', '<input type="password">', '<input type="hidden">', '<input type="file">'
);

function Encode($sString)
{
	global $g_aEncoding, $g_aConversion;

	$sString = MathDecToAny(MathStrToDec($sString), Count($g_aEncoding));
	
	$sResult = '';
	for($i = 0; $i < StrLen($sString); $i++)
		$sResult .= $g_aEncoding[Array_Search(SubStr($sString, $i, 1), $g_aConversion)];

	return $sResult;
}

function Decode($sString)
{
	global $g_aEncoding, $g_aConversion;

	$sString = Str_Replace($g_aEncoding, $g_aConversion, $sString);

	$sString = MathDecToStr(MathAnyToDec($sString, Count($g_aEncoding)));

	return $sString;
}

function TestIncludeEncode()
{
	$aTests[] = 'return Encode("A") == "<h2>SubTitle</h2><dir>Directory</dir>";';
	$aTests[] = 'return Encode("B") == "<h2>SubTitle</h2><b>Bold</b>";';
	$aTests[] = 'return Encode("C") == "<h2>SubTitle</h2><i>Italic</i>";';

	return Test('IncludeEncode', $aTests);
}

?>