function sendAnimation(isPlayAnimation){
    /* file_name , data , time , stat (=save) */
    if (save) {
        scriptstat("orange", "Enregistrement en cours", 2000);

        let xhr = new XMLHttpRequest();
        let url = "save.php";
        xhr.open("POST", url, true);
        xhr.setRequestHeader("Content-Type", "application/json");
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4 && xhr.status === 200) {
                scriptstat("vert", "Enregistré avec succès", 2000);
                save = false;
                isSavable();
                if(isPlayAnimation){
                    playAnimation();
                }
            } else if (xhr.readyState === 4) {
                scriptstat("rouge", "Erreur lors de l'enregistrement: " + xhr.responseText, 0);
            }
        };
        let data = {};
        data["data"] = framecontent;
        data["fileName"] = document.getElementById("fileName").value;
        data["time"] = document.getElementById("frameTime").value;
        data["stat"] = "save";

        xhr.send(JSON.stringify(data));
    } else {
        scriptstat("orange", "Votre fichier a déjà été enregistré", 2000);
        if(isPlayAnimation){
            playAnimation();
        }
    }
}

function playAnimation() {
    scriptstat("orange", "Demande en cours", 2000);

    let xhr = new XMLHttpRequest();
    let url = "execute.php?f=" + document.getElementById("fileName").value;
    xhr.open("GET", url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            scriptstat("vert", "Animation envoyé avec succès", 2000);
        } else if (xhr.readyState === 4) {
            scriptstat("rouge", "Erreur lors de la requète: " + xhr.responseText, 0);
        }
    };
    xhr.send();
}

function playAnimationParam(animation) {
    scriptstat("orange", "Demande en cours", 2000);

    let xhr = new XMLHttpRequest();
    let url = "execute.php?f=" + animation;
    xhr.open("GET", url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            scriptstat("vert", "Animation envoyé avec succès", 2000);
        } else if (xhr.readyState === 4) {
            scriptstat("rouge", "Erreur lors de la requète: " + xhr.responseText, 0);
        }
    };
    xhr.send();
}


function scriptstat(classes, message, temps) {
    var image = "done";
    if (classes.indexOf("rouge") >= 0) {
        image = "error";
    }
    if (classes.indexOf("orange") >= 0) {
        image = "warning";
    }
    var ligne = document.getElementById('contentnotifs').insertRow(0);
    ligne.innerHTML = '<tr><td class="' + image + '"><div onclick="statremove(this.parentNode.parentNode)" class="message ' + classes + '"><p class="contentmessage">' + message + '</p></div></td>';
    document.getElementById('contentnotifs').style.height = document.getElementById('contentnotifs').offsetHeight + ligne.firstChild.firstChild.firstChild.offsetHeight + 1;
    if (temps != 0) {
        setTimeout(function () { statremove(ligne); }, temps);
    }
    return ligne;
};


function statremove(element) {
    document.getElementById('contentnotifs').style.height = document.getElementById('contentnotifs').offsetHeight - element.firstChild.firstChild.firstChild.offsetHeight - 1;
    element.parentNode.removeChild(element)
};


