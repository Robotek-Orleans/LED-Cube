<?php
session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();
?>

<!DOCTYPE html>
<html lang="fr">

<head>
    <title>LED cube IHM</title>
    <meta http-equiv="content-type" content="text/html;charset=utf-8" />
    <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
    <link rel="stylesheet" href="css/Main.css">
    <style>
        div.buttons {
            display: flex;
            height:max-content;
            align-content:center;
            align-items: center;
            align-self: center;
            vertical-align: middle;
            flex-flow: wrap;
            max-width: 500px;
            margin-top: 50px;
        }

        button {
            display: flex;
            align-items: center;
            justify-content: center;
            flex-grow: 2;
            align-content: center;
            align-items: center;
            width: 100%;
        }
    </style>
</head>


<body>

    <div class="header" style="align-items: baseline;">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>

    <div class = buttons>
        <button onclick="window.location.href = 'openfile.php'" class=" red">Ouvrir une animation existante</button>
        <button onclick="window.location.href = 'edit.php'" class=" purple">Créer une animation</button>
        <button onclick="window.location.href = 'from_image.php'" class=" green">Créer une animation à partir d'une image</button>
    </div>
</body>


</html>