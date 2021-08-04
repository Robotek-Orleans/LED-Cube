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
    controls.autoRotate = true;



    var d = distanceLED;
    var offset_x = -d * (RES - 1) / 2, offset_y = -d * (RES - 1) / 2 + 30, offset_z = -150;
    var s = 40;
    var s2 = 15;

    // floor
    var floorMaterial = new THREE.MeshPhongMaterial({
        color: 0,
        shininess: 2
    });
    var floorGeometry = new THREE.PlaneGeometry(2000, 2000, 10, 10);
    var floor = new THREE.Mesh(floorGeometry, floorMaterial);
    floor.position.y = offset_y - 30;
    floor.rotation.x = -Math.PI / 2;
    scene.add(floor);

    var textureGlow = THREE.ImageUtils.loadTexture('images/textures/led1.png');
    var texturePoint = THREE.ImageUtils.loadTexture('images/textures/led4.png');
    var textureOff = THREE.ImageUtils.loadTexture('images/textures/ledoff.png');

    materialGlow = new THREE.SpriteMaterial({ map: textureGlow, color: 0x0, transparent: true });
    materialPoint = new THREE.SpriteMaterial({ map: texturePoint, color: 0x0, transparent: true });
    materialOff = new THREE.SpriteMaterial({ map: textureOff, color: 0xffffff, transparent: true, opacity: 0.5 });

    for (var z = 0; z < RES; z++) {
        for (var y = 0; y < RES; y++) {
            for (var x = 0; x < RES; x++) {
                var sprite = new THREE.Sprite(materialOff);
                sprite.position.set(offset_x + d * x, offset_y + d * y, offset_z + d * z);
                sprite.scale.set(s2, s2, s2);
                scene.add(sprite);

                var sprite = new THREE.Sprite(materialGlow.clone());
                sprite.material.opacity = 0;
                sprite.position.set(offset_x + d * x, offset_y + d * y, offset_z + d * z);
                sprite.scale.set(s, s, s);
                scene.add(sprite);
            }
        }
    }

    for (var z = 0; z < RES; z++) {
        for (var x = 0; x < RES; x++) {
            var light = new THREE.PointLight(0x0000ff, 0.0, 700);
            light.position.set(offset_x + d * x, offset_y, offset_z + d * z);

            scene.add(light);
        }
    }

    window.addEventListener('resize', onWindowResize, false);
}

function displaySelectedPlan(plan, nb) {
    /*DEBUT TEST WIREFRAME*/
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

    if (plan === "Y") {
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[0]+nb, z[lvp[i]] ))
        }
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[0]+nb+pw, z[lvp[i]] ))
        }
        geometry.vertices.push(new THREE.Vector3(x[1], y[0]+nb, z[0]))
    }else if(plan === "X"){
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[0]+nb, y[fvp[i]], z[lvp[i]] ))
        }
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[0]+nb+pw, y[fvp[i]], z[lvp[i]] ))
        }
        geometry.vertices.push(new THREE.Vector3(x[0]+nb, y[1], z[0]))
    }else if(plan === "Z"){
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[lvp[i]], z[0]+nb ))
        }
        for(let i=0; i < fvp.length; i++){
            geometry.vertices.push(new THREE.Vector3( x[fvp[i]], y[lvp[i]], z[0]+nb+pw ))
        }
        geometry.vertices.push(new THREE.Vector3(x[1], y[0], z[0]+nb))
    }

    wireFrame = new THREE.Line(geometry, new THREE.LineBasicMaterial({ color: 0xff0000 }));
    scene.add(wireFrame);
    /*FIN TEST WIREFRAME*/
}

function onWindowResize() {
    camera.aspect = container.offsetWidth / container.offsetHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(container.offsetWidth, container.offsetHeight);
}

function animate() {
    requestAnimationFrame(animate);
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

    var i = 2 + (x + y * RES + z * RES * RES) * 2;
    return scene.children[i + 1].material.color.getHex();
}

function get_scene_offset(x, y, z) {
    return 2 + (x + y * RES + z * RES * RES) * 2;
}

function cube_set_color(x, y, z, color) {
    
    if (!cube_check_coords(x, y, z)) {
        return;
    }

    var i = get_scene_offset(x, y, z);

    if (color == 0x0) {
        // turn off the LED
        scene.children[i].material = materialOff;
        scene.children[i + 1].material.opacity = 0;
        scene.children[i + 1].material.color.setHex(color);
    } else {
        // turn on the LED
        scene.children[i].material = materialPoint.clone();
        scene.children[i].material.color.setHex(color);
        var hsl = scene.children[i].material.color.getHSL();
        scene.children[i].material.color.setHSL(hsl.h, hsl.s, 0.7);

        scene.children[i + 1].material.opacity = 1;
        scene.children[i + 1].material.color.setHex(color);
    }

    // update the light
    var light = scene.children[2 + RES * RES * RES * 2 + x + z * RES];
    var lightOff = true;

    for (var yy = 0; yy < RES; yy++) {
        var ledGlow = scene.children[get_scene_offset(x, yy, z) + 1];
        if (ledGlow.material.opacity == 1) {
            if (lightOff) {
                light.intensity = 1.0;
                light.color.setHex(ledGlow.material.color.getHex());
                light.position.y = yy * distanceLED;

                lightOff = false;
            } else {
                var mixinColor = ledGlow.material.color.clone();
                light.color.lerp(mixinColor, 1 / yy);
            }
        }
    }

    if (lightOff) {
        light.intensity = 0;
    }
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