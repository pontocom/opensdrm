<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.time.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
2^64 = 18446744073709551616 (20 Stellen)
2^32 = 4294967296 (10 Stellen)
18.01.584554 08:01:49 551:616

2^66.439 = 99999999999999999999 (20 Stellen)
2^66     = 73786976294838206464 (20 Stellen)

YYYY = Year    = 2022
YY   = Year    = 22
MMMM = Month   = Januay
MMM  = Month   = Jan
MM   = Month   = 01
MM   = Month   = 1
DD   = Day     = 01
D    = Day     = 1
hh   = Hour    = 00
h    = Mour    = 0
mm   = Minute  = 00
m    = Minute  = 0
ss   = Second  = 00
s    = Second  = 0
t    = Ticks   = 000
u    = Micro   = 000
WW   = Week    = 01
W    = Week    = 1
SSS  = WeekDay = Monday
SS   = WeekDay = Mon
S    = WeekDay = 1

*/

$g_aMonthDays = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
$g_aLeapYearMonthDays = array(31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

$g_aYearDays = array(0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334);
$g_aLeapYearYearDays = array(0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335);

function CurrentTime()
{
	$aDate1 = GetTimeOfDay();
	$aDate2 = GetDate();
	$aResult['Year'] = $aDate2['year'];
	$aResult['Month'] = $aDate2['mon'];
	$aResult['Day'] = $aDate2['mday'];
	$aResult['Hour'] = $aDate2['hours'];
	$aResult['Minute'] = $aDate2['minutes'];
	$aResult['Second'] = $aDate2['seconds'];
	$aResult['MilliSecond'] = Floor($aDate1['usec'] / 1000);
	$aResult['MicroSecond'] = $aDate1['usec'] % 1000;

	return $aResult;
}

function IsLeapYear($iYear)
{
	return (($iYear % 4 == 0) And (($iYear % 400 == 0) Or ($iYear % 100 <> 0)));
}

function MonthDays($iYear, $iMonth)
{
	global $g_aLeapYearMonthDays, $g_aMonthDays;

	if(IsLeapYear($iYear))
		return $g_aLeapYearMonthDays[$iMonth - 1];
	else
		return $g_aMonthDays[$iMonth - 1];
}

function DaysUpToYear($iYear)
{
	//Correction1 = Ceil($iYear / 4)
	//Correction2 = -(Ceil($iYear / 100) - 1)
	//Correction3 = Ceil($iYear / 400) - 1
	//Days = Year * 365 + Correction1 + Correction2 + Correction3
	return $iYear * 365 + Ceil($iYear / 4) - Ceil($iYear / 100) + Ceil($iYear / 400);
}

function DaysUpToMonth($iYear, $iMonth)
{
	global $g_aYearDays, $g_aLeapYearYearDays;

	if(IsLeapYear($iYear))
		return $g_aLeapYearYearDays[$iMonth - 1];
	else
		return $g_aYearDays[$iMonth - 1];
}

function MakeTimeStamp($iYear, $iMonth = 1, $iDay = 1, $iHour = 0, $iMinute = 0, $iSecond = 0, $iMilliSecond = 0, $iMicroSecond = 0)
{
	//Sum Days
	$aDays[] = DaysUpToYear($iYear);
	$aDays[] = DaysUpToMonth($iYear, $iMonth);
	$aDays[] = $iDay - 1;

	$sDays = Array_Reduce($aDays, 'MathAdd');

	//Sum MicroSeconds
	$aMicroSeconds[] = MathMul($sDays, '86400000000');
	$aMicroSeconds[] = MathMul($iHour, '3600000000');
	$aMicroSeconds[] = MathMul($iMinute, '60000000');
	$aMicroSeconds[] = MathMul($iSecond, '1000000');
	$aMicroSeconds[] = MathMul($iMilliSecond, '1000');
	$aMicroSeconds[] = $iMicroSecond;

	$sMicroSeconds = Array_Reduce($aMicroSeconds, 'MathAdd');

	return $sMicroSeconds;
}

function CurrentTimeStamp()
{
	$aTime = CurrentTime();
	return MakeTimeStamp($aTime['Year'], $aTime['Month'], $aTime['Day'], $aTime['Hour'], $aTime['Minute'], $aTime['Second'], $aTime['MilliSecond'], $aTime['MicroSecond']);
}

function MakeCorrectTimeStamp($iYear, $iMonth = 1, $iDay = 1, $iHour = 0, $iMinute = 0, $iSecond = 0, $iMilliSecond = 0, $iMicroSecond = 0)
{
	$iYear = Limit($iYear, 0, 99999);
	$iMonth = Limit($iMonth, 1, 12);
	$iDay = Limit($iDay, 1, MonthDays($iYear, $iMonth));
	$iHour = Limit($iHour, 0, 23);
	$iMinute = Limit($iMinute, 0, 59);
	$iSecond = Limit($iSecond, 0, 59);
	$iMilliSecond = Limit($iMilliSecond, 0, 999);
	$iMicroSecond = Limit($iMicroSecond, 0, 999);

	return MakeTimeStamp($iYear, $iMonth, $iDay, $iHour, $iMinute, $iSecond, $iMilliSecond, $iMicroSecond);
}

function IsCorrectTime($iYear, $iMonth = 1, $iDay = 1, $iHour = 0, $iMinute = 0, $iSecond = 0, $iMilliSecond = 0, $iMicroSecond = 0)
{
	if(($iYear < 0) Or ($iYear > 99999))
		return FALSE;
	
	if(($iMonth < 1) Or ($iMonth > 12))
		return FALSE;
	
	if(($iDay < 1) Or ($iDay > MonthDays($iYear, $iMonth)))
		return FALSE;
	
	if(($iHour < 0) Or ($iHour > 23))
		return FALSE;
	
	if(($iMinute < 0) Or ($iMinute > 59))
		return FALSE;
	
	if(($iSecond < 0) Or ($iSecond > 59))
		return FALSE;
	
	if(($iMilliSecond < 0) Or ($iMilliSecond > 999))
		return FALSE;

	if(($iMicroSecond < 0) Or ($iMicroSecond > 999))
		return FALSE;

	return TRUE;
}

class CTime
{
	var $m_sTimeStamp;

	function CTime()
	{
		$this->m_sTimeStamp = CurrentTimeStamp();
	}

	function Set($sTimeStamp)
	{
		$this->m_sTimeStamp = $sTimeStamp;
	}

	function Time()
	{
		return $this->m_sTimeStamp;
	}

	function Sub($sTimeStamp)
	{
		return MathSub($this->m_sTimeStamp, $sTimeStamp);
	}

	function Add($sTimeStamp)
	{
		return MathAdd($this->m_sTimeStamp, $sTimeStamp);
	}

	function Year()
	{
		$sDays = MathDiv($this->m_sTimeStamp, '86400000000');
		$iYear = MathDiv(MathMul($sDays, '400'), '146097');

		$iDays = MathDiv(MathSub($this->m_sTimeStamp, MakeTimeStamp($iYear)), '86400000000');

		if((!IsLeapYear($iYear)) And ($iDays >= 365))
			$iYear++;

		return $iYear;
	}

	function Month()
	{
		global $g_aYearDays, $g_aLeapYearYearDays;

		$iYear = $this->Year();
		$iDays = $this->YearDays();

		for($i = 0; $i < 12; $i++)
			if(IsLeapYear($iYear))
			{
				if($iDays >= $g_aLeapYearYearDays[$i]) 
					$iMonth = $i;
			}
			else
			{
				if($iDays >= $g_aYearDays[$i]) 
					$iMonth = $i;
			}

		return $iMonth + 1;
	}

	function Day()
	{
		return MathDiv(MathSub($this->m_sTimeStamp, MakeTimeStamp($this->Year(), $this->Month())), '86400000000') + 1;
	}

	function Hour()
	{
		return MathDiv(MathMod($this->m_sTimeStamp, '86400000000'), '3600000000');
	}

	function Minute()
	{
		return MathDiv(MathMod($this->m_sTimeStamp, '3600000000'), '60000000');
	}

	function Second()
	{
		return MathDiv(MathMod($this->m_sTimeStamp, '60000000'), '1000000');
	}

	function MilliSecond()
	{
		return MathDiv(MathMod($this->m_sTimeStamp, '1000000'), '1000');
	}

	function MicroSecond()
	{
		return MathMod($this->m_sTimeStamp, '1000');
	}

	function WeekDay()
	{
		return MathMod(MathAdd(MathDiv($this->m_sTimeStamp, '86400000000'), 5), '7');
	}

	function YearDays()
	{
		return MathDiv(MathSub($this->m_sTimeStamp, MakeTimeStamp($this->Year())), '86400000000');
	}

	function Week()
	{
		$iFirstDay = 3;

		$CTime = new CTime;
		for($i = 0; $i < 7; $i++)
		{
			$CTime->Set(MakeTimeStamp($this->Year(), 1, $i + 1));
			if($CTime->WeekDay() == $iFirstDay)
				$iSearchDay = $i;
		}

		$iWeek = Floor(($this->YearDays() + 7 + $iFirstDay - $iSearchDay) / 7);

		if($iWeek == 0)
			$iWeek = 53;

		return $iWeek;
	}

	function ISO()
	{
		//2000-01-01T00:00+00:00
		return $this->Format('YYYY-MM-DDThh:mm+00:00');
	}

	function Format($sFormat = 'DD.MM.YYYY hh:mm:ss')
	{
		global $u_iLanguage, $t_aWeekDays, $t_aShortWeekDays, $t_aMonths, $t_aShortMonths;

		$sResult = $sFormat;

		$iYear = $this->Year();
		$iMonth = $this->Month();
		$iDay = $this->Day();
		$iHour = $this->Hour();
		$iMinute = $this->Minute();
		$iSecond = $this->Second();
		$iMilliSecond = $this->MilliSecond();
		$iMicroSecond = $this->MicroSecond();
		$iWeek = $this->Week();
		$iWeekDay = $this->WeekDay();

		$aWeekDays = array('Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday');
		$aShortWeekDays = array('Monday#', 'Tuesday#', 'Wednesday#', 'Thursday#', 'Friday#', 'Saturday#', 'Sunday#');

		$aMonths = array('January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'Novembers', 'December');
		$aShortMonths = array('January#', 'February#', 'March#', 'April#', 'May#', 'June#', 'July#', 'August#', 'September#', 'October#', 'Novembers#', 'December#');

		$a1 = array('YYYY', 'YY', 'MMMM', 'MMM', 'MM', 'M', 'DD', 'D', 'hh', 'h', 'mm', 'm', 'ss', 's', 't', 'u', 'WW', 'W', 'SSS', 'SS', 'S');
		$a2 = array('[$@01]', '[$@02]', '[$@03]', '[$@04]', '[$@05]', '[$@06]', '[$@07]', '[$@08]', '[$@09]', '[$@10]', '[$@11]', '[$@12]', '[$@13]', '[$@14]', '[$@15]', '[$@16]', '[$@17]', '[$@18]', '[$@19]', '[$@20]', '[$@21]');

		$a3 = array(
			StrPadLeft($iYear, 4, '0'),	StrPadLeft($iYear % 100, 2, '0'),
			T($aMonths[$iMonth - 1]), T($aShortMonths[$iMonth - 1]), StrPadLeft($iMonth, 2, '0'), $iMonth,
			StrPadLeft($iDay, 2, '0'), $iDay, StrPadLeft($iHour, 2, '0'), $iHour,
			StrPadLeft($iMinute, 2, '0'), $iMinute,
			StrPadLeft($iSecond, 2, '0'), $iSecond,
			StrPadLeft($iMilliSecond, 3, '0'),
			StrPadLeft($iMicroSecond, 3, '0'),
			StrPadLeft($iWeek, 2, '0'), $iWeek,
			T($aWeekDays[$iWeekDay - 1]), T($aShortWeekDays[$iWeekDay - 1]), $iWeekDay);

		$sResult = Str_Replace($a1, $a2, $sResult);
		$sResult = Str_Replace($a2, $a3, $sResult);

		return $sResult;
	}
}

function TestClassTime()
{
	$aTests[] = '$a = CurrentTime(); return Floor($a["Year"] / 100) == 20;';
	$aTests[] = 'return IsLeapYear(0) == TRUE;';
	$aTests[] = 'return IsLeapYear(2000) == TRUE;';
	$aTests[] = 'return IsLeapYear(2004) == TRUE;';
	$aTests[] = 'return IsLeapYear(2100) == FALSE;';
	$aTests[] = 'return MonthDays(2000, 1) == 31;';
	$aTests[] = 'return MonthDays(2000, 2) == 29;';
	$aTests[] = 'return MonthDays(2001, 2) == 28;';
	$aTests[] = 'return DaysUpToYear(0) == 0;';
	$aTests[] = 'return DaysUpToYear(1) == 366;';
	$aTests[] = 'return DaysUpToYear(2) == 731;';
	$aTests[] = 'return DaysUpToMonth(2000, 1) == 0;';
	$aTests[] = 'return DaysUpToMonth(2000, 2) == 31;';
	$aTests[] = 'return DaysUpToMonth(2000, 3) == 60;';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1) == "0";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 0, 0, 0, 123, 456) == "123456";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 0, 0, 1, 0, 0) == "1000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 0, 1, 0, 0, 0) == "60000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 1, 0, 0, 0, 0) == "3600000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 1, 0, 0) == "3600000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 24) == "86400000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 2, 0) == "86400000000";';
	$aTests[] = 'return MakeTimeStamp(0, 1, 1, 23) == "82800000000";';
	$aTests[] = 'return MakeCorrectTimeStamp(0, 1, 1, 24) == "82800000000";';
	$aTests[] = 'return MakeCorrectTimeStamp(0, 1, 1, 25, -1) == "82800000000";';
	$aTests[] = 'return IsCorrectTime(0, 1, 1, 24) == FALSE;';
	$aTests[] = 'return IsCorrectTime(0, 1, 1) == TRUE;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(0, 1, 1)); return $c->WeekDay() == 5;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1000, 1, 1)); return $c->WeekDay() == 2;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1900, 1, 1)); return $c->WeekDay() == 0;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1901, 1, 1)); return $c->WeekDay() == 1;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1995, 1, 1)); return $c->WeekDay() == 6;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1996, 1, 1)); return $c->WeekDay() == 0;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1997, 1, 1)); return $c->WeekDay() == 2;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1998, 1, 1)); return $c->WeekDay() == 3;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1999, 1, 1)); return $c->WeekDay() == 4;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(2000, 1, 1)); return $c->WeekDay() == 5;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(2005, 1, 1)); return $c->WeekDay() == 5;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(2100, 1, 1)); return $c->WeekDay() == 4;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(3000, 1, 1)); return $c->WeekDay() == 2;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(9999, 1, 1)); return $c->WeekDay() == 4;';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1234, 1, 1)); return $c->Format("YY.MM.DD") == "34.01.01";';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1234, 1, 1)); return $c->Format("YYYY.MM.DD") == "1234.01.01";';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1234, 1, 1)); return $c->Format("S") == "6";';
	$aTests[] = '$c = new CTime; $c->Set(MakeTimeStamp(1234, 1, 1)); return $c->ISO() == "1234-01-01T00:00+00:00";';

	return Test('ClassTime', $aTests);
}

?>