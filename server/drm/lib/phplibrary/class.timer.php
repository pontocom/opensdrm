<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.timer.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CTimer
{
	var $m_sTimerStart, $m_sTimerStop;

	function CTimer()
	{
		$this->m_sTimerStart = '0';
		$this->m_sTimerStop = '0';
		$this->Start();
	}

	function Start()
	{
		$this->m_sTimerStart = CurrentTimeStamp();
	}

	function Stop()
	{
		$this->m_sTimerStop = CurrentTimeStamp();
	}

	function Delay()
	{
		return MathSub($this->m_sTimerStop, $this->m_sTimerStart);
	}

	function StopEcho()
	{
		$this->Stop();

		$cDate = new CDate;
		$cDate->Set($this->Delay());
		echo $cDate->Format('hh:mm:ss.t.u') . '<br>' . phpCrLf;
	}
}

function TestClassTimer()
{
	$aTests[] = '$cTimer = new CTimer; $cTimer->Stop(); return $cTimer->Delay() < 100000;';

	return Test('ClassTimer', $aTests);
}


?>