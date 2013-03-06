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
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/common/DatabaseHandler.class.php');

class tableDrawer
{
	var $border;
	var $t_b_color;
	var $h_c_b_color;
	var $h_c_t_color;
	var $n_c_b_color;

	function tableDrawer() {
	}

	function draw($titleArr, $contentArr, $url) {
		echo "<table border=$this->border>";
		$this->drawHeader($titleArr);
		$num_linhas = count($contentArr);
		for($l=0; $l<$num_linhas;$l++) {
			echo "<tr>";
			echo "<td><input type=checkbox name=opd[] value=".$contentArr[$l][0]."></td>";
			$num_cols=count($contentArr[$l]);
			for($c=0; $c<$num_cols; $c++) {
				//echo "<td>[$l][$c]</td>";
				if($c==0) {
					echo "<td><a href=\"".$url."id=".$contentArr[$l][$c]."\">".$contentArr[$l][$c]."</a></td>";
				} else {
					echo "<td>".$contentArr[$l][$c]."</td>";
				}
			}
			echo "</tr>";
		}
		echo "</table>";
	}

	function drawHeader($titleArr) {
		echo "<tr>";
		echo "<td></td>";
		$n=0;
		while($n<count($titleArr)) {
			echo "<td bgcolor=$this->h_c_b_color><font color=$this->h_c_t_color><b>$titleArr[$n]</b></td>";
			$n++;
		}
		echo "</tr>";
	}
}
?>