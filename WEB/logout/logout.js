import { LedCubeWS } from '../common/js/ws/LedCubeWS.js';

// loggoff the user to stop other connections
var ledcube = new LedCubeWS();
ledcube.addEventListener(ledcube.EVENTS.LOGGED, event => ledcube.loggoff());
ledcube.addEventListener(ledcube.EVENTS.DISCONNECTED, event => setTimeout(loggedOut, 200));
if (!ledcube.tryCookieConnect()) {
	setTimeout(loggedOut, 500);
}
else {
	setTimeout(loggedOut, 10000);
}

function loggedOut() {
	// remove session cookie and logout from ledcube
	localStorage.removeItem('ck');
	localStorage.removeItem('cc');
	localStorage.removeItem('sid');

	// redirect to main page
	window.location.href = webFolder;
}

export default {
	ledcube,
};