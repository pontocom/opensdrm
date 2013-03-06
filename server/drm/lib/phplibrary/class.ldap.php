<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.ldap.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
[Active Directory]
UserDN => cn=Administrator,cn=users,dc=domain,dc=net
BaseDN => cn=users,dc=domain,dc=net

[OpenLDAP]
UserDN => cn=Manager,dc=domain,dc=net
BaseDN => dc=domain,dc=net
*/

class CLDAP
{
	var $m_hLDAP, $m_sBaseDN, $m_iPort, $m_bVersion3;

	function CLDAP()
	{
		$this->m_sBaseDN = '';
		$this->m_iPort = 389;
		$this->m_bVersion3 = FALSE;
	}

	function Close()
	{
		LDAP_Close($this->m_hLDAP);
	}

	function Connect($sHost, $sUserDN = '', $sPassword = '')
	{
		$this->m_hLDAP = LDAP_Connect($sHost, $this->m_iPort);

		if($this->m_bVersion3)
			LDAP_Set_Option($this->m_hLDAP, LDAP_OPT_PROTOCOL_VERSION, 3);
		
		LDAP_Bind($this->m_hLDAP, $sUserDN, $sPassword);
	}

	function BaseDN($sBaseDN)
	{
		$this->m_sBaseDN = $sBaseDN;
	}

	function CorrectDN($sDN)
	{
		$sDN = Trim($sDN);

		if(StrLen($sDN) == 0)
			return '';

		if($sDN[0] == ',')
			$sDN[0] = ' ';
		if($sDN[StrLen($sDN) - 1] == ',')
			$sDN[StrLen($sDN) - 1] = ' ';

		$sDN = Trim($sDN);
		return $sDN;
	}

	function CreateDN($sBaseDN, $sRDN)
	{
		return $this->CorrectDN($this->CorrectDN($sRDN) . ',' . $this->CorrectDN($sBaseDN));
	}

	function Entries($sRDN = '')
	{
		$hResult = LDAP_Read($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), 'objectclass=*');
		return LDAP_Get_Entries($this->m_hLDAP, $hResult);
	}

	function SubEntries($sRDN = '')
	{
		$hResult = LDAP_List($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), 'objectclass=*');
		return LDAP_Get_Entries($this->m_hLDAP, $hResult);
	}

	function Search($sRDN = '')
	{
		$hResult = LDAP_Search($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), 'objectclass=*');
		return LDAP_Get_Entries($this->m_hLDAP, $hResult);
	}

	function GetAttribute($sRDN = '', $sAttribute)
	{
		$hResult = LDAP_Read($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), 'objectclass=*');
		$hEntry = LDAP_First_Entry($this->m_hLDAP, $hResult);
		return LDAP_Get_Values($this->m_hLDAP, $hEntry, $sAttribute);
	}

	function GetAttributeBinary($sRDN = '', $sAttribute)
	{
		$hResult = LDAP_Read($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), 'objectclass=*');
		$hEntry = LDAP_First_Entry($this->m_hLDAP, $hResult);
		return LDAP_Get_Values_Len($this->m_hLDAP, $hEntry, $sAttribute);
	}

	function AddAttribute($sRDN, $sAttribute, $sValue)
	{
		$aEntry[$sAttribute] = $sValue; 
		$iResult = LDAP_Mod_Add($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), $aEntry); 
	}

	function DelAttribute($sRDN, $sAttribute, $sValue = array())
	{
		$aEntry[$sAttribute] = $sValue; 
		$iResult = LDAP_Mod_Del($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), $aEntry); 
	}

	function AddEntry($sRDN, $aEntry)
	{
		$iResult = LDAP_Add($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN), $aEntry);
	}

	function DelEntry($sRDN)
	{
		$iResult = LDAP_Delete($this->m_hLDAP, $this->CreateDN($this->m_sBaseDN, $sRDN));
	}
}

/*
$cLDAP = new CLDAP;
$cLDAP->Connect('192.168.1.64','cn=Manager,dc=domain,dc=net', 'secret');
$cLDAP->BaseDN('cn=Users,dc=chcms,dc=net');

$aEntry['objectClass'] = 'inetOrgPerson';
$aEntry['cn'] = 'Thomas Stauffer';
$aEntry['sn'] = 'Stauffer';
$aEntry['givenName'] = 'Thomas';
$aEntry['initials'] = 'TS';
$aEntry['physicalDeliveryOfficeName'] = 'Buero';
$aEntry['telephoneNumber'] = 'Telefon G';
$aEntry['mail'] = 'EMail';
$aEntry['postalAddress'] = 'Strasse G';
$aEntry['l'] = 'Stadt G';
$aEntry['st'] = 'KantonBundesland G';
$aEntry['postalCode'] = 'PLZ G';
$aEntry['homePhone'] = 'Telefon P';
$aEntry['pager'] = 'Pager G';
$aEntry['mobile'] = 'Telefon N';
$aEntry['facsimileTelephoneNumber'] = 'Fax G';
$aEntry['title'] = 'Position';
$aEntry['ou'] = 'Abteilung';
$aEntry['o'] = 'Firmenname';
$aEntry['labeledURI'] = 'http://www.deepsource.ch';
$aEntry['homePostalAddress'] = 'Addresse G';

$cLDAP->AddEntry('cn=Thomas Stauffer', $aEntry);

$cLDAP->Close();
*/

function TestClassLDAP()
{
	//$aTests[] = '$cLDAP = new CLDAP; $cLDAP->BaseDN("dc=net"); return $cLDAP->CreateDN("dc=domain") == ;';
	$aTests[] = '$cLDAP = new CLDAP; return $cLDAP->CreateDN("dc=net", "dc=domain") == "dc=domain,dc=net";';
	$aTests[] = '$cLDAP = new CLDAP; return $cLDAP->CreateDN(",dc=net,", "dc=domain") == "dc=domain,dc=net";';
	$aTests[] = '$cLDAP = new CLDAP; return $cLDAP->CreateDN("dc=net", ",dc=domain,") == "dc=domain,dc=net";';
	$aTests[] = '$cLDAP = new CLDAP; return $cLDAP->CreateDN(",dc=net,", ",dc=domain,") == "dc=domain,dc=net";';
	$aTests[] = 'return 1 == 1;';
	$aTests[] = 'return 1 == 1;';

	return Test('ClassLDAP', $aTests);
}

?>