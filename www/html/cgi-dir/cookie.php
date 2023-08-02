<?php
ob_start();
$cookie_name = "user";
$cookie_value = "John Doe";
setcookie("1", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
setcookie("2", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
setcookie("3", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day


ob_end_flush();
?>