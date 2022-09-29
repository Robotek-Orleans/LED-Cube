import { LedCubeWS } from './LedCubeWS.js';

/** @type {LedCubeWS} */
export var ledcubeWS = new LedCubeWS();
var tryingCookieConnect = ledcubeWS.tryCookieConnect();

document.addEventListener('DOMContentLoaded', () => {
	if (!tryingCookieConnect) {
		// If the cookie connect is not trying to connect, we are disconnected
		document.body.setAttribute('connected', 'false');
		// Else this attribute is set when the cookie connect is successful or unsuccessful
	}

	document.querySelector('#login').addEventListener('click', (event) => {
		// When clicking on the "login" button, we want to connect to the server
		// First, we check if the user can be logged in with a cookie
		tryingCookieConnect = ledcubeWS.tryCookieConnect();
		if (!tryingCookieConnect) {
			// If not, we ask the user to enter his credentials
			document.location.href = webFolder + 'login';
		} else {
			event.preventDefault();
			function onLogged() {
				// If the user is logged in, everything is fine (no need to redirect)
				removeEvents();
			}
			function onRejected() {
				// If the user is rejected, we redirect him to the login page
				removeEvents();
				document.location.href = webFolder + 'login';
			}
			function removeEvents() {
				document.removeEventListener(ledcubeWS.EVENTS.LOGGED, onLogged);
				document.removeEventListener(ledcubeWS.EVENTS.LOGIN_REJECTED, onRejected);
			}
			ledcubeWS.addEventListener(ledcubeWS.EVENTS.LOGGED, onLogged);
			ledcubeWS.addEventListener(ledcubeWS.EVENTS.LOGIN_REJECTED, onRejected);
		}
	});
});

export var requests = {
	playAnimationAleat: ledcubeWS.sendLedCubeWithNotification.animation.playRandom,
	stopAnimation: ledcubeWS.sendLedCubeWithNotification.animation.stop,
};