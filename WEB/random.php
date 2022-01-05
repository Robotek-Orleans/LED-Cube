<?php
session_start();

include('functions/function.php');

redirect_user_if_is_not_log_in();

    http_response_code(200);
    
    exec('sudo kill -9 `pgrep -f aleatoire.sh`');
    
    if(isset($_SESSION['username'])){
       exec('sudo /var/www/html/LED-Cube/cpp/aleatoire.sh > /dev/null &');
       echo "OK";
    }else{
        http_response_code(400);
        die('Disconnected');
    }
    
?>
