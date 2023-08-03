<?php
// // Process the input data
// phpinfo();
// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "\0";
// echo "es";

session_start();
var_dump($_SESSION);
$username = $_SESSION['username'];
echo "|$username|";

?>
