<?php

    http_response_code(200);

    exec('sudo pkill -SIGINT execLEDCube');

    if(isset($_GET['f'])){
       exec('sudo /var/www/html/LED-Cube/cpp/execLEDCube '.$_GET['f']. " > /dev/null &");
       echo "OK";
    }else{
        http_response_code(400);
        die('bad request');
    }
    

?>
