<?php
if (isset($_POST['submit_create'])) {
    $filename = $_POST['filename'];
    $type = $_POST['type'];
    if (!isset($filename) || trim($filename) == '') {
        echo "<h1>Error: Filename is empty.</h1>";
        echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
        return;
    }
    if ($type == "type_file") {
        $file = fopen("./data/$filename", "w");
        if (!$file) {
            echo "<h1>Error: Unable to create file.</h1>";
            echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
            return;
        }
        fwrite($file, "This is a file.");
        fclose($file);
        echo "<h1>File created.</h1>";
    }
    else if ($type == "type_dir") {
        if (!mkdir("./data/$filename")) {
            echo "<h1>Error: Unable to create directory.</h1>";
            echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
            return;
        }
        echo "<h1>Directory created.</h1>";

    }
    else {
        echo "<h1>Error: Invalid type.</h1>";
    }
}
else
{
    echo "<h1>Error: Invalid request.</h1>";
}

echo "<a href='".$_SERVER['HTTP_REFERER']."'><button>Go Back</button></a>";
