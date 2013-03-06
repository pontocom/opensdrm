<?php

// function by acebone@f2s.com, a HUGE help!

function traverseXMLIZE($array, $arrName = "array", $level = 0) {

        if ( $level == 0 ) {
		echo "<pre>";
	}

        while ( list($key, $val) = each($array) ) {
                if ( is_array($val) ) {
	                traverseXMLIZE($val, $arrName . "[" . $key . "]", $level + 1);
        	} else {
                    echo '$' . $arrName . '[' . $key . '] = "' . $val . "\"\n";
                }
        }

        if ( $level == 0 ) {
			echo "</pre>";
	}
}

?>