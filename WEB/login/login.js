import { ledcubeWS } from '../common/js/ws/main.js';

var input = document.querySelector('.pswrd');
var show = document.querySelector('.show');
show.addEventListener('click', showPassword);
function showPassword() {
	if (input.type === "password") {
		input.type = "text";
		show.style.color = "#1DA1F2";
		show.textContent = "HIDE";
	} else {
		input.type = "password";
		show.textContent = "SHOW";
		show.style.color = "#111";
	}
}

document.querySelector('#username').addEventListener('keydown', inputKeyDown);
document.querySelector('.pswrd')?.addEventListener('keydown', inputKeyDown);

ledcubeWS.addEventListener(ledcubeWS.EVENTS.LOGGED, event => onLogged(event));
ledcubeWS.addEventListener(ledcubeWS.EVENTS.LOGIN_REJECTED, event => {
	console.log('login rejected', event);
	switch (event.reason) {
		case 'wrong_password':
			onWrongPassword();
			break;
		case 'cancel':
		default:
			onAuthCancel();
			break;
	}
});

function inputKeyDown(e) {
	if (e.keyCode === 13) {
		e.preventDefault();
		document.querySelector('#login').click();
	}
}

function loginLedCube() {
	const clientChallenge = dcodeIO.bcrypt.genSaltSync();
	const username = document.querySelector('#username').value;
	const password = document.querySelector('#pswrd').value;
	const cookieKey = dcodeIO.bcrypt.hashSync(password, clientChallenge);
	localStorage.setItem('ck', cookieKey);
	localStorage.setItem('cc', clientChallenge);
	localStorage.setItem('sid', '');
	console.log('connecting...', { clientChallenge, cookieKey });
	ledcubeWS.connect();
}

function onWrongPassword() {
	// shake the password input
	const field = document.querySelector('#field-password');
	field.setAttribute('shake', 'true');
	setTimeout(() => {
		field.removeAttribute('shake');
	}, 1000);
}

function onAuthCancel() {
	// shake the button
	const button = document.querySelector('#button-login');
	button.setAttribute('shake', 'true');
	setTimeout(() => {
		button.removeAttribute('shake');
	}, 1000);
}

function onLogged() {
	const form = document.querySelector('#loginForm');
	form.style.display = 'none';
	const loggedAnimation = document.querySelector('#loggedAnimation');
	loggedAnimation.style.display = '';

	// redirect
	setTimeout(() => {
		localStorage.setItem('comeFromLogin', '1');
		window.history.back();
		setTimeout(() => {
			window.location.href = webFolder;
		}, 1000);
	}, 500);
}

export default {
	ledcubeWS,
	showPassword,
	inputKeyDown,
	loginLedCube,
	onWrongPassword,
	onAuthCancel,
	onLogged,
};