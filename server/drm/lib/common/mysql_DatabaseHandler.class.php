<?
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

/*
Class DatabaseHandler
*/
//include_once(getenv('OPENSDRM_INSTALL_DIR')."config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");

/** DatabaseHandler class
To handle all the database connections.
*/
class DatabaseHandler
{
	var $connection; /**< an connection to the database */

	/**
	 * The constructor of the DatabaseHandler. Establishs the ADOdb connection.
	 */
	function DatabaseHandler()
	{
	}

	/**
	 * Connects to a specific data source.
	 * @param server The database server name.
	 * @param database The database name.
	 * @param username Username to authenticate to the database.
	 * @param password Password to authenticate to the database.
	 * @return The connection to the database.
	 */
	function connectTo($server, $database, $username, $password)
	{
		if(($this->connection = mysql_connect($server, $username, $password))==FALSE) return false;
		if((mysql_select_db($database, $this->connection))==FALSE) return false;
	}

	/**
	 * Executes an SQL query on a database.
	 * @param sql_query The SQL query to be executed.
	 * @return Returns the Result set of the query.
	 */
	function executeQuery($sql_query)
	{
		return $this->connection->mysql_query($sql_query);
	}

	function returnError() {
		return $this->connection->mysql_error();
	}

	function returnNumRows($rs) {
		return $this->connection->mysql_num_rows($rs);
	}

	/**
	 * Closes the database connection.
	 * @return The result value.
	 */
	function close()
	{
		return mysql_close($this->connection);
	}
}
?>