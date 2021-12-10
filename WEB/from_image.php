<!DOCTYPE html>
<html lang="fr">

<head>
	<meta charset='utf-8'>
	<meta http-equiv='X-UA-Compatible' content='IE=edge'>
	<title>Image To Pattern</title>
	<meta name='viewport' content='width=device-width, initial-scale=1'>
	<link rel='stylesheet' type='text/css' media='screen' href='css/from_image.css'>
	<script src='js/from_image.js'></script>
</head>

<body>

	<div>
		<label for="image_input">Choisir une image</label>
		<input type="file" name="image_input" id="image_input" accept="image/bmp">
	</div>

	<div><img id="display_image" alt="Aucune Image Chargée" /></div>

	<button id="send_matrice" disabled onClick="javascript: sendMatrice()">Envoyer au ledcube</button>

</body>

</html>