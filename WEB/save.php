<?php

session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();

$path = "./animations/";

$data = json_decode(file_get_contents('php://input'), true);
//print_r($data);


http_response_code(200);

if(isset($data['stat']) && isset($data['fileName'])){
    if(isset($data['fileName']) && isset($data['data']) && isset($data['time']) && $data['stat'] == 'save'){

        $file = fopen($path.$data['fileName'], "w") or die("Unable to open file!");

        fwrite($file, strval(count($data['data'])).PHP_EOL);
        fwrite($file, $data['time'].PHP_EOL);

        for($i=0;$i<count($data['data']);$i++){
            for($j=0;$j<count($data['data'][$i]);$j++){
                for($k=0;$k<count($data['data'][$i][$j]);$k++){
                    for($l=0;$l<count($data['data'][$i][$j][$k]);$l++){
                        $red = strval(hexdec(substr($data['data'][$i][7-$l][$k][7-$j],1,2)));
                        $green = strval(hexdec(substr($data['data'][$i][7-$l][$k][7-$j],3,2)));
                        $blue = strval(hexdec(substr($data['data'][$i][7-$l][$k][7-$j],5,2)));
                        fwrite($file, $red.PHP_EOL); // rouge
                        fwrite($file, $green.PHP_EOL); // vert
                        fwrite($file, $blue.PHP_EOL); // bleu
                    }
                }
            }   
        }
        //fwrite($file, $txt);


        fclose($file);

        //print_r($data['data']);

    }else{
        http_response_code(400);

        die('bad request');
    }
    
}

?> 
