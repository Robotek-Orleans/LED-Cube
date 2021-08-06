//Script mirrored from dkobozev.github.io/webgl/led_cube/

/*
* LED Cube simulator in THREE.js
*
* Effects/animations from:
* http://www.instructables.com/id/Led-Cube-8x8x8/ by Christian Moen and
* Ståle Kristoffersen.
* http://www.kevindarrah.com/?cat=99 by Kevin Darrah
*/

if (!Detector.webgl) Detector.addGetWebGLMessage();

let container, scene, camera, renderer, controls, stats;
const RES = 8;
const distanceLED = 40;
let materialGlow, materialPoint, materialOff, wireFrame;
let paused = true;

var colorOn = 1;

function initGL() {

    scene = new THREE.Scene();
    container = document.getElementById('container');

    // camera
    var SCREEN_WIDTH = container.offsetWidth, SCREEN_HEIGHT = container.offsetHeight;
    var VIEW_ANGLE = 45, ASPECT = SCREEN_WIDTH / SCREEN_HEIGHT, NEAR = 0.1, FAR = 20000;
    camera = new THREE.PerspectiveCamera(VIEW_ANGLE, ASPECT, NEAR, FAR);
    scene.add(camera);
    camera.position.set(-400, 500, 1000);
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


    var d = distanceLED;
    var offset_x = -d * (RES - 1) / 2, offset_y = -d * (RES - 1) / 2 + 30, offset_z = -150;
    const s2 = 15;

    var textureGlow = THREE.ImageUtils.loadTexture('images/textures/ledon.svg');
    var textureOff = THREE.ImageUtils.loadTexture('images/textures/ledoff.svg');

    materialGlow = new THREE.SpriteMaterial({ map: textureGlow, color: 0x0, transparent: true });
    materialOff = new THREE.SpriteMaterial({ map: textureOff, color: 0xffffff, transparent: true, opacity: 0.5 });

    for (var z = 0; z < RES; z++) {
        for (var y = 0; y < RES; y++) {
            for (var x = 0; x < RES; x++) {
                var sprite = new THREE.Sprite(materialOff);
                sprite.position.set(offset_x + d * x, offset_y + d * y, offset_z + d * z);
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
    var geometry = new THREE.Geometry();

    plan = plan.toUpperCase()
    nb = (nb - 1) * 40
    
    const pw = 20 // Plan Width
    
    scene.remove(wireFrame)

    const x = [ -150, 150] // xmin,xmax
    const y = [ -120, 180] // ymin,ymax
    const z = [ -160, 140] // zmin,zmax
    const fvp = [0,0,1,1,0] // First Vertex Patern
    const lvp = [0,1,1,0,0] // Last Vertex Patern

    switch(plan){
        case "X":
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[0]+nb, y[fvp[i]], z[lvp[i]] ))
            }
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[0]+nb+pw, y[fvp[i]], z[lvp[i]] ))
            }
            geometry.vertices.push(new THREE.Vector3(x[0]+nb, y[1], z[0]))
            break;
        case "Y":
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[0]+nb, z[lvp[i]] ))
            }
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[0]+nb+pw, z[lvp[i]] ))
            }
            geometry.vertices.push(new THREE.Vector3(x[1], y[0]+nb, z[0]))
            break;
        case "Z":
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[lvp[i]], z[0]+nb ))
            }
            for(let i=0; i < fvp.length; i++){
                geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[lvp[i]], z[0]+nb+pw ))
            }
            geometry.vertices.push(new THREE.Vector3(x[1], y[0], z[0]+nb))
            break;
    }

    wireFrame = new THREE.Line(geometry, new THREE.LineBasicMaterial({ color: 0xff0000 }));
    scene.add(wireFrame);
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
    if(paused) return;
    setTimeout( function() {
        requestAnimationFrame(animate);
    } , 20 );
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

    var i = 1 + (x + y * RES + z * RES * RES) * 2;
    return scene.children[i + 1].material.color.getHex();
}

function get_scene_offset(x, y, z) {
    return 1 + (x + y * RES + z * RES * RES);
}

function cube_set_color(x, y, z, color) {

    const s = 40;
    const s2 = 15;
    
    if (!cube_check_coords(x, y, z)) {
        return;
    }

    var i = get_scene_offset(x, y, z);

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

    renderer.render(scene, camera);
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

