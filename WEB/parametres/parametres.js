import { ledCubeTools } from '../common/js/main.js';

var askInterval;
var latestStatus = { sys: {}, ledcube: {} };
function askStatus() {
	document.getElementById("connected").setAttribute("value", ledCubeTools.ledcubeWS.isConnected());

	if (!ledCubeTools.ledcubeWS.isConnected() && !ledCubeTools.ledcubeWS.isConnecting()) {
		ledCubeTools.ledcubeWS.tryCookieConnect();
	}
	if (ledCubeTools.ledcubeWS.isConnected()) {
		askSysStatus();
	}
}

var lastUpdate = { sys: 0, ledcube: 0 };
function askSysStatus() {
	if (lastUpdate.sys && Date.now() - lastUpdate.sys < 2000) return;
	lastUpdate.sys = Date.now();
	ledCubeTools.ledcubeWS.send('status', {});
}
async function askLEDCubeStatus() {
	if (lastUpdate.ledcube && Date.now() - lastUpdate.ledcube < 2000) return;
	lastUpdate.ledcube = Date.now();
	const answer = await ledCubeTools.ledcubeWS.sendLedcube.status.get_ledcube_status();
	if (answer) {
		fillLEDCubeStatus(answer);
	}
}

document.querySelector("#raspberrypi_status > h1").addEventListener('click', () => askSysStatus());
document.querySelector("#ledcube_status > h1").addEventListener('click', () => askLEDCubeStatus());

function round(number, precision) {
	const factor = Math.pow(10, precision);
	return Math.round(number * factor) / factor;
}

/**
 * @param {{
 * 	temp: number,
 * 	cpuUsage: number,
 * 	ramUsage: number,
 * 	bootTime: number
 * }} status
 */
function fillRaspberryPiStatus(status) {
	status ??= {};

	document.getElementById("connected").setAttribute("value", ledCubeTools.ledcubeWS.isConnected());

	/** @type {HTMLOutputElement} */
	const temperature = document.getElementById("temperature");
	if (status.temp >= 0) { // Kelvin
		const temp = round(status.temp - 273.15, 1);
		temperature.value = temp;
		temperature.setAttribute('color', temp > 50 ? 'red' : temp > 40 ? 'darkorange' : 'green');
	}
	else {
		temperature.value = 'N/A';
		temperature.removeAttribute('color');
	}

	/** @type {HTMLTimeElement} */
	const boot_time = document.getElementById("boot_time");
	applyDateTime(boot_time, status.bootTime);

	/** @type {HTMLOutputElement} */
	const cpuUsage = document.getElementById("cpuUsage");
	if (status.cpuUsage >= 0) {
		cpuUsage.value = round(status.cpuUsage, 2);
		cpuUsage.style.width = status.cpuUsage + '%';
	}
	else {
		cpuUsage.value = 'N/A';
		cpuUsage.style.width = '100%';
	}

	/** @type {HTMLOutputElement} */
	const ramUsage = document.getElementById("ramUsage");
	if (status.ramUsage >= 0) {
		ramUsage.value = round(status.ramUsage, 2);
		ramUsage.style.width = status.ramUsage + '%';
	}
	else {
		ramUsage.value = 'N/A';
		ramUsage.style.width = '100%';
	}
}

/**
 * @param {{
 * 	currentName: string,
 * 	currentStartedAt: number,
 * 	currentDuration: number,
 * 	nextRandomMinTime: number,
 * 	nextName: string,
 * 	nextDuration: number,
 * }} status
 */
function fillLEDCubeStatus(status) {
	status ??= {};
	latestStatus.ledcube = status;

	/** @type {HTMLOutputElement} */
	const ledCubePlaying = document.getElementById("ledcubePlaying");
	ledCubePlaying.setAttribute('value', !!status.currentName || status.currentName == "");

	/** @type {HTMLOutputElement} */
	const currentAnimation = document.getElementById("currentAnimation");
	currentAnimation.value = status.currentName ?? 'N/A';

	/** @type {HTMLOutputElement} */
	const nextAnimation = document.getElementById("nextAnimation");
	nextAnimation.value = status.nextName ?? 'N/A';

	/** @type {HTMLTimeElement} */
	const animationWorkingTime = document.getElementById("animationWorkingTime");
	applyDateTime(animationWorkingTime, status.currentStartedAt);
}

/**
 * @param {number} date
 */
function formatDateTime(date, withDate = false) {
	if (!date) return 'N/A';
	const d = new Date(date);
	return (withDate ? d.toLocaleDateString() + ' ' : '') + d.toLocaleTimeString();
}

/**
 * @param {HTMLTimeElement} element
 * @param {number} date
 */
function applyDateTime(element, date) {
	element.innerText = formatDateTime(date);
	element.title = element.dateTime = formatDateTime(date, true);
}

// Update when connected
ledCubeTools.addEventListener(ledCubeTools.EVENTS.LOGGED, () => {
	ledCubeTools.ledcubeWS.syncViewer(ledCubeTools.cubeViewer)?.then(answer => {
		if (answer) fillLEDCubeStatus(answer);
	});
	askStatus();
	if (document.visibilityState == "visible") {
		clearInterval(askInterval);
		askInterval = setInterval(askStatus, 10000);
	}
});

// Update when disconnected
ledCubeTools.addEventListener(ledCubeTools.EVENTS.DISCONNECTED, () => askStatus());

// Stop the update interval when the page is not visible
document.addEventListener("visibilitychange", () => {
	if (document.hidden) {
		clearInterval(askInterval);
	}
	else {
		askStatus();
		clearInterval(askInterval);
		askInterval = setInterval(askStatus, 10000);
	}
});

/**
 * @param {object} data
 */
ledCubeTools.ledcubeWS.addEventListener(ledCubeTools.ledcubeWS.EVENTS.MESSAGE, data => {
	switch (data.type) {
		case 'sysStatus':
			latestStatus.sys = data;
			fillRaspberryPiStatus(data);
			break;
		case 'ledcubeEvents': {
			switch (data.eventName) {
				case 'animationChanged':
					fillLEDCubeStatus(data);
					break;
			}
			break;
		}
	}
});

askStatus(); // Update at refresh


export default {
	ledCubeTools,
	get latestStatus() { return latestStatus; },
	get sysStatus() { return latestStatus.sys; },
	get ledcubeStatus() { return latestStatus.ledcube; }
};
