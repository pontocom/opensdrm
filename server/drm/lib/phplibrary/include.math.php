<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.math.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
Bin => Base 2
Oct => Base 8
Dec => Base 10
Hex => Base 16
Str => Base 256
*/

function MathAdd($sOperandLeft, $sOperandRight)
{
	return BCAdd($sOperandLeft, $sOperandRight);
}

function MathSub($sOperand, $sSubtrahend)
{
	return BCSub($sOperand, $sSubtrahend);
}

function MathMul($sOperand, $sMultiplier)
{
	return BCMul($sOperand, $sMultiplier);
}

function MathDiv($sDividend, $sDivisor)
{
	return BCDiv($sDividend, $sDivisor);
}

function MathMod($sOperand, $sModulus)
{
	return BCMod($sOperand, $sModulus);
}

function MathComp($sOperandLeft, $sOperandRight)
{
	return BCComp($sOperandLeft, $sOperandRight);
}

function MathPow($qBase, $qExponent)
{
	return BCPow($qBase, $qExponent);
}

function MathPowMod($qBase, $qExponent, $qModulus)
{
	return BCPowMod($qBase, $qExponent, $qModulus);
}

function MathGCD($qOperand1, $qOperand2)
{
	return BCGCD($qOperand1, $qOperand2);
}

function MathInv($a, $n)
{
	return BCInv($a, $n);
}

function MathAnd()
{
	return Array_Reduce(Func_Get_Args(), 'BCAnd');
}

function MathOr()
{
	return Array_Reduce(Func_Get_Args(), 'BCOr');
}

function MathXor()
{
	return Array_Reduce(Func_Get_Args(), 'BCXor');
}

function MathNot($qOperand, $iBits)
{
	return BCNot($qOperand, $iBits);
}

function MathShL($qOperand, $qBits)
{
	return MathMul($qOperand, MathPow(2, $qBits));
}

function MathShR($qOperand, $qBits)
{
	return MathDiv($qOperand, MathPow(2, $qBits));
}

for($i = 0; $i < 10; $i++)
	$g_aConversion[StrVal($i)] = StrVal($i); //0-9
for($i = 0; $i < 26; $i++)
	$g_aConversion[StrVal($i + 10)] = Chr($i + 97); //a-z
for($i = 0; $i < 128; $i++)
	$g_aConversion[StrVal($i + 36)] = Chr($i + 128); //#128 - #255

// Any <=> Dec

function MathDecToAny($qNumber, $qBase)
{
	global $g_aConversion;

	$qResult = '';
	while($qNumber != '0')
	{
		$qResult = $g_aConversion[MathMod($qNumber, $qBase)] . $qResult;
		$qNumber = MathDiv($qNumber, $qBase);
	}
	return $qResult;
}

function MathAnyToDec($qNumber, $qBase)
{
	global $g_aConversion;

	$qResult = '0';
	$iStrLen = StrLen($qNumber);
	for($i = 0; $i < $iStrLen; $i++)
	{
		$iPos = Array_Search($qNumber[$iStrLen - $i - 1], $g_aConversion);
		$qResult = MathAdd($qResult, MathMul($iPos, MathPow($qBase, $i)));
	}
	return $qResult;
}

// Dec <=> Str

function MathDecToStr($qNumber)
{
	global $g_aConversion;

	$qResult = '';
	while($qNumber != '0')
	{
		$qResult = Chr(MathMod($qNumber, 256)) . $qResult;
		$qNumber = MathDiv($qNumber, 256);
	}
	return $qResult;
}

function MathStrToDec($qNumber)
{
	$qResult = '0';
	$iStrLen = StrLen($qNumber);
	for($i = 0; $i < $iStrLen; $i++)
	{
		$iNumber = Ord($qNumber[$iStrLen - $i - 1]);
		$qResult = MathAdd($qResult, MathMul($iNumber, MathPow(256, $i)));
	}
	return $qResult;
}

// Dec <=> Bin

function MathDecToBin($qNumber)
{
	$qResult = '';
	while($qNumber != '0')
	{
		$qResult = MathMod($qNumber, '2') . $qResult;
		$qNumber = MathDiv($qNumber, '2');
	}
	return $qResult;
}

function MathBinToDec($qNumber)
{
	return MathAnyToDec($qNumber, 2);
}

// Dec <=> Hex

function MathDecToHex($qNumber)
{
	return MathDecToAny($qNumber, 16);
}

function MathHexToDec($qNumber)
{
	return MathAnyToDec($qNumber, 16);
}

// Hex <=> Str

function MathHexToStr($qNumber)
{
	return MathDecToStr(MathHexToDec($qNumber));
}

function MathStrToHex($qNumber)
{
	//return MathDecToHex(MathStrToDec($qNumber));
	return Bin2Hex($qNumber);
}

function IntToDoubleWord($qNumber)
{
	return StrLeft(StrRev(StrPadLeft(MathDecToStr($qNumber), 4, phpNul)), 4);
}

function IntToWord($qNumber)
{
	return StrLeft(StrRev(StrPadLeft(MathDecToStr($qNumber), 2, phpNul)), 2);
}

function DecToAny($iValue, $iBase)
{
	global $g_aConversion;

	$sOutput = '';
	while($iValue > 0)
	{
		$sOutput = $g_aConversion[($iValue % $iBase)] . $sOutput;
		$iValue = Floor($iValue / $iBase);
	}
	return $sOutput;
}

function LowerLimit($iValue, $iLimit)
{
	if($iValue < $iLimit)
		return $iLimit;
	else
		return $iValue;
}

function UpperLimit($iValue, $iLimit)
{
	if($iValue > $iLimit)
		return $iLimit;
	else
		return $iValue;
}

function Limit($iValue, $iLowerLimit, $iUpperLimit)
{
	if($iValue < $iLowerLimit)
		return $iLowerLimit;

	if($iValue > $iUpperLimit)
		return $iUpperLimit;

	return $iValue;
}

function TestIncludeMath()
{
	$aTests[] = 'return MathShL(32, 2) == 128;';
	$aTests[] = 'return MathShR(32, 2) == 8;';
	$aTests[] = 'return MathDecToAny(255, 16) == "ff";';
	$aTests[] = 'return MathDecToAny(255, 2) == "11111111";';
	$aTests[] = 'return MathAnyToDec("ffff", 16) == 65535;';
	$aTests[] = 'return MathAnyToDec("0", 16) == 0;';
	$aTests[] = 'return MathStrToDec(Chr(255)) == 255;';
	$aTests[] = 'return MathStrToDec("\xff\xff\xff") == 16777215;';
	$aTests[] = 'return MathDecToStr(255) == Chr(255);';
	$aTests[] = 'return MathDecToStr(65535) == Chr(255) . Chr(255);';
	$aTests[] = 'return IntToDoubleWord("1234567890") == "\xd2\x02\x96\x49";';
	$aTests[] = 'return IntToWord("1234567890") == "\xd2\x02";';
	$aTests[] = 'return UpperLimit(10, 20) == 10;';
	$aTests[] = 'return UpperLimit(20, 20) == 20;';
	$aTests[] = 'return UpperLimit(30, 20) == 20;';
	$aTests[] = 'return LowerLimit(10, 20) == 20;';
	$aTests[] = 'return LowerLimit(20, 20) == 20;';
	$aTests[] = 'return LowerLimit(30, 20) == 30;';
	$aTests[] = 'return Limit(10, 20, 30) == 20;';
	$aTests[] = 'return Limit(20, 20, 30) == 20;';
	$aTests[] = 'return Limit(30, 20, 30) == 30;';
	$aTests[] = 'return Limit(40, 20, 30) == 30;';
	$aTests[] = 'return Limit(25, 30, 20) == 30;';

	return Test('IncludeMath', $aTests);
}

?>