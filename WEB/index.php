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
        div {
            top: 50%;
            position: fixed;
            height: auto;
            align-content: center;
            align-items: center;
            align-self: center;
            display: flex;
            flex-flow: wrap;
        }

        button {
            display: flex;
            flex-grow: 2;
            align-content: center;
            align-items: center;
        }
    </style>
    <script>
        function edit(){
            window.location.href = "edit.php"
        }

        function fromImage(){
            window.location.href = "from_image.php"
        }

        function openfile(){
            window.location.href = "openfile.php"
        }
    </script>
</head>


<body>

    <div class="header" style="align-items: baseline;">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>

    <div>
        <button onclick="edit()" class=" purple">Créer une animation</button>
        <button onclick="fromImage()" class=" green">Créer une animation à partir d'une image</button>
        <button onclick="openfile()" class=" red">Ouvrir une animation existante</button>
    </div>


    <div id="modal" class="contentviewer modal-content">
        <button onclick="closeModal()" class="close-modal">X</button>
        <div style="top: 0px;height: 100%;" id="container"></div>
    </div>
    <table id="contentnotifs"></table>
</body>


</html>