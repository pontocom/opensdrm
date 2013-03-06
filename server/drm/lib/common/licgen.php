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

// get the license parser
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/licpars.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");

// (1) set without uid (CLT creation)

/*
$vals = array('version'=>'1.0',
              'keyvalue'=>'12345678123456781234567812345678',
              'cid'=>'1234567890ABCDEF',
              'count'=>'1',
              'start'=>'00000101',
              'end'=>'99993112',
              'uid'=>'');

// (2) set with uid (UL creation)
*/

/*
$vals = array('version'=>'1.0',
              'keyvalue'=>'12345678123456781234567812345678',
              'cid'=>'1234567890ABCDEF',
              'count'=>'1',
              'start'=>'00000101',
              'end'=>'99993112',
              'uid'=>'1234');
*/

// demo with (1), verified GS, 22/04/03
//createCLT($vals);
// demo with (2), verified GS, 22/04/03
//createUL($vals);

// demo with CLT created with (2), verified GS, 22/04/03
//createCLT_from_Template('/test.xml','aaaaaaaabbbbbbbbccccccccdddddddd','aaaabbbbccccdddd',1);
// demo with CLT created with (2), verified GS, 22/04/03
//createUL_from_Template('/test.xml','1111','121201','130102');

/// creates and inserts a tag in the xml license<br>
/// parameter:<br>
/// in: $doc    - document class<br>
///     $parent - parent node<br>
///     $name   - name of the node to be created<br>
///     $ns     - namespace, shall be URI<br>
///     $value  - when not false, the value to insert<br>
/// out: $doc<br>
/// return: $temp a copy of the new object
function insert($doc,$parent,$name,$ns=false,$value=false) {
        // create new node with $name
        $temp = $doc->create_element($name);
        // append child to parent node
        $temp = $parent->append_child($temp);
        // get the prefix for matching uri
        if($ns) $temp->set_namespace ($ns);
        // set value, if available
        if($value) $temp->set_content($value);
        return $temp;
}// end insert

/// create a CLT which supplies content binding<br>
/// function expects array to function<br>
/// paramater:<br>
/// in: $vals = array('version'=>Int,<br>
///             'keyvalue'=>char[32],<br>
///             'cid'=>char[16],<br>
///              'count'=>Int,<br>
///              'start'=>Int,<br>
///              'end'=>Int,<br>
///              'uid'=>Int);<br>
/// return: -1 on error, array has then 6 entries<br>
///         (without uid)
function createCLT($vals) {
		$l=new Logger();
		$l->Log("estou no createCLT");
        // error when less then 6 values supplied
        if(count($vals) < 6){
        	$l->Log("menores que 6");
        	return -1;
        }
          // create new xml document
        $doc = domxml_new_doc('1.0');
        // create root element "rights"
        $rights = $doc->create_element_ns('http://odrl.net/1.1/ODRL-EX','rights','o-ex');
        // add additional namespace
        $rights->add_namespace ('http://odrl.net/1.1/ODRL-DD','o-dd');
        $rights->add_namespace ('http://www.w3.org/2000/09/xmlds','ds');
        // append root element "rights"
        $rights = $doc->append_child($rights);

        $context = insert($doc,$rights,'context','http://odrl.net/1.1/ODRL-EX');
        // set the version element
        insert($doc,$context,'version','http://odrl.net/1.1/ODRL-DD',$vals['version']);
        //set the agreement element
        $agreement = insert($doc,$rights,'agreement','http://odrl.net/1.1/ODRL-EX');
        // set the asset element
        $asset = insert($doc,$agreement,'asset','http://odrl.net/1.1/ODRL-EX');
        // set the keyInfo element
        $keyinfo = insert($doc,$asset,'keyinfo','http://www.w3.org/2000/09/xmlds');
        // set the keyvalue element
        insert($doc,$keyinfo,'keyvalue','http://www.w3.org/2000/09/xmlds',$vals['keyvalue']);
        // set the context element
        $context = insert($doc,$asset,'context','http://odrl.net/1.1/ODRL-EX');
        // set the uid element
        insert($doc,$context,'uid','http://odrl.net/1.1/ODRL-DD',$vals['cid']);
        // set the permission element
        $permission = insert($doc,$agreement,'permission','http://odrl.net/1.1/ODRL-EX');
        // set the play element
        $play = insert($doc,$permission,'play','http://odrl.net/1.1/ODRL-DD');
        // set the constraint element
        $constraint = insert($doc,$play,'constraint','http://odrl.net/1.1/ODRL-EX');
        // set the count element
		//if ($vals['count']!=0) {
        //	insert($doc,$constraint,'count','http://odrl.net/1.1/ODRL-DD',$vals['count']);
		//}
        //insert($doc,$constraint,'count','http://odrl.net/1.1/ODRL-DD',$vals['count']);
        // set the datetime element
        $l->Log("estou no aqui...");

        $datetime = insert($doc,$constraint,'datetime','http://odrl.net/1.1/ODRL-DD');
		if($vals['end']!=0) {
	        // set the start element
	        insert($doc,$datetime,'start','http://odrl.net/1.1/ODRL-DD',$vals['start']);
	        // set the end element
        	insert($doc,$datetime,'end','http://odrl.net/1.1/ODRL-DD',$vals['end']);
		} else {
        	insert($doc,$constraint,'count','http://odrl.net/1.1/ODRL-DD',$vals['count']);
		}
        // set the individual element
        if($vals['uid'])
        	insert($doc,$constraint,'individual','http://odrl.net/1.1/ODRL-DD',$vals['uid']);

        // dump xml stack to screen for debugging
        // echo "<PRE>";
        // echo htmlentities($doc->dump_mem(true));
        // echo "</PRE>";

        // dump xml stack into output file
        if(!$vals['uid']) {
	        $str = $vals['cid'];
	        $str=str_pad($str,1+strlen($str),'/',STR_PAD_LEFT);
	        $str=str_pad($str,strlen('.test.xml')+strlen($str),'.test.xml',STR_PAD_RIGHT);
        }
        else {
	        $str = $vals['uid'];
	        $str=str_pad($str,1+strlen($str),'/',STR_PAD_LEFT);
	        $str=str_pad($str,strlen('.test.xml')+strlen($str),'.test.xml',STR_PAD_RIGHT);
        }
        $l->Log("estou no aqui2...");

        $doc->dump_file($str, false, true);

        $l->Log("$doc");

        return $doc->dump_mem(true);
}// end createCLT


/// create a UL which supplies user binding<br>
/// same as createCTL but checks uid field of the array<br>
/// function expects array to function<br>
/// paramater:<br>
/// in: $vals = array('version'=>Int,<br>
///              'keyvalue'=>char[32],<br>
///              'cid'=>char[16],<br>
///              'count'=>Int,<br>
///              'start'=>Int,<br>
///              'end'=>Int,<br>
///              'uid'=>Int);<br>
/// return: -1 on error, uid is empty
function createUL($vals) {
		$l=new Logger();
		$l->Log("estou no createUL");
        // error
        if(!$vals['uid'] || !$vals['start'] || !$vals['end']) {
        	$l->Log("retorna -1");
        	return -1;
        }
        $l->Log("vou entrar no createCLT");
        return createCLT($vals);
}// end createUL


/// create a CLT which supplies content binding<br>
/// function expects template to function<br>
/// paramater:<br>
/// in: $name - license file name<br>
///     $ckey - the content key<br>
///     $cid  -  the content id<br>
///     $count - count of play allowed<br>
/// return: -1 on error, internal error, not a valid template
function createCLT_from_Template($name,$ckey,$cid,$count) {

        // init vals
        $vals = initVals();
        $_SESSION['vals'] = $vals;

        // check input parameter
        if(!file_exists($name) ||
            empty($count) || !$count ||
            empty($ckey) || !$ckey || strlen($ckey) < 32 ||
            empty($cid) || !$cid || strlen($cid) < 16) return -1;

        // get XML file (generated)
        $fp = fopen($name,'r');
        $data = fread($fp,filesize($name));
        fclose($fp);

        // get $vals from template check vals
        if(parseCLT($data)==-1) return -1;
        $vals = $_SESSION['vals'];
        //   $vals['count'] == 0 ||
        if($vals['start'] == 0 ||
           $vals['end'] ==0) return -1;

        // set remaining paramters
        $vals['count'] = $count;
        $vals['keyvalue'] = $ckey;
        $vals['cid'] = $cid;

        // create content License Template
        return createCLT($vals);

}//end createCLT_from_Template

/// create a UL which supplies user binding<br>
/// function expects a content template to function<br>
/// paramater:<br>
/// in: $name - license file name<br>
///     $uid  -  the content id<br>
///     $start - start date of the license<br>
///     $end - expire date of the license<br>
/// return: -1 on error, internal error, not a valid template
function createUL_from_Template($name,$uid,$start,$end) {

        // init vals
        $vals = initVals();
        $_SESSION['vals'] = $vals;

        // check input parameter
        if(!file_exists($name) ||
            empty($start) || !$start ||
            empty($end) || !$end ||
            empty($uid)|| !$uid) return -1;

        // get XML file (generated)
        $fp = fopen($name,'r');
        $data = fread($fp,filesize($name));
        fclose($fp);

        // get $vals from template check vals
        if(parseCLT($data)==-1) return -1;
        $vals = $_SESSION['vals'];
        if(!$vals['count'] ||
           !$vals['start'] ||
           !$vals['end'] ||
           !$vals['keyvalue'] || strlen($vals['keyvalue']) < 32 ||
           !$vals['cid'] || strlen($vals['cid']) < 16)  return -1;

        // set remaining paramters
        $vals['start'] = $start;
        $vals['end'] = $end;
        $vals['uid'] = $uid;

        // create content License Template
        return createUL($vals);

}//end createCLT_from_Template
?>
