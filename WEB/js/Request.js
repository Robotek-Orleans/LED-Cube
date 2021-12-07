function sendAnimation( ) {

    /* file_name , data , time , stat (=save) */

    let xhr = new XMLHttpRequest();
    let url = "save.php";
    xhr.open("POST", url, true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            console.log("ok :" + xhr.responseText);
        }else{

        }
    };
    let data = {};
    data["data"] = framecontent;
    data["fileName"] = "test.txt";
    data["time"] = 100;
    data["stat"] = "save";

    xhr.send(JSON.stringify(data));

}


function scriptstat(classes, message, temps){
	var image = "done";
	if(classes.indexOf("rouge") >= 0){
	 image = "error";
	}
	if(classes.indexOf("orange") >= 0){
	 image = "warning";
	}
    var ligne = document.getElementById('contentnotifs').insertRow(0);
    ligne.innerHTML = '<tr><td class="' + image + '"><div onclick="statremove(this.parentNode.parentNode)" class="message ' + classes + '"><p class="contentmessage">' + message + '</p></div></td>';
    document.getElementById('contentnotifs').style.height = document.getElementById('contentnotifs').offsetHeight + ligne.firstChild.firstChild.firstChild.offsetHeight +1;
    if(temps != 0){
      setTimeout(function(){statremove(ligne);}, temps);
    }
    return ligne;
};


function statremove(element){
    document.getElementById('contentnotifs').style.height = document.getElementById('contentnotifs').offsetHeight - element.firstChild.firstChild.firstChild.offsetHeight -1;
    element.parentNode.removeChild(element)
  };