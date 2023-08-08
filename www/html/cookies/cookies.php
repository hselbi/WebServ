<?php
$coockie_path = "/";
if (isset($_POST['submit_create_cookie'])) {
    setcookie($_POST['cookie_name'], $_POST['cookie_value'], time() + (86400 * 30), $coockie_path);
    echo "<h2>Cookie created successfully</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
else if (isset($_POST["submit_destroy_cookie_by_name"]))
{
    if (isset($_COOKIE[$_POST['cookie_name']]))
    {
        setcookie($_POST['cookie_name'], "", time() - (86400 * 30), $coockie_path);
        echo "<h2>Cookie destroyed successfully</h2>";
    }
    else
        echo "<h2>Cookie not found</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";

}
else if (isset($_POST["submit_destroy_all_cookies"]))
{
    foreach ($_COOKIE as $key => $value) {
        setcookie($key, "", time() - (86400 * 30), $coockie_path);
    }
    echo "<h2>All cookies destroyed successfully</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
else
{
    echo "<h2 style='color:red'>Invalid Request</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
?>