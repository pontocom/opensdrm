<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.log.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CLog
{
	var $m_sFileName, $m_sPrefix, $m_bLogging, $m_iFileSize;

	function CLog()
	{
		$this->m_sFileName = 'log.txt';
		$this->m_sPrefix = '';
		$this->m_bLogging = TRUE;
		$this->m_iFileSize = 1024 * 1024; //Byte

		if(File_Exists($this->m_sFileName) And FileSize($this->m_sFileName) > $this->m_iFileSize)
			DeleteFile($this->m_sFileName);
	}

	function SetFileName($sFileName)
	{
		$this->m_sFileName = $sFileName;
	}

	function Prefix($sPrefix)
	{
		$this->m_sPrefix = $sPrefix;
	}

	function Write($sString)
	{
		if($this->m_bLogging)
		{
			$cTime = new CTime;
			$cTime->Set(CurrentTimeStamp());

			$sOutput = '<entry>';
			$sOutput .= '<date>' . $cTime->Format('DD.MM.YYYY') . '</date>';
			$sOutput .= '<time>' . $cTime->Format('hh:mm:ss') . '</time>';
			$sOutput .= '<prefix>' . $this->m_sPrefix . '</prefix>';
			$sOutput .= '<string>' . $sString . '</string>';
			$sOutput .= '</entry>' . phpCrLf;

			AppendFile($this->m_sFileName, $sOutput);
		}
	}

	function On()
	{
		$this->m_bLogging = TRUE;
	}

	function Off()
	{
		$this->m_bLogging = FALSE;
	}
}

function TestClassLog()
{
	$aTests[] = '$cLog = new CLog; $cLog->Write("MyLog"); return FileSize("log.txt") > 90;';
	$aTests[] = 'DeleteFile("log.txt"); return TRUE;';

	return Test('ClassLog', $aTests);
}
?>