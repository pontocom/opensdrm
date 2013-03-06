<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.wave.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CWave
{
	var $m_sData, $m_bMono, $m_b8Bit, $m_iFrequency;

	function CWave()
	{
		$this->m_bMono = TRUE;
		$this->m_b8Bit = TRUE;
		$this->m_iFrequency = 11025;
		$this->m_sData = '';
	}

	function AddData($sData)
	{
		$this->m_sData .= $sData;
	}

	function AddRaWFile($sFileName)
	{
		$this->m_sData .= GetFile($sFileName);
	}

	function Duration()
	{
		$fTime = StrLen($this->m_sData);
		$fTime /= $this->m_bMono ? 1 : 2;
		$fTime /= $this->m_b8Bit ? 1 : 2;
		$fTime /= $this->m_iFrequency;

		return $fTime;
	}

	function Wave()
	{
		$iSize = StrLen($this->m_sData);
		$iChannels = $this->m_bMono ? 1 : 2;
		$iResolution = $this->m_b8Bit ? 1 : 2;
		$iBytesPerSecond = $this->m_iFrequency * $iChannels * $iResolution;
		$iBytesPerSample = $iChannels * $iResolution;
		$iBitsPerSample = $this->m_b8Bit ? 8 : 16;

		$sWave = "\x52\x49\x46\x46" . IntToDoubleWord($iSize + 36) . "\x57\x41\x56\x45\x66\x6D\x74\x20\x10\x00\x00\x00\x01\x00" . IntToWord($iChannels) . IntToDoubleWord($this->m_iFrequency) . IntToDoubleWord($iBytesPerSecond) . IntToWord($iBytesPerSample) . IntToWord($iBitsPerSample) . "\x64\x61\x74\x61" . IntToDoubleWord($iSize);
		$sWave .= $this->m_sData;

		return $sWave;
	}
}

function TestClassWave()
{
	$aTests[] = '$cWave = new CWave; return $cWave->Duration() == 0;';
	$aTests[] = '$cWave = new CWave; return StrLen($cWave->Wave()) == 44;';

	return Test('ClassWave', $aTests);
}

?>