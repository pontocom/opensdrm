<?php
/*
 OpenSDRM - Open Secure Digital Rights Management
 Copyright (c) 2002 Adetti

 This software was originaly developed under the MOSES project (IST-2001-34144)

________                         _________________ __________    _____
\_____  \ ______   ____   ____  /   _____/\______ \\______   \  /     \
 /   |   \\____ \_/ __ \ /    \ \_____  \  |    |  \|       _/ /  \ /  \
/    |    \  |_> >  ___/|   |  \/        \ |    `   \    |   \/    Y    \
\_______  /   __/ \___  >___|  /_______  //_______  /____|_  /\____|__  /
        \/|__|        \/     \/        \/         \/       \/         \/

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA


*/

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/CFS/configCFS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/X509Certificate.class.php");

function dumpArguments($trigger)
{
	echo "Wrong parameters\n";
	echo "Correct usage: ";
	switch ($trigger)
	{
		case 1: echo "php cfs_admin.php setup LOGIN PASSWORD\n\n";
			break;
		case 2: echo "php cfs_admin.php add componentID componentURL\n";
			echo "example: php cfs_admin.php add AUS http://www.aus.com/aus.ws.php\n\n";
			break;
		case 3: echo "php cfs_admin.php list\n\n";
			break;
		case 4: echo "php cfs_admin.php detail componentID\n";
			echo "example: php cfs_admin.php detail AUS\n\n";
			break;
		case 5: echo "php cfs_admin.php delete componentID";
			echo "example: php cfs_admin.php delete AUS\n\n";
			break;
		case 6: echo "php cfs_admin.php alter componentID component URL";
			echo "example: php cfs_admin.php alter http://www.aus.com/aus.ws.phpAUS\n\n";
			break;
		default: echo "php cfs_admin.php setup|add|list|detail|delete|alter LOGIN PASSWORD\n\n";
			break;
	}
		
}

function cfsSetup($login, $password)
{
	$pCFS = new PrivateKey();
	$pCFS->create();
}

function cfsAddNewComponent($componentID, $componentURL, $login, $password)
{
	if($componentID=='' || $componentURL=='') 
	{
		dumpArguments(2);
		return ;
	}
}

function cfsListComponents($login, $password)
{
}

function cfsDetailComponent($componentID, $login, $password)
{
	if($componentID=='')
	{
		dumpArguments(4);
	}
}

function cfsDeleteComponent($componentID, $login, $password)
{
	if($componentID=='')
	{
		dumpArguments(5);
	}
}

function cfsAlterComponent($componentID, $componentURL, $login, $password)
{
	if($componentID=='' || $componentURL=='')
	{
		dumpArguments(6);
	}
}

var_dump($argv);
if($argc<2)
{
	dumpArguments(-1);
} else 
{
	switch ($argv[1])
	{
		case 'setup': cfsSetup($argv[2], $argv[3]); 
			break;
		case 'add': cfsAddNewComponent($argv[2], $argv[3], $argv[4], $argv[5]);
			break;
		case 'list': cfsListComponents($argv[2], $argv[3]);
			break;
		case 'detail': cfsDetailComponent($argv[2], $argv[3], $argv[4]);
			break;
		case 'delete': cfsDeleteComponent($argv[2], $argv[3], $argv[4]);
			break;
		case 'alter': cfsAlterComponent($argv[2], $argv[3], $argv[4], $argv[5]);
			break;
		default: dumpArguments(-1);
			break;
	}
}
?>