<?php

// var_dump($_POST);

$inputData = file_get_contents('php://stdin');

// Process the input data
echo "Received input: " . $inputData;

?>
