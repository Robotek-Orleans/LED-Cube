/**
 * @author qiao / https://github.com/qiao
 * @author mrdoob / http://mrdoob.com
 * @author alteredq / http://alteredqualia.com/
 * @author WestLangley / http://github.com/WestLangley
 * @author erich666 / http://erichaines.com
 */
/*global THREE, console */

// This set of controls performs orbiting, dollying (zooming), and panning. It maintains
// the "up" direction as +Y, unlike the TrackballControls. Touch on tablet and phones is
// supported.
//
//    Orbit - left mouse / touch: one finger move
//    Zoom - middle mouse, or mousewheel / touch: two finger spread or squish
//
// This is a drop-in replacement for (most) TrackballControls used in examples.
// That is, include this js file and wherever you see:
//    	controls = new THREE.TrackballControls( camera );
//      controls.target.z = 150;
// Simple substitute "OrbitControls" and the control should work as-is.

THREE.OrbitControls = function (object, domElement) {

	this.object = object;
	this.domElement = (domElement !== undefined) ? domElement : document;

	// "target" sets the location of focus, where the control orbits around
	// and where it pans with respect to.
	this.target = new THREE.Vector3();

	// center is old, deprecated; use "target" instead
	this.center = this.target;

	// This option actually enables dollying in and out; left as "zoom" for
	// backwards compatibility
	this.zoomSpeed = 1.0;

	// Limits to how far you can dolly in and out
	this.minDistance = 0;
	this.maxDistance = Infinity;

	// Set to true to disable this control
	this.rotateSpeed = 1.0;

	// How far you can orbit vertically, upper and lower limits.
	// Range is 0 to Math.PI radians.
	this.minPolarAngle = 0; // radians
	this.maxPolarAngle = Math.PI; // radians

	// How far you can orbit horizontally, upper and lower limits.
	// If set, must be a sub-interval of the interval [ - Math.PI, Math.PI ].
	this.minAzimuthAngle = - Infinity; // radians
	this.maxAzimuthAngle = Infinity; // radians

	// The four arrow keys
	this.keys = { LEFT: 37, UP: 38, RIGHT: 39, BOTTOM: 40 };

	// Mouse buttons
	this.mouseButtons = { ORBIT: THREE.MOUSE.LEFT, ZOOM: THREE.MOUSE.MIDDLE };

	////////////
	// internals

	let scope = this;

	let EPS = 0.000001;

	const rotateStart = new THREE.Vector2();
	const rotateEnd = new THREE.Vector2();
	let rotateDelta = new THREE.Vector2();

	let offset = new THREE.Vector3();

	const dollyStart = new THREE.Vector2();
	const dollyEnd = new THREE.Vector2();
	let dollyDelta = new THREE.Vector2();

	let theta;
	let phi;
	let phiDelta = 0;
	let thetaDelta = 0;
	let scale = 1;

	let lastPosition = new THREE.Vector3();
	let lastQuaternion = new THREE.Quaternion();

	let STATE = { NONE: -1, ROTATE: 0, DOLLY: 1, PAN: 2, TOUCH_ROTATE: 3, TOUCH_DOLLY: 4, TOUCH_PAN: 5 };

	let state = STATE.NONE;

	// for reset

	this.target0 = this.target.clone();
	this.position0 = this.object.position.clone();

	// so camera.up is the orbit axis

	let quat = new THREE.Quaternion().setFromUnitVectors(object.up, new THREE.Vector3(0, 1, 0));
	let quatInverse = quat.clone().inverse();

	// events

	let changeEvent = { type: 'change' };
	let startEvent = { type: 'start' };
	let endEvent = { type: 'end' };

	this.rotateLeft = function (angle) {

		if (angle === undefined) {

			angle = getAutoRotationAngle();

		}

		thetaDelta -= angle;

	};

	this.rotateUp = function (angle) {

		if (angle === undefined) {

			angle = getAutoRotationAngle();

		}

		phiDelta -= angle;

	};

	this.dollyIn = function (dollyScale) {

		if (dollyScale === undefined) {

			dollyScale = getZoomScale();

		}

		scale /= dollyScale;

	};

	this.dollyOut = function (dollyScale) {

		if (dollyScale === undefined) {

			dollyScale = getZoomScale();

		}

		scale *= dollyScale;

	};

	this.update = function () {

		let position = this.object.position;

		offset.copy(position).sub(this.target);

		// rotate offset to "y-axis-is-up" space
		offset.applyQuaternion(quat);

		// angle from z-axis around y-axis

		theta = Math.atan2(offset.x, offset.z);

		// angle from y-axis

		phi = Math.atan2(Math.sqrt(offset.x * offset.x + offset.z * offset.z), offset.y);

		theta += thetaDelta;
		phi += phiDelta;

		// restrict phi to be between desired limits
		phi = Math.max(this.minPolarAngle, Math.min(this.maxPolarAngle, phi));

		// restrict phi to be betwee EPS and PI-EPS
		phi = Math.max(EPS, Math.min(Math.PI - EPS, phi));

		let radius = offset.length() * scale;

		// restrict radius to be between desired limits
		radius = Math.max(this.minDistance, Math.min(this.maxDistance, radius));

		offset.x = radius * Math.sin(phi) * Math.sin(theta);
		offset.y = radius * Math.cos(phi);
		offset.z = radius * Math.sin(phi) * Math.cos(theta);

		// rotate offset back to "camera-up-vector-is-up" space
		offset.applyQuaternion(quatInverse);

		position.copy(this.target).add(offset);

		this.object.lookAt(this.target);

		thetaDelta = 0;
		phiDelta = 0;
		scale = 1;

	};


	this.reset = function () {

		state = STATE.NONE;

		this.target.copy(this.target0);
		this.object.position.copy(this.position0);

		this.update();

	};

	this.getPolarAngle = function () {

		return phi;

	};

	this.getAzimuthalAngle = function () {

		return theta

	};

	function getAutoRotationAngle() {

		return 2 * Math.PI / 60 / 60 * scope.autoRotateSpeed;

	}

	function getZoomScale() {

		return Math.pow(0.95, scope.zoomSpeed);

	}

	function onMouseDown(event) {
		paused = false;
		animate();
		event.preventDefault();

		if (event.button === scope.mouseButtons.ORBIT) {

			state = STATE.ROTATE;

			rotateStart.set(event.clientX, event.clientY);

		} else if (event.button === scope.mouseButtons.ZOOM) {

			state = STATE.DOLLY;

			dollyStart.set(event.clientX, event.clientY);

		}
		if (state !== STATE.NONE) {
			document.addEventListener('mousemove', onMouseMove, false);
			document.addEventListener('mouseup', onMouseUp, false);
			scope.dispatchEvent(startEvent);
		}

	}

	function onMouseMove(event) {

		event.preventDefault();

		let element = scope.domElement === document ? scope.domElement.body : scope.domElement;

		switch (state) {
			case STATE.ROTATE:

				rotateEnd.set(event.clientX, event.clientY);
				rotateDelta.subVectors(rotateEnd, rotateStart);

				// rotating across whole screen goes 360 degrees around
				scope.rotateLeft(2 * Math.PI * rotateDelta.x / element.clientWidth * scope.rotateSpeed);

				// rotating up and down along whole screen attempts to go 360, but limited to 180
				scope.rotateUp(2 * Math.PI * rotateDelta.y / element.clientHeight * scope.rotateSpeed);

				rotateStart.copy(rotateEnd);
				break;
			case STATE.DOLLY:

				dollyEnd.set(event.clientX, event.clientY);
				dollyDelta.subVectors(dollyEnd, dollyStart);

				if (dollyDelta.y > 0) {

					scope.dollyIn();

				} else {

					scope.dollyOut();

				}

				dollyStart.copy(dollyEnd);
				break;
			default:
				state = STATE.NONE;
				break;
		}


		if (state !== STATE.NONE) scope.update();

	}

	function onMouseUp( /* event */) {
		document.removeEventListener('mousemove', onMouseMove, false);
		document.removeEventListener('mouseup', onMouseUp, false);
		scope.dispatchEvent(endEvent);
		state = STATE.NONE;
		paused = true;

	}

	function onMouseWheel(event) {
		event.preventDefault();
		event.stopPropagation();

		let delta = 0;

		if (event.wheelDelta !== undefined) { // WebKit / Opera / Explorer 9

			delta = event.wheelDelta;

		} else if (event.detail !== undefined) { // Firefox

			delta = - event.detail;

		}

		if (delta > 0) {

			scope.dollyOut();

		} else {

			scope.dollyIn();

		}

		scope.update();
		scope.dispatchEvent(startEvent);
		scope.dispatchEvent(endEvent);
		paused = true;

		renderer.render(scene, camera);
	}

	function touchstart(event) {

		paused = false;
		animate();

		switch (event.touches.length) {

			case 1:	// one-fingered touch: rotate

				state = STATE.TOUCH_ROTATE;

				rotateStart.set(event.touches[0].pageX, event.touches[0].pageY);
				break;

			case 2:	// two-fingered touch: dolly

				state = STATE.TOUCH_DOLLY;

				let dx = event.touches[0].pageX - event.touches[1].pageX;
				let dy = event.touches[0].pageY - event.touches[1].pageY;
				let distance = Math.sqrt(dx * dx + dy * dy);
				dollyStart.set(0, distance);
				break;

			default:

				state = STATE.NONE;

		}

		if (state !== STATE.NONE) scope.dispatchEvent(startEvent);

	}

	function touchmove(event) {

		event.preventDefault();
		event.stopPropagation();

		let element = scope.domElement === document ? scope.domElement.body : scope.domElement;

		switch (event.touches.length) {

			case 1: // one-fingered touch: rotate

				if (state !== STATE.TOUCH_ROTATE) return;

				rotateEnd.set(event.touches[0].pageX, event.touches[0].pageY);
				rotateDelta.subVectors(rotateEnd, rotateStart);

				// rotating across whole screen goes 360 degrees around
				scope.rotateLeft(2 * Math.PI * rotateDelta.x / element.clientWidth * scope.rotateSpeed);
				// rotating up and down along whole screen attempts to go 360, but limited to 180
				scope.rotateUp(2 * Math.PI * rotateDelta.y / element.clientHeight * scope.rotateSpeed);

				rotateStart.copy(rotateEnd);

				scope.update();
				break;

			case 2: // two-fingered touch: dolly

				if (state !== STATE.TOUCH_DOLLY) return;

				let dx = event.touches[0].pageX - event.touches[1].pageX;
				let dy = event.touches[0].pageY - event.touches[1].pageY;
				let distance = Math.sqrt(dx * dx + dy * dy);

				dollyEnd.set(0, distance);
				dollyDelta.subVectors(dollyEnd, dollyStart);

				if (dollyDelta.y > 0) {

					scope.dollyOut();

				} else {

					scope.dollyIn();

				}

				dollyStart.copy(dollyEnd);

				scope.update();
				break;

			default:

				state = STATE.NONE;

		}

	}

	function touchend( /* event */) {

		paused = true;
		scope.dispatchEvent(endEvent);
		state = STATE.NONE;

	}

	this.domElement.addEventListener('mousedown', onMouseDown, false);


	this.domElement.addEventListener('mousewheel', onMouseWheel, false)
	this.domElement.addEventListener('DOMMouseScroll', onMouseWheel, false); // firefox


	this.domElement.addEventListener('touchstart', touchstart, false);
	this.domElement.addEventListener('touchend', touchend, false);
	this.domElement.addEventListener('touchmove', touchmove, false);

	// force an update at start
	this.update();

};

THREE.OrbitControls.prototype = Object.create(THREE.EventDispatcher.prototype);
THREE.OrbitControls.prototype.constructor = THREE.OrbitControls;
