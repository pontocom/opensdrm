<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.html.php
// Date: 28.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

function Attributes()
{
	$sResult = '';
	for($i = 0; $i < Func_Num_Args(); $i+=2)
		$sResult .=  ' ' . Func_Get_Arg($i) . '="' . Func_Get_Arg($i + 1) . '"';
	return $sResult;
}

function Tag($sTag, $sContent = '')
{
	$sAttributes = '';
	for($i = 2; $i < Func_Num_Args(); $i+=2)
		if(IsData(Func_Get_Arg($i + 1)))
			$sAttributes .=  ' ' . Func_Get_Arg($i) . '="' . Func_Get_Arg($i + 1) . '"';

	return '<' . $sTag . $sAttributes . '>' . $sContent . '</' . $sTag . '>';
}

function ShortTag($sTag)
{
	$sAttributes = '';
	for($i = 1; $i < Func_Num_Args(); $i+=2)
		if(IsData(Func_Get_Arg($i + 1)))
			$sAttributes .=  ' ' . Func_Get_Arg($i) . '="' . Func_Get_Arg($i + 1) . '"';

	return '<' . $sTag . $sAttributes . '>';
}

function Hyperlink($sCaption, $sURL, $sTarget = '', $sClass = '')
{
	return Tag('a', $sCaption, 'target', $sTarget, 'class', $sClass, 'href', $sURL);
}

function EMail($sCaption, $sEMail, $sTarget = '', $sClass = '')
{
	return Hyperlink($sCaption, 'mailto:' . $sEMail, $sTarget, $sClass);
}

function Anchor($sCaption, $sName, $sTarget = '')
{
	return Tag('a', $sCaption, 'target', $sTarget, 'name', $sName);
}

function Bold($sContent)
{
	return Tag('b', $sContent);
}

function Font($sContent, $sColor = '', $sFamily = '')
{
	return Tag('font', $sContent, 'color', $sColor, 'face', $sFamily);
}

function Div($sContent, $sID = '', $bDisplay = TRUE, $sOnClick = '')
{
	if(!$bDisplay)
		$sDisplay = 'display : none;';
	else
		$sDisplay = '';

	return Tag('div', $sContent, 'id', $sID, 'style', $sDisplay, 'onclick', $sOnClick);
}

function Pixel()
{
	return Image('pixel.gif', '1x1 Pixel', 1, 1);
}

function Image($sURL, $sHint = '', $iWidth, $iHeight)
{
	return ShortTag('img', 'alt', $sHint, 'width', $iWidth, 'height', $iHeight, 'src', $sURL);
}

function Checked(&$sCheckBoxValue)
{
	return IsData($sCheckBoxValue);
}

function InputCheckBox($sName, $bChecked = FALSE)
{
	if($bChecked)
		$sChecked = ' checked';
	else
		$sChecked = '';

	return '<input name="' . $sName . '" type="checkbox" class="CheckBox"' . $sChecked . '>';
}

function InputHidden($sName, $sValue)
{
	return ShortTag('input', 'type', 'hidden', 'name', $sName, 'value', $sValue);
}

function JavaScriptFromSource($sFileName)
{
	return  Tag('script', '', 'language', 'javascript', 'src', $sFileName);
}

function JavaScript($sJavaScript)
{
	return Tag('script', '<!--' . phpCrLf . $sJavaScript . phpCrLf . '//-->', 'language', 'javascript');
}

function JavaScriptHyperlink($sJavaScript)
{
	return 'javascript:' . StrReplace($sJavaScript, phpQuote, phpSingleQuote);	
}

function InputTextArea($sName, $sValue = '')
{
	return Tag('textarea', $sValue, 'scrolling', 'no', 'name', $sName, 'id', $sName);
}

function InputText($sName, $sValue = '', $sOnKeyUp = '')
{
	return ShortTag('input', 'type', 'text', 'class', 'text', 'name', $sName, 'id', $sName, 'value', HTMLSpecialChars($sValue), 'onkeyup', $sOnKeyUp);
}

function InputPassword($sName)
{
	return ShortTag('input', 'type', 'password', 'class', 'password', 'name', $sName);
}

//10485760 = 10 MegaByte
function InputFile($sName, $iMaxSize = 10485760)
{
	return ShortTag('input', 'type', 'file', 'class', 'file', 'name', $sName) . InputHidden('MAX_FILE_SIZE', $iMaxSize);
}

function InputSubmit($sValue = '', $iWidth = 200, $iHeight = 24)
{
	return ShortTag('input', 'type', 'submit', 'class', 'submit', 'value', $sValue);
}

function InputSubmitImage($sURL, $iWidth = 16, $iHeight = 16)
{
	return '<input type="image" class="Hyperlink" src="' . $sURL . '" width="' . $iWidth . '" height="' . $iHeight . '">';
}

function Form($sName, $sURL, $sData, $sOnSubmit = '')
{
	return Tag('form', $sData, 'method', 'post', 'enctype', 'multipart/form-data', 'name', $sName, 'action', $sURL, 'onsubmit', $sOnSubmit);
}

function Paragraph($sText)
{
	return Tag('p', $sText);
}

function NewLine($iCount = 1)
{
	return Str_Repeat('<br>' . phpCrLf, $iCount);
}

function Line()
{
	return '<hr>';
}

function Space($iCount = 1)
{
	return Str_Repeat('&nbsp;', $iCount);
}

function Span($sData, $sID)
{
	return Tag('span', $sData, 'id', $sID);
}

function OptionBoxDefault($sOptions, $sDefault)
{
	return StrReplace($sOptions, 'value="' . $sDefault . '"', 'value="' . $sDefault . '" selected');
}

function InputListBoxString($sName, $sOptions, $sDefault = '', $sOnChange = '')
{
	$sMyOption = '';
	foreach(Explode(';', $sOptions) as $sItem)
		$sMyOption .= '<option value="' . $sItem . '">' . $sItem . '</option>';

  if(IsData($sDefault))
		$sMyOption = OptionBoxDefault($sMyOption, $sDefault);

	return Tag('select', $sMyOption, 'name', $sName, 'onchange', $sOnChange);
}

//$aOptions = array('Name' => '?', 'Value' => '?', 'Color' => '?', 'BackgroundColor' => '?');
function InputListBox($sName, $aOptions, $sDefault = '', $sSize = '1', $sOnChange = '')
{
	$sHTML = '';
	foreach($aOptions as $aOption)
	{
		$sCaption = $aOption['Name'];

		//Null value doesn't be sent if 'Value' ist left out
		$sValue = ' value="' . $aOption['Value'] . '"';

		if(IsData($aOption['Color']))
			$sColor = ' color : ' . $aOption['Color'] . ';';
		else
			$sColor = '';

		if(IsData($aOption['BackgroundColor']))
			$sBackgroundColor = ' background-color : ' . $aOption['BackgroundColor'] . ';';
		else
			$sBackgroundColor = '';

		if(IsData($sColor) Or IsData($sBackgroundColor))
			$sStyle = ' style="' . $sColor . $sBackgroundColor . '"';
		else
			$sStyle = '';

		$sHTML .= '<option' . $sValue . $sStyle . '>' . $sCaption . '</option>';
	}

  if(IsData($sDefault))
		$sHTML = OptionBoxDefault($sHTML, $sDefault);

	return Tag('select', $sHTML, 'name', $sName, 'size', $sSize, 'onchange', $sOnChange);
}

function Error($sError)
{
	return '<table border="0" class="Error" cellpadding="0" cellspacing="0" width="0">' .
		'<tr><td width="20">' . Image('error.gif', 'Error', 16, 16) . '</td><td><b>' . $sError . '</b></td></tr>' .
		'</table>';
}

function Warning($sWarning)
{
	return '<table border="0" class="Warning" cellpadding="0" cellspacing="0" width="0">' .
		'<tr><td width="20">' . Image('warning.gif', 'Warning', 16, 16) . '</td><td><b>' . $sWarning . '</b></td></tr>' .
		'</table>';
}

function Information($sInformation)
{
	return '<table border="0" class="Information" cellpadding="0" cellspacing="0" width="0">' .
		'<tr><td width="20">' . Image('information.gif', 'Information', 16, 16) . '</td><td><b>' . $sInformation . '</b></td></tr>' .
		'</table>';
}

//filename.ext?Arg1=Arg2&Arg3=Arg4&...
function QueryString($sFileName)
{
	$sResult = $sFileName . '?';
	for($i = 1; $i < Func_Num_Args(); $i++)
	{
		if(($i % 2) == 1)
			$sResult .= Func_Get_Arg($i) . '=';
		else
			$sResult .= Func_Get_Arg($i) . '&';

	}
	return SubStr($sResult, 0, -1);
}

//Arg1=Arg2&Arg3=Arg4&...
function QueryStringAdd()
{
	$sResult = '';
	for($i = 0; $i < Func_Num_Args(); $i++)
	{
		if(($i % 2) == 0)
			$sResult .= '&' . Func_Get_Arg($i) . '=';
		else
			$sResult .= Func_Get_Arg($i);

	}
	return $sResult;
}

/*
echo Error('Error');
echo Warning('Warning');
echo Information('Information');
*/

function TestIncludeHTML()
{
	$aTests[] = 'return Tag("font", "MyText", "face", "Arial") == "<font face=" . phpQuote . "Arial" . phpQuote . ">MyText</font>";';
	$aTests[] = 'return QueryString("filename") == "filename";';
	$aTests[] = 'return QueryString("filename", "a", 1, "b", 2) == "filename?a=1&b=2";';
	$aTests[] = 'return QueryStringAdd("a", 1, "b", 2) == "&a=1&b=2";';

	return Test('IncludeHTML', $aTests);
}

?>