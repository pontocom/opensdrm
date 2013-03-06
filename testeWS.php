<?php
$client = new
SoapClient('http://reservas.pousadas.pt/ServicesWeb/services/ListaPousadasOper/wsdl/ListaPousadasOper.wsdl', array('trace' => 1));

//$result = new stdClass();
$result = $client->getListaPousadas();
//$result = $client->__soapCall('getListaPousadas', array());
//var_dump($result);
/*print "<pre>";
print_r($result);
print "</pre>";*/
//echo "RESPONSE:\n" . $client->__getLastResponse() . "\n";
var_dump($result->getListaPousadasReturn[0]->nome);
?>