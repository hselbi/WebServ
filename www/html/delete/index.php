<?php

echo "<h1 style='color: #286da1;'>List all files in data directory</h1><hr>";

$files = glob('./data/*');

if (count($files) == 0) {
    echo "<p>No files found.</p>";
}
else
{
    echo "<ul>";
    foreach($files as $file) {
        if (is_dir($file))
            echo "<li>$file<b>/</b></li>";
        else
            echo "<li>$file</li>";
    }
    echo "</ul>";
}
echo "<hr>";
?>


<h2 style="color: #286da1;">Create a file or directory in data directory</h2>
<hr>
<form action="create_file.php" method="post" >
    <table style="width: 500px;">
        <tr>
            <td><label for="filename">Filename:</label></td>
            <td><input type="text" name="filename" placeholder="Enter file name" style="width:100%;height: 30px;"></td>
        </tr>
        <tr>
            <td><label for="session_value">Type:</label></td>
            <td>
                <select name="type" id="type" style="width:100%;height: 30px;">
                    <option value="type_file">File</option>
                    <option value="type_dir">Directory</option>
                </select>
            </td>
        </tr>
        <tr>
            <td></td>
            <td><button type="submit" value="submit" name="submit_create"  style="width:100%">Create</button></td>
        </tr>
    </table>
</form>

<hr>

<h2 style="color: #286da1;">Delete a file or directory in data directory (using axios to send delete request)</h2>
<hr>

<?php


if (count($files) == 0) {
    echo "<p>No files found.</p>";
}
else
{
    echo "<select id='delete_file' style='width: 500px;height: 30px;'>";
    foreach($files as $file) {
        if (is_dir($file))
            echo "<option value='$file/'><b>$file/</b></option>";
        else
            echo "<option value='$file'>$file</option>";
    }
    echo "</select>";
    echo "<button onclick='deleteFile()' style='margin-left: 10px; width: 100px;height: 30px;'>Delete</button>";
}
?>

<script src="https://cdnjs.cloudflare.com/ajax/libs/axios/1.4.0/axios.min.js"></script>

<script>
    function deleteFile() {
        var file = document.getElementById("delete_file").value;
        axios.delete(file)
            .then(function (response) {
                console.log(response);
                alert("File deleted.");
                location.reload();
            })
            .catch(function (error) {
                console.log(error);
                alert("Error: Unable to delete file.");
            });
    }
</script>