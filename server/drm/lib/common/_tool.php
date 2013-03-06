<?php   /// \file tool.php
        /// \brief implements call and convinient integartion for CFS location <br>
        /// resolving into other servers <br>
        /// \author Gregor Siegert Avanti Communications Ltd
        /// email Gregor.Siegert@avanti-communications.com        
        /// \version R1
        /// \date 24/05/03

        // usage: require_once('tool.php');        
        // getLocation("MDS"); // or whatever shall be found
        

        // include nusoap.php, must eb in the same directory
        require_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
        require_once($_SERVER["DOCUMENT_ROOT"]."/lib/nusoap/"."nusoap.php");

        /// resolve location from configuration server
        function getLocation($id) {
                // set soap endpoint, this address shall be the only one hardcoded
                // in MOSES

                // for lisbon meeting
                $endpt =implode("",array('http://','127.0.0.1','/opensdrmWS/CFS/CFS.php'));
                //$endpt =implode("",array('http://','Gregor','/CFS/CFS.php'));

                // for now the endpoint is set statically
                $client = new soapclient($endpt);

                // set the paramter for soap
                $params = array('id'=>$id);

                // call the RPC
                $response = $client->call("CFSrequestServerLocation",$params,$ns);

                // parse the response and retrieve the location
                $location = $response['location'];

                // print out location or set error message in log fields
                if(empty($location)) return new soap_fault('Server','','AUS location can not be resolved');
    
                // return the location to the caller
                return $location;
        }// end getLocation
        
        // perform hex to bin reconversion
        function convertHexToBin($msg) {
                 // reconversion, verified
                for($a=0;$a<strlen($msg);$a+=2) {
                      $sub = substr($msg,$a,2);
                      $tmp =  sscanf("$sub","%x");
                      if($tmp<=0) $msga = $msga.chr($sub[1]);
                      else $msga = $msga.chr($tmp[0]);
                }
                return $msga;
        }// end reconvertHexToBin

        // perform bin to hex conversion
        function convertBinToHex($msg) {
                // conversion, verified
                for($a=0;$a<strlen($msg);$a++) {
                      if(ord($msg[$a])<16) {$msga = $msga . "0";}
                      $msga = $msga.sprintf("%x",ord($msg[$a]));
                }
                return $msga;
        }// end convertBinToHex
?>
