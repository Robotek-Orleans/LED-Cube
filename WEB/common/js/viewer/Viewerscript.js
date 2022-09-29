//Script mirrored from dkobozev.github.io/webgl/led_cube/

/*
* LED Cube simulator in THREE.js
*
* Effects/animations from:
* http://www.instructables.com/id/Led-Cube-8x8x8/ by Christian Moen and
* Ståle Kristoffersen.
* http://www.kevindarrah.com/?cat=99 by Kevin Darrah
* Optimisé par
* https://github.com/Robotek-Orleans/LED-Cube/tree/main/WEB by Robotek Orléans
*/

if (!Detector.webgl) Detector.addGetWebGLMessage();

//LED draw parameters 

let container, scene, camera, renderer, controls, stats;
const RES = 8;
const distanceLED = 40;
let materialGlow, materialPoint, materialOff, wireFrame, wireAxis;
let paused = true;

const offset_x = -distanceLED * (RES - 1) / 2, offset_y = -distanceLED * (RES - 1) / 2, offset_z = -distanceLED * (RES - 1) / 2;

//Wireframe parameters
const pw = distanceLED / 2 // Plan Width
const vx = [offset_x - distanceLED / 2, -offset_x + distanceLED / 2] // xmin,xmax
const vy = [offset_y - distanceLED / 2, -offset_y + distanceLED / 2] // ymin,ymax
const vz = [offset_z - distanceLED / 2, -offset_z + distanceLED / 2] // zmin,zmax

async function initGL() {

    THREE.Object3D.DefaultUp.set(0, 0, 1);

    scene = new THREE.Scene();
    container = document.getElementById('cube_container');

    // camera
    var SCREEN_WIDTH = container.offsetWidth, SCREEN_HEIGHT = container.offsetHeight;
    var VIEW_ANGLE = 45, ASPECT = SCREEN_WIDTH / SCREEN_HEIGHT, NEAR = 0.1, FAR = 20000;
    camera = new THREE.PerspectiveCamera(VIEW_ANGLE, ASPECT, NEAR, FAR);
    scene.add(camera);
    camera.position.set(300, -700, 300);
    camera.lookAt(scene.position);

    // renderer
    if (Detector.webgl)
        renderer = new THREE.WebGLRenderer({ antialias: true });
    else
        renderer = new THREE.CanvasRenderer();
    renderer.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    container.appendChild(renderer.domElement);

    // controls
    controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.maxPolarAngle = Math.PI / 2;
    controls.addEventListener('change', () => renderer.render(scene, camera));

    const s2 = 15;

    const loader = new THREE.TextureLoader();
    var textureGlow = loader.load(webFolder + 'common/images/textures/ledon.svg');
    var textureOff = loader.load(webFolder + 'common/images/textures/ledoff.svg');
    textureGlow.minFilter = textureOff.minFilter = THREE.LinearFilter;

    materialOff = new THREE.SpriteMaterial({ map: textureOff, color: 0xffffff, transparent: true, opacity: 0.5 });
    materialGlow = new THREE.SpriteMaterial({ map: textureGlow, color: 0x0, transparent: true });

    for (let z = 0; z < RES; z++) {
        for (let y = 0; y < RES; y++) {
            for (let x = 0; x < RES; x++) {
                const sprite = new THREE.Sprite(materialOff);
                sprite.position.set(offset_x + distanceLED * x, offset_y + distanceLED * y, offset_z + distanceLED * z);
                sprite.scale.set(s2, s2, s2);
                scene.add(sprite);
            }
        }
    }

    window.addEventListener('resize', onWindowResize, false);


    renderer.render(scene, camera);
}

function displaySelectedPlan(plan, nb) {
    //DRAW WIREFRAME

    plan = plan.toUpperCase()
    var nb_offset = nb * distanceLED + distanceLED / 4

    scene.remove(wireFrame)

    // origin of the slab
    const x0 = vx[0] + (plan == 'X' ? nb_offset : 0);
    const y0 = vy[0] + (plan == 'Y' ? nb_offset : 0);
    const z0 = vz[0] + (plan == 'Z' ? nb_offset : 0);
    // size of the slab
    const sx = plan == 'X' ? pw : vx[1] - vx[0];
    const sy = plan == 'Y' ? pw : vy[1] - vy[0];
    const sz = plan == 'Z' ? pw : vz[1] - vz[0];

    // create the box
    const geometry = new THREE.BoxGeometry(sx, sy, sz);
    geometry.translate(x0 + sx / 2, y0 + sy / 2, z0 + sz / 2);

    // link the dots
    const link_points = [0, 2, 3, 1, 0, 5, 7, 2, 7, 6, 3, 6, 8, 1, 8, 5];
    var array = new Uint16Array(link_points.length);
    array.set(link_points);
    geometry.index.array = array;
    geometry.index.count = link_points.length;

    wireFrame = new THREE.Line(geometry, new THREE.LineBasicMaterial({ color: 0xff0000 }));
    scene.add(wireFrame);
    renderer.render(scene, camera);
}

function displayAxis() {
    //DRAW WIREAXIS (array of 3 wires)
    const points = [
        { vec: [1, 0, 0], color: 0xFF0000 },
        { vec: [0, 1, 0], color: 0x00FF00 },
        { vec: [0, 0, 1], color: 0x0000FF }
    ];

    wireAxis?.forEach(wire => scene.remove(wire));
    wireAxis = [];

    for (const point of points) {
        const vec = point.vec.map(v => v ? v * 2 * distanceLED : 10);

        const geometry = new THREE.BoxGeometry(vec[0], vec[1], vec[2]);

        geometry.translate(
            vx[0] + vec[0] / 2 + point.vec[0] * 10,
            vy[0] + vec[1] / 2 + point.vec[1] * 10,
            vz[0] + vec[2] / 2 + point.vec[2] * 10
        );

        var wire = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({ color: point.color, transparent: true, opacity: 0.5 }));


        wireAxis.push(wire);
        scene.add(wire);
    }

    renderer.render(scene, camera);
}

function removeAxis() {
    wireAxis?.forEach(wire => scene.remove(wire));
    wireAxis = [];
    renderer.render(scene, camera);
}

function onWindowResize() {
    camera.aspect = container.offsetWidth / container.offsetHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(container.offsetWidth, container.offsetHeight);

    renderer.render(scene, camera);
}


//  Set fps to 50 (20 ms) 
function animate() {
    if (paused) return;
    setTimeout(function () {
        requestAnimationFrame(animate);
    }, 20);
    renderer.render(scene, camera);
}

function cube_check_coords(x, y, z) {
    return (x >= 0 && x < RES && y >= 0 && y < RES && z >= 0 && z < RES);
}

function cube_get_color(x, y, z) {
    scene.remove(wireFrame)
    if (!cube_check_coords(x, y, z)) {
        return -1;
    }

    var i = 1 + x + y * RES + z * RES * RES;
    return scene.children[i].material.color.getHex();
}

function get_scene_offset(x, y, z) {
    return 1 + x + y * RES + z * RES * RES;
}

function cube_set_color(x, y, z, color, refresh = true) {

    const s = 40;
    const s2 = 15;

    if (!cube_check_coords(x, y, z)) {
        return;
    }

    const i = get_scene_offset(x, y, z);

    if (color == 0x0) {
        // turn off the LED
        scene.children[i].material = materialOff;
        scene.children[i].scale.set(s2, s2, s2);
    } else {
        // turn on the LED
        scene.children[i].material = materialGlow.clone();
        scene.children[i].material.color.setHex(color);
        scene.children[i].scale.set(s, s, s);
    }

    if (refresh) renderer.render(scene, camera);
}

function cube_clear(color) {
    if (typeof color === 'undefined') {
        color = 0x0;
    }

    for (var z = 0; z < RES; z++) {
        for (var y = 0; y < RES; y++) {
            for (var x = 0; x < RES; x++) {
                cube_set_color(x, y, z, color);
            }
        }
    }
}

function clear3DViewPlan() {
    scene.remove(wireFrame);
    renderer.render(scene, camera);
}

