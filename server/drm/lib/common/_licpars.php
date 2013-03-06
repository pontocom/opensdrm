<?php   /// \file licpars.php
	/// \brief Parse a license for the MOSES project, tool for various servers
        /// \author Gregor Siegert Avanti Communications Ltd
	/// email Gregor.Siegert@avanti-communications.com	
	/// \version R1
	/// \date 14/04/03
	// Last update: 22/04/03

	// CLT = Content License Template
	// UL  = User License

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
    global $gotval;
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
}// end startElement

/// help function for the XML parser, called when data tag is found<br>
/// vals retrieves the values, must be global, gotval determines the value<br>
/// The function uses the global $gotval. and $vars<br>
/// parameter:<br>
/// in: $paraer - the parser class<br>
///     $data   - the data of the tag
function characterData($parser, $data) {
    global $gotval;
    global $vals;

    switch($gotval) {
        case 1 : $vals['version'] = $data; break;  // versiom
        case 2 : $vals['keyvalue']=$data; break;// keyinfo
        case 3 ; $vals['cid']=$data; break;// cid
        case 4 ; $vals['count'] = $data; break;// count
        case 5 ; $vals['start'] = $data; break;// start
        case 6 ; $vals['end'] = $data; break;// end
        case 7 ; $vals['uid']=$data; break;// uid
    }// end switch

    // reset flag
    $gotval = 0;
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
