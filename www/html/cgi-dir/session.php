<?php
if (isset($_COOKIE['session_id'])) {
    $sessionId = $_COOKIE['session_id'];

    // Path to the file where you want to store the session_id
    $filePath = '/path/to/your/file.txt';

    // Open the file in write mode
    $file = fopen($filePath, 'w');

    if ($file) {
        // Write the session_id to the file
        fwrite($file, $sessionId);
        fclose($file);
        echo 'Session ID stored in the file successfully.';
    } else {
        echo 'Unable to open the file for writing.';
    }
} else {
    echo 'No session_id cookie found.';
}
?>
