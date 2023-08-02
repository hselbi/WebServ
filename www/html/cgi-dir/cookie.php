<?php
ob_start();

$cookie_value = "1337";
setcookie("1", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
setcookie("2", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
setcookie("3", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day


ob_end_flush();
?>