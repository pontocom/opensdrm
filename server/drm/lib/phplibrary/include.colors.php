<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.colors.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

function RGB($iRed, $iGreen, $iBlue)
{
	$iRed = Floor($iRed) % 256;
	$iGreen = Floor($iGreen) % 256;
	$iBlue = Floor($iBlue) % 256;

	return $iRed * 65536 + $iGreen * 256 + $iBlue;
}

Define('HLSMAX', 255);
Define('RGBMAX', 255);

function HueToRGB($iN1, $iN2, $iHue)
{
	if($iHue > HLSMAX)
		$iHue -= HLSMAX;
	if($iHue < 0)
		$iHue += HLSMAX;

	if($iHue < (HLSMAX / 6))
	{
		return (($iN1 + ((($iN2 - $iN1) * $iHue + (HLSMAX / 12)) / (HLSMAX / 6))));
	}
	else
	{
		if($iHue < (HLSMAX / 2))
		{
			return $iN2;
		}
		else
		{
			if($iHue < ((HLSMAX * 2) / 3))
			{
				return (($iN1 + ((($iN2 - $iN1) * (((HLSMAX * 2) / 3) - $iHue) + (HLSMAX / 12)) / (HLSMAX / 6))));
			}
			else
			{
				return $iN1;
			}
		}
	}
}

function HLSToRGB($iHue, $iLum, $iSat)
{
	if($iSat == 0)
		$iRed = $iGreen = $iBlue = ($iLum * RGBMAX / HLSMAX);
	else
	{
		if($iLum <= (HLSMAX / 2))
			$iMagic2 = (($iLum * (HLSMAX + $iSat) + (HLSMAX / 2)) / HLSMAX);
		else
			$iMagic2 = ($iLum + $iSat - (($iLum * $iSat) + (HLSMAX / 2)) / HLSMAX);
		$iMagic1 = 2 * $iLum - $iMagic2;
		
		$iRed = ((HueToRGB($iMagic1, $iMagic2, ($iHue + (HLSMAX / 3))) * RGBMAX + (HLSMAX / 2)) / HLSMAX);
		$iGreen = ((HueToRGB($iMagic1, $iMagic2, $iHue) * RGBMAX + (HLSMAX / 2)) / HLSMAX);
		$iBlue = ((HueToRGB($iMagic1, $iMagic2, ($iHue - (HLSMAX / 3))) * RGBMAX + (HLSMAX / 2)) / HLSMAX);
	}
	return RGB($iRed, $iGreen, $iBlue);
}

function RGBToHTML($iColor)
{
	return '#' . StrPadLeft(DecToAny($iColor, 16), 6, '0');
}

function RGBBrightness($iColor)
{
	$iRed = $iColor % 256;
	$iGreen = ($iColor / 256) % 256;
	$iBlue = ($iColor / 65535) % 256;
	return Floor(($iRed + $iGreen + $iBlue) / 3);
}

Define('c_iPaletteGrey', 0);
Define('c_iPaletteGreyInvers', 1);
Define('c_iPaletteRainbow', 10);
Define('c_iPaletteRainbowLight', 11);
Define('c_iPaletteRainbowDark', 12);
Define('c_iPaletteFire', 20);
Define('c_iPaletteIce', 30);
Define('c_iPaletteNeon', 40);
Define('c_iPaletteRandom', 50);
Define('c_iPaletteRandomLight', 51);
Define('c_iPaletteRandomDark', 52);

function CreatePalette($iPalette, $iCount)
{
	$iSeed = MT_Rand();
	MT_SRand($iPalette);

	for($i = 0; $i < $iCount; $i++)
	{
		switch($iPalette)
		{
			case c_iPaletteGreyInvers:
				$iStep = Floor(255 / ($iCount - 1) * $i);
				$aPalette[$i] = RGB(255 - $iStep, 255 - $iStep, 255 - $iStep);
				break;
			case c_iPaletteRainbow:
				$iStep = Floor(255 / ($iCount - 1) * $i);
				$aPalette[$i] = HLSToRGB($iStep, 200, 255);
				break;
			case c_iPaletteRainbowLight:
				$iStep = Floor(255 / ($iCount - 1) * $i);
				$aPalette[$i] = HLSToRGB($iStep, 240, 255);
				break;
			case c_iPaletteRainbowDark:
				$iStep = Floor(255 / ($iCount - 1) * $i);
				$aPalette[$i] = HLSToRGB($iStep, 127, 255);
				break;
			case c_iPaletteFire:
				$iStep = Floor(767 / ($iCount - 1) * $i);
				if($i < ($iCount / 3))
					$aPalette[$i] = RGB($iStep, 0, 0);
				elseif($i < ($iCount * 2 / 3))
					$aPalette[$i] = RGB(255, $iStep - 256, 0);
				else
					$aPalette[$i] = RGB(255, 255, $iStep - 512);
				break;
			case c_iPaletteIce:
				$iStep = Floor(767 / ($iCount - 1) * $i);
				if($i < ($iCount / 3))
					$aPalette[$i] = RGB(0, 0, $iStep);
				elseif($i < ($iCount * 2 / 3))
					$aPalette[$i] = RGB(0, $iStep - 256, 255);
				else
					$aPalette[$i] = RGB($iStep - 512, 255, 255);
				break;
			case c_iPaletteNeon:
				$iStep = Floor(767 / ($iCount - 1) * $i);
				if($i < ($iCount / 3))
					$aPalette[$i] = RGB(0, $iStep, 0);
				elseif($i < ($iCount * 2 / 3))
					$aPalette[$i] = RGB($iStep - 256, 255, 0);
				else
					$aPalette[$i] = RGB(255, 255, $iStep - 512);
				break;
			case c_iPaletteRandom:
				$aPalette[$i] = RGB(MT_Rand(0, 255), MT_Rand(0, 255), MT_Rand(0, 255));
				break;
			case c_iPaletteRandomLight:
				$aPalette[$i] = RGB(MT_Rand(128, 255), MT_Rand(128, 255), MT_Rand(128, 255));
				break;
			case c_iPaletteRandomDark:
				$aPalette[$i] = RGB(MT_Rand(0, 128), MT_Rand(0, 128), MT_Rand(0, 128));
				break;
			default:
				$iStep = Floor(255 / ($iCount - 1) * $i);
				$aPalette[$i] = RGB($iStep, $iStep, $iStep);
				break;
		}
	}

	MT_SRand($iSeed);

	return $aPalette;
}

function TestIncludeColors()
{
	$aTests[] = 'return RGB(1, 1, 1) == 65793;';
	$aTests[] = 'return RGB(0, 0, 1) == 1;';
	$aTests[] = 'return RGB(0, 1, 0) == 256;';
	$aTests[] = 'return RGB(1, 0, 0) == 65536;';
	$aTests[] = 'return HLSToRGB(1, 2, 3) == 197121;';
	$aTests[] = 'return HLSToRGB(0, 0, 255) == 65536;';
	$aTests[] = 'return RGBBrightness(8421504) == 128;';
	$aTests[] = 'return RGBBrightness(65793) == 1;';
	$aTests[] = 'return RGBToHTML(65793) == "#010101";';
	$aTests[] = 'return RGBToHTML(8421504) == "#808080";';
	$aTests[] = 'return Count(CreatePalette(c_iPaletteFire, 32)) == 32;';
	$aTests[] = 'return Ceil(Array_Sum(Array_Map("RGBBrightness", CreatePalette(c_iPaletteGrey, 100))) / 100) == 127;';

	return Test('IncludeColors', $aTests);
}

function TestIncludeColorsOutput()
{
	$iCount = 1000;
	$iPalette = c_iPaletteRainbow;

	$aPalette = CreatePalette($iPalette, $iCount);
	$cTable = new CTable();
	for($i = 0; $i < $iCount; $i++)
	{
		$cTable->Table[0][$i] = RGBToHTML($aPalette[$i]);
		$cTable->RowStyle[$i] = 'background-color: ' . RGBToHTML($aPalette[$i]);
	}

	return $cTable->HTML();
}

?>