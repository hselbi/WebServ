<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GET method</title>
</head>
<body>
    
    <h1 style="color: #286da1;">GET Method</h1>

    <?php
        if (isset($_GET) && count($_GET) > 0) {
            echo "<ul>";
            foreach ($_GET as $key => $value) {
                echo "<li>" . $key . " => " . $value . "</li>";
            }
            echo "</ul>";
        } else {
            echo "<h4>No data received</h4>";
            echo "<div>example: <a href='?project=WebServ&user1=hselbi&user2=adouib&user3=zmahmooud'>?project=WebServ&user1=hselbi&user2=adouib&user3=zmahmooud</a></div>";
        }
    ?>
    <hr>
    <h2>Display image by url path </h2>
    

    <form action="<?php $_SERVER['PHP_SELF'] ?>" method="get">
            <table >
                <tr>
                    <td><label for="image_url">Image URL</label></td>
                    <td><input type="text" name="image_url" style="width:400px"></td>
                    <td colspan="2"><button type="submit" value="submit" name="submit" >Get Image</button></td>

                </tr>
            </table>
    </form> 

    <?php
        if (isset($_GET['image_url']) && $_GET['image_url'] != "") {
            echo "<img src='" . $_GET['image_url'] . "' alt='image' style='width:100%'>";
        }
        else 
            echo "<h4>No image url received, enter a valid url.</h4>";
        ?>

</body>
</html>