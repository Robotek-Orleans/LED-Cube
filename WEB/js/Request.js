function send( StringData ) {

    var xhr = new XMLHttpRequest();
    var url = "save.php";
    xhr.open("POST", url, true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var json = JSON.parse(xhr.responseText);
            console.log(json.email + ", " + json.password);
        }
    };
    var data = JSON.stringify({"email": "hey@mail.com", "password": "101010"});
    xhr.send(data);
    
    

}




function scriptstat(classes, message, temps){
	var image = "done";
	if(classes.indexOf("rouge") >= 0){
	 image = "error";
	}
	if(classes.indexOf("orange") >= 0){
	 image = "warning";
	}
    var ligne = document.getElementById('servstats').insertRow(0);
    ligne.innerHTML = '<tr><td class="' + image + '"><div onclick="statremove(this.parentNode.parentNode)" class="message ' + classes + '"><p class="contentmessage">' + message + '</p></div></td>';
    document.getElementById('servstats').style.height = document.getElementById('servstats').offsetHeight + ligne.firstChild.firstChild.firstChild.offsetHeight +1;
    if(temps != 0){
      setTimeout(function(){statremove(ligne);}, temps);
    }
    return ligne;
};


function statremove(element){
    document.getElementById('servstats').style.height = document.getElementById('servstats').offsetHeight - element.firstChild.firstChild.firstChild.offsetHeight -1;
    element.parentNode.removeChild(element)
  };