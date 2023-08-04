<?php session_start(); ?>

<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Generate Session</title>
</head>
<body>
        <h1 style="color: #286da1;">Sessions in PHP</h1>
        <ul>
            
            <?php
               
                if (!isset($_SESSION) || count($_SESSION) == 0) {
                    echo "<li>No session available</li>";
                } else {
                    foreach ($_SESSION as $key => $value) {
                        echo "<li>" . $key . " => " . $value . "</li>";
                    }
                }
                ?>
        </ul>
        <hr>
        <h3>Create a session</h3>

        <form action="sessions.php" method="post">

                <table>
                    <tr>
                        <td><label for="session_name">Session Name</label></td>
                        <td><input type="text" name="session_name" ></td>
                    </tr>
                    <tr>
                        <td><label for="session_value">Session Value</label></td>
                        <td><input type="text" name="session_value" ></td>
                    </tr>
                    <tr>
                        <td colspan="2"><button type="submit" value="submit" name="submit_create_session"  style="width:100%">Create Session</button></td>
                    </tr>
                </table>
        </form>

        <hr>
        <h3>Destroy a session by name or destroy all</h3>

        <form action="sessions.php" method="post">

                <table>
                    <tr>
                        <td><label for="session_name">Session Name</label></td>
                        <td><input type="text" name="session_name" ></td>
                    </tr>
                    <tr>
                        <td ><button type="submit" value="submit" name="submit_destroy_session_by_name"  style="width:100%">Destroy </button></td>
                        <td ><button type="submit" value="submit" name="submit_destroy_all_sessions"  style="width:100%">Destroy All </button></td>
                    </tr>
                </table>
        </form>
        
</body>
</html>