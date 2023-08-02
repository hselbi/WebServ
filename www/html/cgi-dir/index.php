<!DOCTYPE html>
<html>
<body>

<?php
$cookie_name = "user";
$cookie_value = "John Doe";
if(isset($_COOKIE[$cookie_name])) {
    echo "Cookie named '" . $cookie_name . "' is already  set!";
} else {
	setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/cgi"); // 86400 = 1 day
    echo "Cookie '" . $cookie_name . "' is set!<br>";
    // echo "Value is: " . $_COOKIE[$cookie_name];
}
?>

</body>
</html>
