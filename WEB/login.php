<?php
session_start();

include('functions/function.php');
redirect_user_if_log_in();
if (isset($_POST['sub'])) {
  
  $user = "ledcube";
  $pass = "ledcube2021";
	
	$errors = [];
	extract($_POST);
	if (empty($username)) {
		array_push($errors,"Champs utilisateur est vide");
	}
  if (empty($password)) {
		array_push($errors,"Champs mot de passe est vide");
	}else if (($user != $username) || ($pass != $password)) {
        array_push($errors,"Mot de passe incorrect");
  }else if(count($errors) == 0){
			$_SESSION['username'] = $username;
			header("Location: index.php");
	}
}   
?>

<!DOCTYPE html>
<html lang="en" dir="ltr">
  <head>
    <meta charset="utf-8">
    <title>LED CUBE</title>
    <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
    <link rel="stylesheet" href="css/style.css">
  </head>
  <body>
    <div class="container">
      <header>LED CUBE</header>

      <?php foreach ($errors as $err) { ?>
        <h5 style="color:red"><?php echo $err ?></h5>
     <?php  }  ?>
      <form method="POST" >
        <div class="input-field">
          <input type="text" name="username" >
          <label>Nom D'utilisateur</label>
        </div>
        <div class="input-field">
          <input class="pswrd" type="password" name="password" >
          <span class="show">SHOW</span>
          <label>Mot de passe</label>
        </div>
        <div class="button">
          <div class="inner"></div>
          <button type="submit" name="sub">LOGIN</button>
        </div>
      </form>
     
    <script>
      var input = document.querySelector('.pswrd');
      var show = document.querySelector('.show');
      show.addEventListener('click', active);
      function active(){
        if(input.type === "password"){
          input.type = "text";
          show.style.color = "#1DA1F2";
          show.textContent = "HIDE";
        }else{
          input.type = "password";
          show.textContent = "SHOW";
          show.style.color = "#111";
        }
      }
    </script>

  </body>
</html>
