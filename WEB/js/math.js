// Math equation
// by Jiogo18
// for @Jig0ll

const n = '[\\d\\.]';
const reel = `[\\+\\-]?${n}+`;
const reelOuVide = `[\\+\\-]?${n}*`;
const op = '\\+\\-\\*\\/';
const MATH = {};
function listCompare(strOperator, funcOperator) {
	// 10d6 >= 3 = (n: 5 6 4 ~~1~~ ~~1~~ 3 6 3 4 6) = 8
	return [
		`(\\d[\\d \\.${op}]*) *${strOperator} *(${n}+)`,
		match => {
			return (
				'(n: ' +
				match[1]
					.replace(/ +/, ' ')
					.replace(/^ /, '')
					.replace(/ $/, '')
					.split(' ')
					.map(v => parseFloat(v) || 0)
					.map(v => (funcOperator(v, match[2]) ? v : `~~${v}~~`))
					.join(' ') +
				')'
			);
		},
	];
}
function funcOperation(strFunction, nbArgs, funcOperator) {
	var param = new Array(nbArgs).fill(`(${reel})`);
	return [
		`${strFunction}\\\( *${param.join(' *, *')} *\\\)`,
		match => {
			const args = new Array(nbArgs).fill(0).map((v, i) => parseFloat(match[1 + i]) || undefined);
			return funcOperator(...args);
		},
	];
}
function pairOperation(strOperator, funcOperator) {
	strOperator = strOperator.replace(/(.)/g, '\\$1');
	return [
		`(${reel}) *${strOperator} *(${reelOuVide})`,
		match => funcOperator(parseFloat(match[1]) || undefined, parseFloat(match[2]) || undefined),
	];
}

const transfoCalc = [
	[
		// transfo fonctions
		funcOperation('max', 2, (a = 0, b = 0) => Math.max(a, b)),
		funcOperation('min', 2, (a = 0, b = 0) => Math.min(a, b)),
	],
	[
		// transfo sign
		[/\+\+/, () => '+'],
		[/\-\-/, () => '+'],
		[/\+\-/, () => '-)'],
		[/\-\+/, () => '-'],
		[/\+ +/, () => '+'],
		[/\- +/, () => '-'],
		[/\* +/, () => '*'],
		[/\/ +/, () => '/'],
		[/ +/, () => ' '],
	],
	[
		[`[${op}]*~~[\\d\\. ${op}]*~~`, () => '', 'before'], //deleted number
	],
	[
		[`\\(n: [\\d \\.${op}]*\\)`, match => match[0].match(new RegExp(`${n}+`, 'g'))?.length || 0], //deleted number
	],
	[
		// specialCalc
		[...listCompare('>', (a = 0, b = 0) => a > b), 'after'],
		listCompare('>=', (a = 0, b = 0) => a >= b),
		listCompare('==?', (a = 0, b = 0) => a == b),
		listCompare('<=', (a = 0, b = 0) => a <= b),
		listCompare('<', (a = 0, b = 0) => a < b),
	],
	[
		// transfoOperator
		pairOperation('||', (a = 0, b = 0) => a || b),
		pairOperation('&&', (a = 0, b = 0) => a && b),
	],
	[
		// transfoMult
		pairOperation('/', (a = 0, b = 0) => a / b), // 1 / 2 * 3
		pairOperation('*', (a = 0, b = 0) => a * b),
	],
	[
		// transfoAddition
		pairOperation('+', (a = 0, b = 0) => a + b),
		pairOperation('-', (a = 0, b = 0) => a - b),
		pairOperation(' ', (a = 0, b = 0) => a + b),
	],
	[
		// transfoAddition
		[/\( *\+? *(\d+) *\)/, match => match[1]],
		[/\( *\- *(\d+) *\)/, match => -match[1]],
		//[/\([ \+\-\*\/]*\)/, () => 0],
	],
];

MATH.parseMath = equation => {
	equation = equation.replace(/^ */, '').replace('&gt;', '>').replace('&lt;', '<');
	//`!r 1++2 * 5` donne `1+2*5`
	var previousLine;
	do {
		previousLine = equation;
		for (const transfoGrp of transfoCalc) {
			equation = applyTransfo(equation, transfoGrp);
		}
	} while (equation != previousLine && equation.length < 100000);

	if (equation == '') equation = 0;
	return equation;
};

/**
 * @param {string} equation
 * @param {[string,Function][]} rules
 */
function applyTransfo(equation, rules) {
	var i = 0;

	/**
	 * @type {Map<RegExpMatchArray,[string,Function]>}
	 */
	var matchesAndCallback = new Map();
	for (const special of rules) {
		const regex = new RegExp(special[0], 'g');
		const speMatches = Array.from(equation.matchAll(regex));
		speMatches.forEach(match => matchesAndCallback.set(match, special));
	}

	var matchesSorted = Array.from(matchesAndCallback.keys()).sort((a, b) => Math.sign(a.index - b.index));
	for (let match of matchesSorted) {
		const rule = matchesAndCallback.get(match);
		const regex = new RegExp(rule[0]);
		const replacement = rule[1](equation.match(regex));
		equation = equation.replace(match[0], replacement);

		//TODO: remove this limit
		i++;
		if (i > 100) break;
	}
	return equation;
}
