<?php
session_start();
if (isset($_POST['submit_create_session'])) {

    $_SESSION[$_POST['session_name']] = $_POST['session_value'];
    echo "<h2>Session created successfully</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
else if (isset($_POST["submit_destroy_session_by_name"]))
{

    if (isset($_SESSION[$_POST['session_name']]))
    {
        unset($_SESSION[$_POST['session_name']]);
        echo "<h2>Session destroyed successfully</h2>";
    }
    else
        echo "<h2>Session not found</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";

}
else if (isset($_POST["submit_destroy_all_sessions"]))
{
    session_destroy();
    echo "<h2>All sessions destroyed successfully</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
else
{
    echo "<h2>Invalid request</h2>";
    echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
}
?>