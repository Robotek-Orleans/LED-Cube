import uniqueFramePainter, { events, Vector3D, Pos3D, getVectorFromKey, paintLed } from './../../common/js/utils/uniqueFramePainter.js';
import { XLENGTH, YLENGTH, ZLENGTH } from '../../common/js/utils/frame.js';

export var direction = new Vector3D(1, 0, 0);
/** @type {Pos3D[]} */
export var snake = [new Pos3D(0, 0, 0)];
for (let i = 0; i < 4; i++) {
	snake.push(snake[snake.length - 1].clone().add(direction));
}

/** @type {Pos3D} */
export var food;

export function getSnakeColor() {
	return uniqueFramePainter.ledCubeTools.colorPicker.getDecimal();
}

export function getRandomPos() {
	return new Pos3D(Math.floor(Math.random() * XLENGTH), Math.floor(Math.random() * YLENGTH), Math.floor(Math.random() * ZLENGTH));
}

events.onStart = () => {
	uniqueFramePainter.ledCubeTools.colorPicker.setColor(0x00FF00);
	uniqueFramePainter.ledCubeTools.colorPicker.addEventListener('pickup', () => refreshSnake());
	refreshSnake();
	setInterval(move, 200);
	console.log("Snake started");
};

events.onReady = () => {
	spawnFood();
	refreshSnake();
	console.log("Snake ready", document.timeline.currentTime);
};

export function refreshSnake() {
	const color = getSnakeColor();
	for (let i = 0; i < snake.length - 1; i++) {
		paintLed(snake[i], color, false);
	}
	if (food) paintLed(food, 0xFF0000, false);
	uniqueFramePainter.ledCubeTools.cubeViewer.refresh();
}

document.addEventListener('keydown', (e) => {
	if (e.repeat || e.altKey) return;
	var vector = getVectorFromKey(e.key);
	if (vector.x != 0 || vector.y != 0 || vector.z != 0) {
		direction = vector;
		e.preventDefault();
		e.stopPropagation();
	}
});

export function move() {
	var currentPos = snake[snake.length - 1];
	var nextPos = currentPos.clone().add(direction);
	// if the place is free
	if (snake.every((pos) => !pos.equals(nextPos))) {
		snake.push(nextPos);
		var eatFood = food?.equals(nextPos);
		if (eatFood)
			spawnFood();
		else
			paintLed(snake.shift(), 0, false);
		paintLed(currentPos, getSnakeColor(), false);
		paintLed(nextPos, 0xFFFFFF, true);
	}
}

export function spawnFood() {
	food = getRandomPos();
	let tentative = 0;
	while (snake.some((pos) => food.equals(pos))) {
		food = getRandomPos();
		tentative++;
		if (tentative > 100) {
			alert("You broke the game! Congratulations!");
			return;
		}
	}
	paintLed(food, 0xFF0000, true);
}

export default {
	uniqueFramePainter,
	ledCubeTools: uniqueFramePainter.ledCubeTools,
	direction,
	snake,
	food,
	refreshSnake,
	move,
	spawnFood
};