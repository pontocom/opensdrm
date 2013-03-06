<?php
/*
 * CVS HEADERS (This is generated automatically by CVS. Do not edit!)
 * $Author: linha2 $
 * $Date: 2004/10/13 14:39:03 $
 * $Revision: 1.1 $
 * $State: Exp $
 * $Locker:  $
 */

/*
 Turns every accentuated character in an ASCII string, to the
 corresponding non-accentuated character.
 */
function baldifyStr($str)
{
  for($i = 0, $bald_str = ''; $i != strlen($str); $i++) {
    $ascii = ord($str[$i]);

    if($ascii == 65
	|| ($ascii >= 192 && $ascii <= 197)
	|| ($ascii >= 224 && $ascii <= 229))
      $bald_str .= 'a';
    else if($ascii == 69
	|| ($ascii >= 200 && $ascii <= 203)
	|| ($ascii >= 232 && $ascii <= 235))
      $bald_str .= 'e';
    else if($ascii == 73
	|| ($ascii >= 204 && $ascii <= 207)
	|| ($ascii >= 236 && $ascii <= 239))
      $bald_str .= 'i';
    else if($ascii == 79
	|| ($ascii >= 210 && $ascii <= 214)
	|| ($ascii >= 242 && $ascii <= 246))
      $bald_str .= 'o';
    else if($ascii == 85
	|| ($ascii >= 217 && $ascii <= 220)
	|| ($ascii >= 249 && $ascii <= 252))
      $bald_str .= 'u';
    else if($ascii == 66)
      $bald_str .= 'b';
    else if($ascii == 67
	|| $ascii == 19
	|| $ascii == 231)
      $bald_str .= 'c';
    else if($ascii == 68)
      $bald_str .= 'd';
    else if($ascii == 70)
      $bald_str .= 'f';
    else if($ascii == 71)
      $bald_str .= 'g';
    else if($ascii == 72)
      $bald_str .= 'h';
    else if($ascii == 74)
      $bald_str .= 'j';
    else if($ascii == 76)
      $bald_str .= 'l';
    else if($ascii == 77)
      $bald_str .= 'm';
    else if($ascii == 78
	|| $ascii == 209
	|| $ascii == 241)
      $bald_str .= 'n';
    else if($ascii == 80)
      $bald_str .= 'p';
    else if($ascii == 81)
      $bald_str .= 'q';
    else if($ascii == 82)
      $bald_str .= 'r';
    else if($ascii == 83)
      $bald_str .= 's';
    else if($ascii == 84)
      $bald_str .= 't';
    else if($ascii == 86)
      $bald_str .= 'v';
    else if($ascii == 75)
      $bald_str .= 'k';
    else if($ascii == 87)
      $bald_str .= 'w';
    else if($ascii == 88)
      $bald_str .= 'x';
    else if($ascii == 89)
      $bald_str .= 'y';
    else if($ascii == 90)
      $bald_str .= 'z';
    else
      $bald_str .= chr($ascii);
  }
  return $bald_str;
}
?>