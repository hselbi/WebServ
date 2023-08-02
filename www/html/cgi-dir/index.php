<?php
// // Process the input data
// var_dump($_GET);

// echo "<br>=================================<br>";

// phpinfo();

// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "Hello World";
// echo "\0";
// echo "es";

session_start();

if (!isset($_SESSION["count"])) {
    echo "count = 0 <br>";
    $_SESSION["count"] = 0;
}

if (isset($_SESSION['username'])) {
    echo "Hello " . $_SESSION['username'] . "<br>";
    $_SESSION["count"]++;
} else {
    echo "Hello guest <br>";
    $_SESSION['username'] = "zmahmoud";
}


if ($_SESSION["count"] > 4) {
    session_destroy();
    echo "Session destroyed <br>";
}
else
    echo "count = " . $_SESSION["count"] . "<br>";


echo "<br>=================================<br>";
echo "<br>=================================<br>";
echo "<br>=================================<br>";

echo "<pre>";
    var_dump($_SERVER);
echo "</pre>";
?>
