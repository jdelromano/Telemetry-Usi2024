<?php
/*
 * Configuration of the user-feedback database backend.
 * Copy or rename this file to localconfig.php and adjust the below settings
 * according to your database setup.
 */

/* Database driver: sqlite, mysql or pgsql */
$USERFEEDBACK_DB_DRIVER=getenv('MYSQL_DRIVER');
$USERFEEDBACK_DB_HOST=getenv('MYSQL_HOST');
$USERFEEDBACK_DB_PORT=getenv('MYSQL_PORT');
$USERFEEDBACK_DB_NAME=getenv('MYSQL_DATABASE');
$USERFEEDBACK_DB_USERNAME=getenv('MYSQL_USER');
$USERFEEDBACK_DB_PASSWORD=getenv('MYSQL_PASSWORD');
?>
