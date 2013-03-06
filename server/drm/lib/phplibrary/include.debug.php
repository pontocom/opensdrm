<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.debug.php
// Date: 22.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

Define('c_iOperatingSystemUnknown', 0);
Define('c_iOperatingSystemLinux', 1);
Define('c_iOperatingSystemWindows2000', 2);

if(File_Exists('/tmp'))
	$g_iOperatingSystem = c_iOperatingSystemLinux;
elseif(File_Exists('c:/winnt/explorer.exe'))
	$g_iOperatingSystem = c_iOperatingSystemWindows2000;
else
	$g_iOperatingSystem = c_iOperatingSystemUnknown;

function Debug($sText, $sIP = '127.0.0.1', $iPort = 6680)
{
	$hSocket = FSockOpen($sIP, $iPort, $iError, $sError, 5);
  FWrite($hSocket, $sText . phpCrLf);
  FClose($hSocket);
}

function Dump($xVariable)
{
	switch(GetType($xVariable))
	{
		case 'boolean':
			return BooleanToStr($xVariable);
		case 'array':
			$cTable = new CTable;
			while(List($sKey, $xValue) = Each($xVariable))
			{
				$iRow = $cTable->Rows();
				$cTable->Table[0][$iRow] = '[' . $sKey . ']';
				$cTable->Table[1][$iRow] = '= ' . Dump($xValue);
			}
			$cTable->Width = '';
			return $cTable->HTML();
		case 'integer':
		case 'string':
		case 'double':
			return $xVariable;
		default:
			return GetType($xVariable);
	}
}

//Write
function W()
{
	$aArguments = Func_Get_Args();
	for($i = 0; $i < Count($aArguments); $i++)
		$aArguments[$i] = Dump($aArguments[$i]);
	echo Implode('&nbsp;|&nbsp;', $aArguments) . '<br>' . phpCrLf;
}

function Swap(&$xVariable1, &$xVariable2)
{
	$xTemp = $xVariable1;
	$xVariable1 = $xVariable2;
	$xVariable2 = $xTemp;
}

function IsData(&$xVariable)
{
	if(IsSet($xVariable))
	{
		if(GetType($xVariable) == 'string' And StrLen($xVariable) < 1)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

//Default
function D(&$xVariable, $xDefault = '')
{
	if(IsData($xVariable))
		return $xVariable;
	else
		return $xDefault;
}

function Execute($sCommand)
{
	return `$sCommand`;
}

function Test($sCaption, $aTests)
{
	$cTable = new CTable;

	for($i = 0; $i < Count($aTests); $i++)
	{
		$sResult = BooleanToStr(Eval($aTests[$i]));

		$cTable->Table[0][$i] = HTMLSpecialChars($aTests[$i]);
		$cTable->Table[1][$i] = $sResult;
	}
	$cTable->ColWidth[1] = 100;

	return '<b>' . $sCaption . '</b><br>' . $cTable->HTML();
}

?>