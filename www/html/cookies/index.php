<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Generate Cookie</title>
</head>
<body>
        <h1 style="color: #286da1;">Cookies in PHP</h1>
        <ul>
            
            <?php
               
                if (!isset($_COOKIE) || count($_COOKIE) == 0) {
                    echo "<li>No session available</li>";
                } else {
                    foreach ($_COOKIE as $key => $value) {
                        echo "<li>" . $key . " => " . $value . "</li>";
                    }
                }
                ?>
        </ul>
        <hr>
        <h3>Create a cookies</h3>

        <form action="cookies.php" method="POST">

                <table>
                    <tr>
                        <td><label for="cookie_name">Cookie Name</label></td>
                        <td><input type="text" name="cookie_name" ></td>
                    </tr>
                    <tr>
                        <td><label for="cookie_value">Cookie Value</label></td>
                        <td><input type="text" name="cookie_value" ></td>
                    </tr>
                    <tr>
                        <td colspan="2"><button type="submit" value="submit" name="submit_create_cookie"  style="width:100%">Create Cookie</button></td>
                    </tr>
                </table>
        </form>

        <hr>
        <h3>Destroy a cookie by name or destroy all</h3>

        <form action="cookies.php" method="post">

                <table>
                    <tr>
                        <td><label for="cookie_name">Cookie Name</label></td>
                        <td><input type="text" name="cookie_name" ></td>
                    </tr>
                    <tr>
                        <td ><button type="submit" value="submit" name="submit_destroy_cookie_by_name"  style="width:100%">Destroy </button></td>
                        <td ><button type="submit" value="submit" name="submit_destroy_all_cookies"  style="width:100%">Destroy All </button></td>
                    </tr>
                </table>
        </form>
        
</body>
</html>