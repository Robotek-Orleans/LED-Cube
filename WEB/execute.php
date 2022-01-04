<?php
session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();


    http_response_code(200);

    exec('sudo pkill -SIGINT execLEDCube');

    if(isset($_GET['f']) && isset($_SESSION['username'])){
       exec('sudo /var/www/html/LED-Cube/cpp/execLEDCube '.$_GET['f']. " > /dev/null &");
       echo "OK";
    }else{
        http_response_code(400);
        die('bad request');
    }
    

?>
