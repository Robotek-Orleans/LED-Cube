
export function addNotif(classes, message, temps) {
	var image = "done";
	if (classes.indexOf("rouge") >= 0) {
		image = "error";
	}
	if (classes.indexOf("orange") >= 0) {
		image = "warning";
	}
	/** @type {HTMLTableElement} */
	const contentnotifs = document.getElementById('contentnotifs');

	let ligne = contentnotifs.insertRow(0);

	var td = document.createElement('td');
	td.classList.add(image);
	ligne.appendChild(td);

	let bulle = document.createElement('div');
	bulle.className = 'message ' + classes;
	bulle.onclick = () => ligne.remove();
	td.appendChild(bulle);

	let p = document.createElement('p');
	p.classList.add('contentmessage');
	p.innerText = message;
	bulle.appendChild(p);


	if (temps != 0) {
		setTimeout(() => ligne.remove(), temps);
	}
	return ligne;
};
