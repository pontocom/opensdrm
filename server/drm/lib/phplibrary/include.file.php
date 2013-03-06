<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.file.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

function DeleteFile($sFileName)
{
	if(File_Exists($sFileName))
		UnLink($sFileName);
}

function CreateZIPFile($sFileName, $sData)
{
	$hFile = GZOpen($sFileName, 'wb9');
	GZWrite($hFile, $sData);
	GZClose($hFile);
}

function GetZIPFile($sFileName)
{
	$sData = '';
	$hFile = GZOpen($sFileName, 'rb');
	while(!GZEoF($hFile))
		$sData .= GZRead($hFile, 4096);
	GZClose($hFile);
	return $sData;
}

function CreateFile($sFileName, $sData)
{
	$hFile = FOpen($sFileName, 'wb');
	FWRite($hFile, $sData);
	FClose($hFile);
}

function AppendFile($sFileName, $sData)
{
	$hFile = FOpen($sFileName, 'ab');
	FWRite($hFile, $sData);
	FClose($hFile);
}

function GetFile($sFileName)
{
	$sData = '';
	$hFile = FOpen($sFileName, 'rb');
	while(!FEoF($hFile))
		$sData .= FRead($hFile, 4096); 
	FClose($hFile);
	return $sData;
}

function GetFileAsArray($sFileName)
{
	$sData = StrRemoveChars(GetFile($sFileName), phpCr);
	return Explode(phpNewLine, $sData);
}

function CreateIniFile($sFileName, $aEntries)
{
	$sData = '';
	while(List($sSection, $aItems) = Each($aEntries))
	{
		$sData .= '[' . UCFirst(StrToLower($sSection)) . ']' . phpCrLf;
		while(List($sKey, $sValue) = Each($aItems))
			$sData .= $sKey . '=' . $sValue . phpCrLf;
	}
	CreateFile($sFileName, $sData);
}

function GetIniFile($sFileName)
{
	$sSection = '-';
	foreach(GetFileAsArray($sFileName) as $sLine)
	{
		$sLine = Trim($sLine);
		if(StrLen($sLine) > 0)
			if(SubStr($sLine, 0, 1) == '[')
				$sSection = StrToUpper(SubStr($sLine, 1, -1));
			else
				$aEntries[$sSection][SubStr($sLine, 0, StrPos($sLine, '='))] = 
					SubStr($sLine, StrPos($sLine, '=') + 1);
	}
	return $aEntries;
}

function FileName($sFileName)
{
	return SubStr($sFileName, StrRPos($sFileName, '/') + 1);
}

function FilePath($sFileName)
{
	return SubStr($sFileName, 0, StrRPos($sFileName, '/') + 1);
}

function FileExtension($sFileName)
{
	if(StrPos($sFileName, '.') !== FALSE)
		return StrToUpper(SubStr($sFileName, StrRPos($sFileName, '.') + 1, StrLen($sFileName)));
	else
		return '';
}

function UniqueFileName($sPrefix = '', $sSufix = 'txt')
{
	do
	{
		$sFileName = $sPrefix . StrPadLeft(CurrentTimeStamp(), 20, '0') . '.' . $sSufix;
	} while(File_Exists($sFileName));
	return $sFileName;
}

function FileFormat($sProbe)
{
	$iPositionUnix = StrPos($sProbe, phpLf);
	$iPositionWindows = StrPos($sProbe, phpCrLf);

  //!CrLf -> Unix
	if($iPositionWindows === FALSE)
		return phpLf;

  //!CrLf & !Lf -> Windows | Unix
	if($iPositionUnix === FALSE)
		return phpLf;

  //Lf < CrLf -> Unix
	if($iPositionUnix < $iPositionWindows)
		return phpLf;

	//Windows
	return phpCrLf;
}

function CorrectPath($sPath)
{
	if(StrLen($sPath) == 0 Or StrRight($sPath, 1) == '/')
		return $sPath;
	else
		return $sPath . '/';
}

function CreateDirectory($sPath)
{
	$sCurrentPath = '';
	foreach(Explode('/', CorrectPath($sPath)) as $sPathSlice)
	{
		$sCurrentPath .= $sPathSlice . '/';
		if(!File_Exists($sCurrentPath))
			MkDir($sCurrentPath, 0);
	}
}

function RemoveDirectory($sPath)
{
	$sCurrentPath = '';
	$aPathSlices = Explode('/', CorrectPath($sPath));
	for($i = Count($aPathSlices) - 1; $i > 0; $i--)
	{
		$sCurrentPath = Implode('/', Array_Slice($aPathSlices, 0, $i));
		if(File_Exists($sCurrentPath))
			RmDir($sCurrentPath);
	}
}

function FilesAsList($sPath, $bRecursive = FALSE)
{
	$sPath = CorrectPath($sPath);
	$hDirectory = OpenDir($sPath);
	$aList = array();
	while($sFileName = ReadDir($hDirectory))
	{ 
		if ($sFileName != '.' And $sFileName != '..')
		{ 
			$sFullName = $sPath . $sFileName;
			if(Is_Dir($sPath . $sFileName))
			{
				if($bRecursive)
					$aList = Array_Merge($aList, FilesAsList($sFullName, $bRecursive));
			}
			else
				$aList[] = $sFullName;
		}
	}
	CloseDir($hDirectory);
	return $aList;
}

function DirectoriesAsList($sPath, $bRecursive = FALSE)
{
	$sPath = CorrectPath($sPath);
	$hDirectory = OpenDir($sPath); 
	$aList = array();
	while($sFileName = ReadDir($hDirectory))
	{ 
		if ($sFileName != '.' And $sFileName != '..')
		{ 
			$sFullName = $sPath . $sFileName;
			if(Is_Dir($sPath . $sFileName))
			{
				$aList[] = $sFullName;
				if($bRecursive)
					$aList = Array_Merge($aList, DirectoriesAsList($sFullName, $bRecursive));
			}
		} 
	}
	CloseDir($hDirectory); 
	return $aList;
}

function TestIncludeFile()
{
	$aTests[] = 'CreateFile("MyFile", "MyData"); return GetFile("MyFile") == "MyData";';
	$aTests[] = 'CreateFile("MyFile", "My"); AppendFile("MyFile", "Data"); return GetFile("MyFile") == "MyData";';
	$aTests[] = 'CreateZIPFile("MyFile", "MyData"); return GetZIPFile("MyFile") == "MyData";';
	$aTests[] = 'CreateFile("MyFile", "a" . phpCrLf . "b"); return GetFileAsArray("MyFile") == array("a", "b");';
	$aTests[] = 'CreateFile("MyFile", "a" . phpLf . "b"); return GetFileAsArray("MyFile") == array("a", "b");';
	$aTests[] = '$aData = array("SECTION" => array("Name" => "Value")); CreateIniFile("MyFile", $aData); return GetIniFile("MyFile") == $aData;';
	$aTests[] = 'DeleteFile("MyFile"); return TRUE;';
	$aTests[] = 'return FileName("/path/filename.dot.ext") == "filename.dot.ext";';
	$aTests[] = 'return FilePath("/path/filename.dot.ext") == "/path/";';
	$aTests[] = 'return FileExtension("/path/filename.dot.ext") == "EXT";';
	$aTests[] = 'return StrLen(UniqueFileName()) == 24;';
	$aTests[] = 'return CorrectPath("") == "";';
	$aTests[] = 'return CorrectPath("/path") == "/path/";';
	$aTests[] = 'return CorrectPath("/path/") == "/path/";';
	$aTests[] = 'CreateDirectory("a/b/c/d/e/f"); return File_Exists("a/b/c/d/e/f") == TRUE;';
	$aTests[] = 'RemoveDirectory("a/b/c/d/e/f"); return File_Exists("a/b/c/d/e/f") == FALSE;';
	$aTests[] = 'return Count(FilesAsList("", TRUE)) > 50;';
	$aTests[] = 'return Count(DirectoriesAsList("", TRUE)) >= 0;';

	return Test('IncludeFile', $aTests);
}

?>