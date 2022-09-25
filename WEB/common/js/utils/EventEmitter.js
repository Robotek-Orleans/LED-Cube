

export class EventEmitter {

	#events = {};

	/**
	 * @param {string} eventName
	 * @param {boolean} singleShot If the event is triggered only once
	 */
	registerEvent(eventName, singleShot = false) {
		this.#events[eventName] = {
			singleShot,
			triggered: false,
			callbacks: []
		};
	}

	/**
	 * @param {string} eventName
	 */
	emitEvent(eventName, ...args) {
		const event = this.#events[eventName];
		if (event) {
			if (event.triggered && event.singleShot) throw new Error(`Event ${eventName} already triggered`);
			event.triggered = true;
			setTimeout(() => event.callbacks.forEach(callback => callback(...args)));
		}
	}

	/**
	 * @param {string} eventName
	 * @param {function} callback
	 */
	addEventListener(eventName, callback) {
		const event = this.#events[eventName];
		if (event) {
			if (event.triggered && event.singleShot) callback();
			else event.callbacks.push(callback);
		}
	}
}