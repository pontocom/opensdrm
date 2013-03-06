<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.translation.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

$g_aTranslation = array();

$g_aTranslationStatistics = array();

Define('c_sTranslationReplacer', '[...]');

function GetTranslationFile($sFileName, $sLanguage)
{
	$aInputLines = GetFileAsArray($sFileName);

	$iColumn = Array_Search($sLanguage, Explode(phpTab, $aInputLines[0]));

	for($i = 1; $i < Count($aInputLines); $i++)
  {
		if(StrLen($aInputLines[$i]) > 0)
		{
			$aTemp = Explode(phpTab, $aInputLines[$i]);
			$aOutput[$aTemp[0]] = $aTemp[$iColumn];
		}
	}
	return $aOutput;
}

function TranslationStatistics()
{
	global $g_aTranslation, $g_aTranslationStatistics;

	$cTable = new CTable;

	while(List($sKey, $sValue) = Each($g_aTranslation))
	{
		$iRow = $cTable->Rows();
		$sContent = '';
		$sRowStyle = '';

		if(IsData($g_aTranslationStatistics[$sKey]))
		{
			$sRowStyle .= 'font-weight: bold;';
			$sContent = $g_aTranslationStatistics[$sKey];
		}

		if(!IsData($g_aTranslation[$sKey]))
			$sRowStyle .= 'color: red;';

		$cTable->Table[0][$iRow] = $sKey;
		$cTable->Table[1][$iRow] = $sContent;
		$cTable->RowStyle[$iRow] = $sRowStyle;
	
	}

	echo $cTable->HTML();
}

//Translate
function T($sID)
{
	global $g_aTranslation, $g_aTranslationStatistics;

	if(IsData($g_aTranslationStatistics[$sID]))
		$g_aTranslationStatistics[$sID] = 1;
	else
		$g_aTranslationStatistics[$sID]++;

	return IsData($g_aTranslation[$sID]) ? $g_aTranslation[$sID] : c_sTranslationReplacer;
}

function TestIncludeTranslationOutput()
{
	echo TranslationStatistics();
}

function TestIncludeTranslation()
{
	$aTests[] = 'return T("TestDummy") == "TestDummy";';
	$aTests[] = 'return T("NoEntryInThisFile") == "' . c_sTranslationReplacer . '";';

	return Test('IncludeTranslation', $aTests);
}

?>