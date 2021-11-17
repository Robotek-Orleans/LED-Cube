<?php

$path = "./animations";

if(isset($_POST['stat']) && isset($_POST['fileName'])){
    if(isset($_POST['file_name']) && isset($_POST['data']) && isset($_POST['time']) && $_POST['stat'] == 'save'){

        $file = fopen($path."lidn.txt", "w") or die("Unable to open file!");

        fwrite($file, count($_POST['data'])."\n");
        fwrite($file, $_POST['time']."\n");

        for($i=0;$i<count($_POST['data']);$i++){
            for($j=0;$j<count($_POST['data'][$i]);$j++){
                for($k=0;$k<count($_POST['data'][$i][$j]);$k++){

                }
            }   
        }
        fwrite($file, $txt);


        fclose($file);

        die('OK');
    }
    
}

?> 
