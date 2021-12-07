<?php

$path = "./animations/";

$data = json_decode(file_get_contents('php://input'), true);
//print_r($data);


if(isset($data['stat']) && isset($data['fileName'])){
    if(isset($data['fileName']) && isset($data['data']) && isset($data['time']) && $data['stat'] == 'save'){

        $file = fopen($path.$data['fileName'], "w") or die("Unable to open file!");

        fwrite($file, count(count($data['data']))."\n");
        fwrite($file, $data['time']."\n");

        for($i=0;$i<count($data['data']);$i++){
            for($j=0;$j<count($data['data'][$i]);$j++){
                for($k=0;$k<count($data['data'][$i][$j]);$k++){
                    for($l=0;$l<count($data['data'][$i][$j][$k]);$l++){
                        fwrite($file, substr($data['data'][$i][$j][$k][$l],1,2)."\n"); // rouge
                        fwrite($file, substr($data['data'][$i][$j][$k][$l],3,2)."\n"); // vert
                        fwrite($file, substr($data['data'][$i][$j][$k][$l],5,2)."\n"); // bleu
                    }
                }
            }   
        }
        //fwrite($file, $txt);


        fclose($file);

        print_r($data['data']);

        http_response_code(200);

        die('OK');
    }else{
        http_response_code(400);

        die('bad request');
    }
    
}

?> 
