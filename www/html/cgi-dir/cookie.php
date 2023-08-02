<?php
ob_start();

$cookies = [
    [
        "key" => "user1",
        "value" => "Zakaria Mahmoud",
    ],
    [
        "key" => "user2",
        "value" => "Anas Douib",
    ],
    [
        "key" => "user3",
        "value" => "Hafid Selbi",
    ]
];


foreach ($cookies as $cookie) {
    if (isset($_COOKIE[$cookie["key"]])) {
        $key = $cookie["key"];
        $value = $cookie["value"];
        echo "================== $key ==================<br>";
        echo "Cookie is set! [key = $key, value = $value]<br>";
        echo "=============================================<br>";
    } else {
        echo "Cookie '" . $cookie["key"] . "' is not set!<br>";
        setcookie($cookie["key"], $cookie["value"], time() + (86400 * 30), "/"); // 86400 = 1 day
    }
}

ob_end_flush();
?>