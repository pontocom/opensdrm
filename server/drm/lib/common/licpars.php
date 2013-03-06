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

/// init function for the value array<br>
/// parameter:<br>
/// return: $temp a copy of the new array<br>
function initVals() {
        $temp = array('version'=>0,'keyvalue'=>0,'cid'=>0,
                      'count'=>0,'start'=>0,'end'=>0,'uid'=>0);
        return $temp;
}// end initVals

// ---example
// init parser variables
//$vals = initVals();

// get XML file (generated)
//$name = '/test.xml';
//$fp = fopen($name,'r');
//$data = fread($fp,filesize($name));
//fclose($fp);

// get $vals from license file
//if(parseCLT($data)!=-1) print_r($vals);
// end example---

/// help function for the XML parser the function chances the<br>
/// global: $gotval<br>
/// parameter:<br>
/// in: $parser - parser class<br>
///     $name   - tag name<br>
///     $attrs  - array of attributes
function startElement($parser, $name, $attrs) {
    // got version
    if(strstr(strtolower($name),'version')) $gotval = 1;
    // got keyvalue
    elseif(strstr(strtolower($name),'keyvalue')) $gotval = 2;
    // got uid
    elseif(strstr(strtolower($name),'uid')) $gotval = 3;
    // got num of plays
    elseif(strstr(strtolower($name),'count')) $gotval = 4;
    // got start
    elseif(strstr(strtolower($name),'start')) $gotval = 5;
    // got end
    elseif(strstr(strtolower($name),'end')) $gotval = 6;
    // got individual
    elseif(strstr(strtolower($name),'individual')) $gotval = 7;
    // nothin ...
    else $gotval = 0;
    $_SESSION['gotval'] = $gotval;
}// end startElement

/// help function for the XML parser, called when data tag is found<br>
/// vals retrieves the values, must be global, gotval determines the value<br>
/// The function uses the global $gotval. and $vars<br>
/// parameter:<br>
/// in: $paraer - the parser class<br>
///     $data   - the data of the tag
function characterData($parser, $data) {
    $gotval = $_SESSION['gotval'];

    switch($gotval) {
        case 1 : $vals['version'] = $data; break;  // versiom
        case 2 : $vals['keyvalue']=$data; break;// keyinfo
        case 3 ; $vals['cid']=$data; break;// cid
        case 4 ; $vals['count'] = $data; break;// count
        case 5 ; $vals['start'] = $data; break;// start
        case 6 ; $vals['end'] = $data; break;// end
        case 7 ; $vals['uid']=$data; break;// uid
    }// end switch

    $_SESSION['vals'] = $vals;
    // reset flag
    $_SESSION['gotval'] = 0;
}// characterData

/// this function must not be used in this case, required init of the parser
function endElement($parser, $name) {}

/// parser function for templates parses the data, initializes and starts the<br>
/// expat file parser.<br>
/// parameter:<br>
/// in: $data - chunk of text, includes XML (well defined)<br>
/// return: -1 on parser error or empty data
function parseCLT($data) {

    // error
    if(empty($data)) return -1;
    // response is xml format
    $xml_parser = xml_parser_create();

    // set message handler
    xml_set_element_handler($xml_parser, 'startElement','endElement');
    xml_set_character_data_handler($xml_parser, 'characterData');

    $done = true;

    // parse the xml data
    if(!empty($data))
          if (!XML_Parse($xml_parser, $data, $done))
                        return -1;

}// end parseCLT
?>
