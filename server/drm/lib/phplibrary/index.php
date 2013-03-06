<?
///////////////////////////////////////////////////////////////////////////////
// Filename: index.php
// Date: 28.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

?>
<html>
<head>
<title>PHP Library</title>
<style>
body, p, td { font-family: tahoma, verdana, arial; font-size: 10pt; vertical-align: top; }
td.Content { background-color: skyblue; }
td.Caption { background-color: darkblue; color: white; }
td.Dark { background-color: #3333ff; }
td.Light { background-color: #aaaaff; }
table.Error { border: 1px solid black; padding: 5px; background-color: #FF8080; width: 100%; }
table.Warning {	border: 1px solid black; padding: 5px; background-color: #FFFF80; width: 100%; }
table.Information { border: 1px solid black; padding: 5px; background-color: #80FF80; width: 100%; }
</style>
</head>
<body>
<h1>PHP Library</h1>
<?

Set_Time_Limit(120);

require('include.const.php');
require('include.debug.php');
require('include.string.php');
require('include.file.php');
require('include.translation.php');
require('include.bcmath.php');
require('include.math.php');
require('include.prime.php');
require('include.rsa.php');
require('include.colors.php');
require('include.encode.php');
require('include.ntauthentication.php');
require('include.html.php');

require('class.table.php');
require('class.wave.php');
require('class.aes.php');
require('class.mersennetwister.php');
require('class.random.php');
require('class.md4.php');
require('class.rsa.php');
require('class.time.php');
require('class.log.php');
require('class.timer.php');
require('class.cache.php');
require('class.rtf.php');
require('class.spell.php');
require('class.pdf.php');
require('class.image.php');
require('class.client.php');
require('class.database.php');
require('class.email.php');
require('class.ldap.php');

$g_aTranslation = GetTranslationFile('translation.txt', 'German');

//Debug('PHPMiniDebugger', '127.0.0.1');
//W('OperatingSystem', $g_iOperatingSystem);

//echo TestIncludeString();
//echo TestIncludeFile();
//echo TestIncludeTranslation();
//echo TestIncludeTranslationOutput(); 
//echo TestIncludeBCMath();
//echo TestIncludeMath();
//echo TestIncludePrime();
//echo TestIncludeRSA();
//echo TestIncludeColors();
//echo TestIncludeColorsOutput();
//echo TestIncludeEncode();
//echo TestIncludeNTAuthentication();
//echo TestIncludeHTML();

//echo TestClassTable();
//echo TestClassWave();
//echo TestClassAES();
//echo TestClassMersenneTwister();
//echo TestClassRandom();
//echo TestClassMD4();
echo TestClassRSA();
//echo TestClassTime();
//echo TestClassLog();
//echo TestClassTimer();
//echo TestClassCache();
//echo TestClassRTF();
//echo TestClassSpell();
//echo TestClassPDF();
//echo TestClassImage();
//echo TestClassClient();
//echo TestClassDatabase();
//echo TestClassLDAP();
//echo TestClassEMail();

/*
[Functions]

Dump
Debug
Test
W
D
IsData
Swap
Execute

BooleanToStr
BooleanToHTML
StrLeft
StrRight
StrReplace
StrReplaceAll
StrRemoveChars
Ellipse
StrPadLeft
StrPadRight
StrRandom
StrRandomPassword
StrRandomSentence
StrPrintable
StrEqualize
StrQuote

DeleteFile
CreateZIPFile
GetZIPFile
CreateFile
AppendFile
GetFile
GetFileAsArray
CreateIniFile
GetIniFile
FileName
FilePath
FileExtension
UniqueFileName
FileFormat
CorrectPath
CreateDirectory
RemoveDirectory
DirectoriesAsList
FilesAsList

GetTranslationFile
T
TranslationStatistics

BCPowMod
BCInv
BCGCD
BCShL
BCShR
BCAnd
BCOr
BCXor
BCNot

MathAdd
MathMul
MathSub
MathDiv
MathPow
MathMod
MathComp

MathPowMod
MathInv
MathGCD
MathShL
MathShR
MathAnd
MathOr
MathXor
MathNot

MathDecToAny
MathAnyToDec

MathDecToStr
MathStrToDec

MathDecToBin
MathBinToDec

MathDecToHex
MathHexToDec

MathHexToStr
MathStrToHex

DecToAny

IntToDoubleWord
IntToWord

LowerLimit
UpperLimit
Limit

IsPrimeFermat
SearchPrime

RSAKeys
RSAE
RSAD
RSAS
RSAV

HLSToRGB
RGB
RGBToHTML
RGBBrightness
CreatePalette

MD4
MD4Hex

CurrentTime

IsLeapYear
MonthDays
DaysUpToYear
DaysUpToMonth

MakeTimeStamp
CurrentTimeStamp
MakeCorrectTimeStamp
IsCorrectTime

Encode
Decode

LMPassword
NTPassword

MMToPSP

ScanPort

ValidEMail

Attributes
Tag
ShortTag
Hyperlink
EMail
Anchor
Bold
Font
Div
Pixel
Image
Checked
InputCheckBox
InputHidden
JavaScriptFromSource
JavaScript
JavaScriptHyperlink
InputTextArea
InputText
InputPassword
InputFile
InputSubmit
InputSubmitImage
Form
Paragraph
NewLine
Line
Space
Span
OptionBoxDefault
InputListBoxString
InputListBox
Error
Warning
Information
QueryString
QueryStringAdd

[Classes]

CAES
CCache
CClient
CPOPClient
CSMTPClient
CHTTPClient
CTIMEClient
CDatabase
CImage
CLDAP
CLog
CMersenneTwister
CRandom
CPDF
CRSA
CRTF
CTable
CTime
CMD4
CTimer
CSpell
CWave

[Constants]

phpNul
phpTab
phpLf
phpCr
phpEsc
phpSpace
phpQuote
phpAmp
phpSingleQuote
phpSlash
phpBackSlash
phpQuestionMark
phpNewLine
phpCrLf

c_iDatabaseConnectionODBC
c_iDatabaseConnectionMySQL
c_iDatabaseConnectionMSSQL
c_iDatabaseConnectionInterbase
c_iDatabaseConnectionOracle
c_iDatabaseConnectionIngres
c_iDatabaseConnectionMSQL
c_iDatabaseConnectionSybase
c_iDatabaseConnectionSesam

c_iDatabaseTypeAny
c_iDatabaseTypeMySQL
c_iDatabaseTypeMSSQL
c_iDatabaseTypeInterbase
c_iDatabaseTypeOracle
c_iDatabaseTypeIngres
c_iDatabaseTypeMSQL
c_iDatabaseTypeSybase
c_iDatabaseTypeSesam

c_iAlignLeft
c_iAlignRight
c_iAlignCenter
c_iAlignJustification

c_iOperatingSystemUnknown
c_iOperatingSystemLinux
c_iOperatingSystemWindows2000
*/

?>
</body>
</html>