import { LEDAnimation } from '../utils/frame.js';
import { EventEmitter } from '../utils/EventEmitter.js';
import { addNotif } from '../utils/notifications.js';
import { CubeViewer } from '../viewer/cubeViewer.js';

/**
 * This is the main class to connect to the server and send/receive data.
 * You can send/receive data in the sendLedcube section,
 * or use sendLedCubeWithNotification to send data and display a notification.
 * 
 * bcrypt is required to connect to the server
 * pako and base64 are required to encode/decode the animations
 */

export class LedCubeWS extends EventEmitter {
	/** @type {WebSocket} */
	ws = null;
	is_connected = false;
	logging = false;
	/** @type {object} */
	last_message = null;

	constructor() {
		super();
		Object.values(this.EVENTS).forEach(event => this.registerEvent(event));
		if (document.location.pathname.endsWith('/login/')) {
			var comeFromLogin = localStorage.getItem('comeFromLogin');
			if (comeFromLogin) {
				comeFromLogin = parseInt(comeFromLogin) || 1;
				localStorage.setItem('comeFromLogin', comeFromLogin + 1);
				if (window.history.length <= 0 || comeFromLogin >= 3) { // no back() or in a loop
					document.location.href = webFolder;
				}
				else {
					window.history.back();
				}
			}
		}
		else {
			localStorage.removeItem('comeFromLogin');
		}
	}

	isConnected() {
		return this.is_connected;
	}

	isConnecting() {
		return this.ws && (this.ws.readyState == this.ws.CONNECTING || (this.ws.readyState == this.ws.OPEN && !this.is_connected));
	}

	connect() {
		if (this.ws && (this.ws.readyState == this.ws.OPEN)) {
			if (this.is_connected)
				console.warn('Already connected');
			else
				console.warn('Already connecting');
			return;
		}
		this.ws = new WebSocket('ws://' + location.host + '/ws');
		this.ws.onopen = () => console.log('open, waiting for server challenge to login');
		this.ws.onmessage = event => this.#onMessage.call(this, event);
		this.ws.onclose = () => {
			console.log(`WebSocket Connection ended`);
			if (this.is_connected) {
				this.#whenLoggedOut();
				this.emitEvent(this.EVENTS.DISCONNECTED);
			}
			this.ws = null;
		}
	}

	tryCookieConnect() {
		var cookieKey = localStorage.getItem('ck');
		var clientChallenge = localStorage.getItem('cc');

		if (!cookieKey || !clientChallenge) {
			return this.isConnected() || this.isConnecting();
		}
		this.connect();
		return true;
	}

	loggoff() {
		this.send('connection', { status: 'loggoff' });
		localStorage.clear();
		this.logging = false;
		this.ws.close();
	}

	EVENTS = {
		LOGGED: 'LEDCUBE_logged',
		DISCONNECTED: 'LEDCUBE_disconnected',
		MESSAGE: 'LEDCUBE_message',
		LOGIN_REJECTED: 'LEDCUBE_login_rejected',
	}

	/**
	 * @param {string} type
	 * @param {object} data
	 */
	send(type, data) {
		if (!this.ws || this.ws.readyState != this.ws.OPEN) return;
		if (!this.is_connected && type != 'auth') return;
		this.ws.send(JSON.stringify({ ...data, type, date: Date.now().toString() }));
	}

	/**
	 * @param {string} serverChallenge
	 */
	sendPassword(serverChallenge) {
		this.logging = true;
		var cookieKey = localStorage.getItem('ck');
		var clientChallenge = localStorage.getItem('cc');
		var sessionId = localStorage.getItem('sid');

		if (!cookieKey || !clientChallenge) {
			document.location.href = '/login';
			return;
		}

		const key = dcodeIO.bcrypt.hashSync(cookieKey + serverChallenge);
		this.send('auth', {
			key,
			challenge: clientChallenge,
			id: sessionId
		});
	}

	/**
	 * @type {{time: number, timestamp: string, type: string, resolve: Function, reject: Function}[]}
	 */
	#waitingForAnwsers = [];
	#lastWaitingTime = 0;
	#waitingTimeId = 0;
	/**
	 * @param {string} type
	 * @param {object} data
	 */
	async sendAndWaitAnswer(type, data) {
		if (this.isConnecting()) await new Promise(resolve => {
			setTimeout(resolve, 3000);
			setInterval(() => {
				if (!this.isConnecting()) resolve();
			}, 100);
		});
		if (!this.ws || this.ws.readyState != this.ws.OPEN) return { success: false, error: 'Not connected' };
		if (!this.is_connected && type != 'auth') return { success: false, error: 'Not logged' };
		const now = Date.now();
		if (this.#lastWaitingTime < now) {
			this.#lastWaitingTime = now;
			this.#waitingTimeId = 0;
		}
		const id = this.#waitingTimeId++;
		const timestamp = now.toString() + '.' + id.toString();
		/**
		 * @type {Promise<{type: string, timestamp: string, success: boolean}>}
		 */
		const promise = new Promise((resolve, reject) => {
			this.#waitingForAnwsers.push({
				time: now,
				timestamp,
				type,
				resolve,
				reject
			});
			data.timestamp = timestamp;
			this.send(type, data);
			setTimeout(() => {
				const reqest = this.#waitingForAnwsers.find(r => r.timestamp == timestamp);
				if (reqest) {
					this.#waitingForAnwsers.splice(this.#waitingForAnwsers.indexOf(reqest), 1);
					reqest.reject('timeout');
				}
			}, 10000); // Timeout 10 seconds
		});
		return promise;
	}

	/**
	 * @param {string} string Normal string
	 * @returns {string} String zipped and encoded in base64
	 */
	strToZip(string) {
		var dec = new TextEncoder().encode(string);
		var zip = pako.deflate(dec, { level: 9 });
		var base64 = Array.from(zip).map(c => String.fromCharCode(c)).join('');
		return '0' + Base64.encode(base64);
	}
	/**
	 * @param {string} base64 String encoded in base64
	 * @returns {string} String unzipped
	 */
	zipToStr(base64) {
		var zip = Base64.decode(base64.substring(1)).split('').map(c => c.charCodeAt(0));
		var enc = pako.inflate(zip, { level: 9 });
		var string = new TextDecoder('utf-8').decode(enc);
		return string;
	}
	/**
	 * @param {object} json JSON object
	 * @returns {string} String encoded in base64
	 */
	jsonZip(json) { return this.strToZip(JSON.stringify(json)); }
	/**
	 * @param {string} base64 String encoded in base64
	 * @returns {object} JSON object
	 */
	jsonUnzip(zipbase64) { return JSON.parse(this.zipToStr(zipbase64)); }

	sendLedcube = {
		/** @param {{x: number, y: number, z: number, color: string}} data @returns {Promise<{success:boolean}>} */
		setLed: ({ x, y, z, color }) => this.sendAndWaitAnswer('ledcube', { action: 'setLed', x, y, z, color }),
		animation: {
			/** @param {string} fileName @returns {Promise<{success:boolean}>} */
			play: (fileName) => this.sendAndWaitAnswer('ledcube', { action: 'animation.play', fileName }),

			/** @param {string} filter @returns {Promise<{success:boolean}>} */
			playRandom: (filter) => this.sendAndWaitAnswer('ledcube', { action: 'animation.playRandom', filter }),

			/** @param {{fileName: string, animation: LEDAnimation, play: boolean, overwrite: boolean}} @returns {Promise<{success:boolean}>} */
			save: ({ fileName, animation, play = undefined, overwrite = undefined }) => this.sendAndWaitAnswer('ledcube', { action: 'animation.save', fileName, animation: this.encodeAnimation(animation), play, overwrite }),

			/** @param {LEDAnimation} animation @returns {Promise<{success:boolean}>} */
			play_no_save: (animation) => this.sendAndWaitAnswer('ledcube', { action: 'animation.play-no-save', animation: this.encodeAnimation(animation) }),

			/** @param {string} fileName @returns {Promise<{animation: LEDAnimation}>} */
			get: async (fileName) => {
				const response = await this.sendAndWaitAnswer('ledcube', { action: 'animation.get', fileName });
				response.animation = this.decodeAnimation(response.animation);
				return response;
			},

			/** @param {string} fileName @returns {Promise<{animation: LEDAnimation}>} */
			get_current: async () => {
				const response = await this.sendAndWaitAnswer('ledcube', { action: 'animation.get-current' });
				response.animation = this.decodeAnimation(response.animation);
				return response;
			},

			list: async () => {
				/** @type {{success: boolean, animations: string[], timestamp: string}} */
				const answer = await this.sendAndWaitAnswer('ledcube', { action: 'animation.list' });
				answer.animations?.sort((a, b) => a.localeCompare(b));
				return answer;
			},

			/** @returns {Promise<{success:boolean}>} */
			stop: () => this.sendAndWaitAnswer('ledcube', { action: 'animation.stop' }),

			/** @param {string} fileName @returns {Promise<{success:boolean}>} */
			delete: (fileName) => this.sendAndWaitAnswer('ledcube', { action: 'animation.delete', fileName }),
		},
		layer: {
			/** @param {string} direction @param {number} layerIndex @returns {Promise<{success:boolean, layer:object}>} */
			get: (direction, layerIndex) => this.sendAndWaitAnswer('ledcube', { action: 'layer.get', direction, layerIndex }),

			/** @param {string} direction @param {number} layerIndex @param {object} layer @returns {Promise<{success:boolean}>} */
			set: (direction, layerIndex, layer) => this.sendAndWaitAnswer('ledcube', { action: 'layer.set', direction, layerIndex, layer: this.jsonZip(layer) }),
		},
		/** @param {{count:number, fps:number}} @returns {Promise<{success:boolean}>} */
		set_animation_frame_count: ({ count, fps }) => this.sendAndWaitAnswer('ledcube', { action: 'set-animation-frames-count', count, fps }),

		status: {
			/**
			 * @returns {Promise<{
			 * 	temp: number,
			 * 	uptime: number,
			 *  cpu: number,
			 *  ram: number}>}
			 */
			get_raspberry_status: () => this.sendAndWaitAnswer('status', {}),
			get_ledcube_status: () => this.sendAndWaitAnswer('ledcube', { action: 'status' }),
			subscribe_changes: (subscribe) => this.sendAndWaitAnswer('ledcube', { action: 'status.subscribe', subscribe }),
		}
	}

	/** @param {Promise<>} promise @param {string} messageSuccess */
	async #sendLedCubeWithBasicNotification(promise, messageSuccess) {
		this.notifications.warning('Demande en cours');
		const answer = await promise;
		if (answer.success) this.notifications.success(messageSuccess);
		else this.notifications.error('Erreur lors de la demande\n' + answer.error);
		return answer;
	}

	sendLedCubeWithNotification = {
		animation: {
			/** @param {string} fileName @returns {Promise<{success:boolean}>} */
			play: (fileName) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.play(fileName), 'Animation lancée'),

			/** @param {string} filter @returns {Promise<{success:boolean}>} */
			playRandom: (filter) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.playRandom(filter), 'Animation lancée'),

			/** @param {{fileName: string, animation: LEDAnimation, play: boolean}} @returns {Promise<{success:boolean}>} */
			save: ({ fileName, animation, play = undefined, overwrite = undefined }) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.save({ fileName, animation, play, overwrite }), 'Animation sauvegardée'),

			/** @param {LEDAnimation} animation @returns {Promise<{success:boolean}>} */
			play_no_save: (animation) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.play_no_save(animation), 'Animation lancée'),

			/** @param {string} fileName @returns {Promise<{animation: LEDAnimation}>} */
			get: (fileName) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.get(fileName), 'Animation récupérée'),

			/** @param {string} fileName @returns {Promise<{animation: LEDAnimation}>} */
			get_current: () => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.get_current(), 'Animation récupérée'),

			/** @returns {Promise<{animations: string[]}>} */
			list: () => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.list(), 'Liste récupérée'),

			/** @returns {Promise<{success:boolean}>} */
			stop: () => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.stop(), 'Animation arrêtée'),

			/** @param {string} fileName @returns {Promise<{success:boolean}>} */
			delete: (fileName) => this.#sendLedCubeWithBasicNotification(this.sendLedcube.animation.delete(fileName), 'Animation supprimée'),
		}
	}

	notifications = {
		warning: (message) => addNotif('orange', message, 2000),
		success: (message) => addNotif('vert', message, 2000),
		error: (message) => addNotif('rouge', message, 0),
	}

	/**
	 * @param {LEDAnimation} animation
	 */
	encodeAnimation(animation) {
		const json = {
			version: 2,
			contentType: 'animation',
			frameCount: animation.frames.length,
			frameDuration: Number.parseInt(animation.frameDuration),
			name: animation.name,
			frames: animation.toDecimalArrays()
		};
		return '2 ' + this.jsonZip(json);
	}

	/** @param {string} animationData */
	decodeAnimation(animationData) {
		var match;
		if (animationData == "" || !animationData) {
			return new LEDAnimation();
		}
		else if (match = animationData.match(/^2\s+([a-zA-Z0-9\+/=]+)$/)) {
			// v2
			const base64 = match[1];
			const jsonAnim = this.jsonUnzip(base64);
			var LCAnim = LEDAnimation.fromDecimalArrays(jsonAnim.frames);
			LCAnim.frameDuration = jsonAnim.frameDuration || 50;
			LCAnim.name = jsonAnim.name;
			LCAnim.version = jsonAnim.version ?? 2;
			return LCAnim;
		}
		else {
			// v1
			var lines = animationData.split('\n');
			if (lines.length == 1) {
				console.warn('decodeAnimation detected v1 but must have 2 lines at least');
				return new LEDAnimation();
			}
			const frameCount = parseInt(lines[0]);
			const frameDuration = parseInt(lines[1]);
			const frames = [];
			const LED_PER_FRAME = 8 * 8 * 8 * 3; // 192 * 8 = 1536
			const LED_PER_LAYER = 8 * 8 * 3; // 192
			const LED_PER_LINE = 8 * 3; // 24
			// one color is 3 lines (r, g, b)
			for (let i = 0; i < frameCount; i++) {
				const frame = [];
				for (let z = 0; z < 8; z++) {
					const layer = [];
					for (let y = 0; y < 8; y++) {
						const line = [];
						for (let x = 0; x < 8; x++) {
							const lineIndex = 2 + i * LED_PER_FRAME + z * LED_PER_LAYER + y * LED_PER_LINE + x * 3;
							const r = parseInt(lines[lineIndex]);
							const g = parseInt(lines[lineIndex + 1]);
							const b = parseInt(lines[lineIndex + 2]);
							const color = r << 16 | g << 8 | b;
							line.push(color);
						}
						layer.push(line);
					}
					frame.push(layer);
				}
				frames.push(frame);
			}
			var LCAnim = LEDAnimation.fromDecimalArrays(frames);
			LCAnim.frameDuration = frameDuration;
			LCAnim.name = 'unknown';
			LCAnim.version = 1;
			return LCAnim;
		}
	}

	#onMessage(event) {
		this.last_message = event;

		var data = JSON.parse(event.data);
		if (data.type !== 'sysStatus' && data.type !== 'answer' && data.type !== 'ledcubeEvents') {
			console.log('LedCubeWS onMessage:', data);
		}
		switch (data.type) {
			case 'clientCount':
			case 'sysStatus':
				break;
			case 'connection':
				/** @type {string} */
				var status = data.status;
				if (status.startsWith('login.success')) {
					this.#whenLoggedIn(data.id);
					this.emitEvent(this.EVENTS.LOGGED);
				}
				else if (status.startsWith('login.failed')) {
					this.#whenLoggedOut();
					const reason = status.substring('login.failed.'.length) || 'unknown';
					this.emitEvent(this.EVENTS.LOGIN_REJECTED, { reason });
				}
				else {
					console.warn('Unknown connection message:', data);
				}
				break;
			case 'auth':
				if (data.content == 'challenge') {
					this.sendPassword(data.challenge);
				}
				else {
					console.warn('Unknown auth response:', data);
				}
				break;
			case 'answer': {
				const request = this.#waitingForAnwsers.find(r => r.timestamp == data.timestamp);
				if (request) {
					this.#waitingForAnwsers.splice(this.#waitingForAnwsers.indexOf(request), 1);
					request.resolve(data);
				}
				break;
			}
			case 'ledcubeEvents':
				if (this.syncData.synced) {
					switch (data.eventName) {
						case 'animationChanged':
							this.syncData.ledcubeInfo = data;
							this.updateSyncViewer();
							break;
						case 'layerChanged':
							this.syncData.cubeViewer.animation.currentFrame.setPlan(data.direction, data.layerIndex, data.lines);
							this.syncData.cubeViewer.refresh();
							break;
						case 'ledChanged':
							this.syncData.cubeViewer.animation.currentFrame.setColor(data.x, data.y, data.z, data.color);
							this.syncData.cubeViewer.refresh();
							break;
						default:
							console.warn('Unknown ledcube event', data);
							break;
					}
				}
				break;
			default:
				console.warn('Unknown message type: ' + data.type);
		}
		this.emitEvent(this.EVENTS.MESSAGE, data);
	}

	#whenLoggedIn(clientId) {
		this.is_connected = true;
		this.clientId = clientId;
		localStorage.setItem('sid', this.clientId);
		document.body.setAttribute('connected', 'true');
	}

	#whenLoggedOut() {
		this.is_connected = false;
		document.body.setAttribute('connected', 'false');
	}

	/**
	 * Store the animation in the local storage
	 * @param {LEDAnimation} animation
	 */
	storeAnimationLocal(animation) {
		const animationData = this.encodeAnimation(animation);
		const key = 'animation_' + Date.now();
		var added = false;
		do {
			try {
				localStorage.setItem(key, animationData);
				added = true;
			}
			catch (e) {
				// Remove 5 oldest animations
				const keys = Object.keys(localStorage).filter(k => k.startsWith('animation_'));
				keys.sort();
				if (keys.length == 0) {
					throw e; // There is not enough space to store the animation
				}
				for (let i = 0; i < 5 && i < keys.length; i++) {
					localStorage.removeItem(keys[i]);
				}
			}
		} while (!added);
		return key;
	}

	/**
	 * Open the editor with the given animation key
	 * @param {string} key
	 * @param {{submit_blank: boolean}} options
	 */
	editAnimationLocal(key, options) {
		const url = webFolder + 'animation/edit/?local_key=' + key;
		if (options?.submit_blank) {
			window.open(url, '_blank'); // Open in new tab
		}
		else {
			document.location.href = url;
		}
	}

	/**
	 * A local animation stored in the local storage to save it between editor pages
	 * @param {string} key
	 */
	getAnimationLocal(key) {
		const animationData = localStorage.getItem(key);
		if (animationData) {
			return this.decodeAnimation(animationData);
		}
		return null;
	}

	/**
	 * A cached version of the server's animations to avoid requesting them multiple times
	 * @type {LEDAnimation[]}
	 */
	cachedAnimations = [];
	/**
	 * @param {string} animationName
	 */
	async getCachedAnimation(animationName) {
		var animation = this.cachedAnimations.find(a => a.name == animationName);
		if (animation) return animation;

		animation = (await this.sendLedcube.animation.get(animationName))?.animation;
		if (animation) {
			this.cachedAnimations.push(animation);
		}

		if (!animation) animation = (await this.sendLedcube.animation.get_current())?.animation;

		return animation;
	}

	syncData = {
		synced: false,
		ledcubeInfo: {
			currentName: "",
			currentStartedAt: 0, // timestamp
			currentFrameIndex: 0,
			nextName: 0,
		},
		/** @type {CubeViewer} */
		cubeViewer: null,
		options: {
			syncAnimation: true,
		}
	}
	/**
	 * Sync the viewer with the server
	 * @param {CubeViewer} cubeViewer
	 * @param {{syncAnimation: boolean}} options
	 */
	async syncViewer(cubeViewer, options = {}) {
		const currentInfo = await this.sendLedcube.status.subscribe_changes(true);
		this.syncData.cubeViewer = cubeViewer;
		this.syncData.options.syncAnimation = options.syncAnimation !== false;
		if (currentInfo) {
			this.syncData.synced = true;
			this.syncData.ledcubeInfo = currentInfo;
			this.updateSyncViewer();
		}
		return currentInfo;
	}

	async updateSyncViewer() {
		if (!this.syncData.options.syncAnimation) return;
		const animation = await this.getCachedAnimation(this.syncData.ledcubeInfo.currentName);
		if (animation) {
			const framesElapsed = Math.floor((Date.now() - this.syncData.ledcubeInfo.currentStartedAt) / animation.frameDuration);
			const frameIndex = this.syncData.ledcubeInfo.currentFrameIndex + framesElapsed;

			this.syncData.cubeViewer.animation = animation;
			this.syncData.cubeViewer.startAnim({ frameIndex });
		}
	}

	/**
	 * End the sync between the viewer and the server
	 */
	endSyncViewer() {
		this.syncData.synced = false;
		this.sendLedcube.status.subscribe_changes(false);
	}

}

function removeOldLocalAnimations() {
	const anim_keys = Object.keys(localStorage).filter(key => key.startsWith('animation_'));
	/** @param {string} key */
	function isOld(key) {
		const timestamp = key.substring('animation_'.length);
		const date = parseInt(timestamp);
		const now = Date.now();
		return now - date > 1000 * 60 * 60 * 24; // 1 day
	}
	const keysToRemove = anim_keys.filter(isOld);
	keysToRemove.forEach(key => localStorage.removeItem(key));
	if (keysToRemove.length > 0) {
		console.log('Removed old local animations:', keysToRemove);
	}
}
removeOldLocalAnimations();
