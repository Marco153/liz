let stack_ptr_offset = 10000

let mem_view = 0
let STACK_PTR_REG = 8
let BASE_STACK_PTR_REG = 9
let RET_1_REG = 10
let RET_2_REG = 11
let FILTER_PTR = 12
let MEM_PTR_CUR_ADDR = 18000
let MEM_PTR_MAX_ADDR = 18008

let DATA_SECT_OFFSET = 100000
let wasm_inst;
let should_close_val;
let ctx_addr;
let stack_ptr_addr_on_main_loop = 0;
let base_stack_ptr_addr_on_main_loop = 0;
let gl_program;

function get_ret(view) {
    return view.getBigUint64(10 * 8, true);
}
function call(func, args, view) {
    let offset = 8;
    for (let i = 0; i < args.length; i++) {
        let val = BigInt(args[i])
        view.setBigUint64(stack_ptr_offset + i * 8, val, true);
        offset += 8;
    }
    return func();
}
window.onload = start



let u_pos;
let x_pos = 0.0;
let gl;
// Vertex shader program
const vsSource = `
attribute vec3 aPos; // Vertex position
attribute vec2 uv;   // Texture coordinates

uniform vec3 pos;
uniform vec3 pivot;
uniform float cam_size;
uniform float screen_ratio;
uniform vec3 ent_size;
uniform vec3 cam_pos;

varying vec2 TexCoord; // Pass to fragment shader

void main()
{
    // Transform the vertex position
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position.xy -= pivot.xy;
    gl_Position.xy *= ent_size.xy;
    gl_Position.xy += pos.xy;
    gl_Position.xy -= cam_pos.xy;
    gl_Position.xy /= cam_size;
    gl_Position.x *= screen_ratio;

    // Pass UV coordinates to the fragment shader
    TexCoord = uv;
}\0
  `;

const fsSource = `
    void main() {
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
  `;

function initShaderProgram(gl, vsSource, fsSource) {
  const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
  const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

  // Create the shader program

  const shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);

  // If creating the shader program failed, alert

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert(
      `Unable to initialize the shader program: ${gl.getProgramInfoLog(
        shaderProgram,
      )}`,
    );
    return null;
  }

  return shaderProgram;
}

//
// creates a shader of the given type, uploads the source and
// compiles it.
//
function loadShader(gl, type, source) {
  const shader = gl.createShader(type);

  // Send the source to the shader object

  gl.shaderSource(shader, source);

  // Compile the shader program

  gl.compileShader(shader);

  // See if it compiled successfully

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert(
      `An error occurred compiling the shaders: ${gl.getShaderInfoLog(shader)}`,
    );
    gl.deleteShader(shader);
    return null;
  }

  return shader;
}
function _IsKeyDown() {
    // Implement the functionality here
}

function _IsKeyHeld() {
    // Implement the functionality here
}

function _GetTimeSinceStart() {
    // Implement the functionality here
}

function _GetDeltaTime() {
    // Implement the functionality here
}

function CallMainLoopFunc() {
    mem_view.setUint32(STACK_PTR_REG * 8, stack_ptr_addr_on_main_loop,  true);
    mem_view.setUint32(BASE_STACK_PTR_REG * 8, base_stack_ptr_addr_on_main_loop,  true);

    //base_stack_ptr_addr_on_main_loop =  mem_view.getUint32(STACK_PTR_REG, true);
    let main_loop = wasm_inst.instance.exports.main_loop;
    let base = mem_view.getUint32(STACK_PTR_REG * 8, true);
    mem_view.setUint32(base + 8, ctx_addr, true)

    main_loop()

}

function _EndFrame() {
    // Implement the functionality here
    if (stack_ptr_addr_on_main_loop == 0) {
        stack_ptr_addr_on_main_loop = mem_view.getUint32(STACK_PTR_REG * 8, true);
        base_stack_ptr_addr_on_main_loop = mem_view.getUint32(STACK_PTR_REG * 8, true);
    }
    should_close_val = 1;
    requestAnimationFrame(CallMainLoopFunc)
    let a = 0;
}

function _OpenWindow() {
    // Implement the functionality here
}

function _ClearBackground() {
    // Implement the functionality here
}

function _ShouldClose() {
    // Implement the functionality here
    mem_view.setUint32(RET_1_REG * 8, should_close_val, true)
    should_close_val = 0;
}

function _DebuggerCommand() {
    // Implement the functionality here
}

function _sin() {
    // Implement the functionality here
}

function _GetMem() {
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 8, true);
    let addr = mem_view.getUint32(MEM_PTR_CUR_ADDR, true)

    mem_view.setUint32(MEM_PTR_CUR_ADDR, addr + val, true)
    mem_view.setUint32(RET_1_REG * 8, addr, true)

    let a = 0;
    // Implement the functionality here
}
function _AssignCtxAddr() {
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 8, true);
    ctx_addr = val;
    // Implement the functionality here
}

function _Print() {
    // Implement the functionality here
}

function _sqrt() {
    // Implement the functionality here
}

function _Draw() {
    // Implement the functionality here
    //drawSquare();

    /*
uniform vec3 pos;
uniform vec3 pivot;
uniform float cam_size;
uniform float screen_ratio;
uniform vec3 ent_size;
uniform vec3 cam_pos;
*/
    u_pos = gl.getUniformLocation(gl_program, "pos");
    u_pivot = gl.getUniformLocation(gl_program, "pivot");
    u_cam_size = gl.getUniformLocation(gl_program, "cam_size");
    u_screen_ratio = gl.getUniformLocation(gl_program, "screen_ratio");
    u_ent_size = gl.getUniformLocation(gl_program, "ent_size");
    u_cam_pos = gl.getUniformLocation(gl_program, "cam_pos");
    
    gl.uniform3f(u_pos, 0, 0, 0);
    gl.uniform3f(u_pivot, 1.0, 1.0, 1.0);
    gl.uniform1f(u_cam_size, 10.0);
    gl.uniform1f(u_screen_ratio, 1.0);
    gl.uniform3f(u_ent_size, 1.0, 1.0, 1.0);
    gl.uniform3f(u_cam_pos, 1.0, 1.0, 1.0);

    drawSquare();
}

function _WasmDbg() {
    // Implement the functionality here
    let a = 0;
}
function _LoadClip() {
    // Implement the functionality here
}
async function start() {
    const imports = {
        funcs_import: {
            test_out: () => console.log("called from js"),
            IsKeyDown: _IsKeyDown,
            IsKeyHeld: _IsKeyHeld,
            GetTimeSinceStart: _GetTimeSinceStart,
            GetDeltaTime: _GetDeltaTime,
            EndFrame: _EndFrame,
            OpenWindow: _OpenWindow,
            ClearBackground: _ClearBackground,
            ShouldClose: _ShouldClose,
            DebuggerCommand: _DebuggerCommand,
            sin: _sin,
            GetMem: _GetMem,
            Print: _Print,
            sqrt: _sqrt,
            Draw: _Draw,
            LoadClip: _LoadClip,
            WasmDbg: _WasmDbg,
            AssignCtxAddr: _AssignCtxAddr,
        }
    }

    let data_sect_prom = await fetch('dbg_wasm_data.dbg');
    let data_sect = await data_sect_prom.arrayBuffer();

    await fetch('test.wasm')
        .then(response => {
            if (!response.ok) {
                throw new Error(`Network response was not ok: ${response.statusText}`);
            }
            return response.arrayBuffer();
        })
        .then(bytes => {
            const wasmBytes = new Uint8Array(bytes);
            // You can now instantiate the WebAssembly module with these bytes
            return WebAssembly.instantiate(wasmBytes, imports)
        })
        .then(result => {
            const inst = result;
            wasm_inst = inst;
            // Use the instance as needed
            let { tests } = inst.instance.exports
            let view = inst.instance.exports.mem;
            view = new DataView(view.buffer);
            view.setUint32(MEM_PTR_CUR_ADDR, 20000, true)
            mem_view = view
            // offset for rsp
            view.setUint32(8 * 8, stack_ptr_offset, true);
            view.setUint32(stack_ptr_offset + 8, 600, true);

            let uint8_data = new Uint8Array(data_sect)
            for (let i = 0; i < data_sect.byteLength; i++)
                view.setUint8(DATA_SECT_OFFSET + i, uint8_data[i], true)
                

            //console.log(call(main, [3], view))
            console.log(tests(view));
            let ret =view.getUint32(600, true);
            if (ret != 0) {
                console.error("Test at line " + ret + " failed")
            }
            let a = 0;
            //console.log(instance.exports);
        })
        .catch(error => {
            console.error('Error fetching or instantiating WASM module:', error);
        });
        /*
    let wasm_file = await fetch("test.wasm");
    let inst = WebAssembly.instantiateStreaming(fetch("test.wasm"),
        {
            funcs_import: {
                test_out: () => console.log("called from js")
            }
        }).then( (inst) => {
            //let data_sect = await fetch("dbg_wasm_data.dbg").arrayBuffer();

            let { main } = inst.instance.exports
            let view = inst.instance.exports.mem;
            view = new DataView(view.buffer);
            // offset for rsp
            view.setUint32(8 * 8, stack_ptr_offset, true);

            console.log(call(main, [3], view))
            console.log(get_ret(view));
            let a = 0;
        }).catch((err) => console.log(err))
        */
    // Get A WebGL context
    var canvas = document.querySelector("#glcanvas");
    gl = canvas.getContext("webgl");
    if (!gl) {
        return;
    }

    //ctx = WebGLDebugUtils.makeDebugContext(canvas.getContext("webgl"));

    // setup GLSL program
    var program = initShaderProgram(gl, vsSource, fsSource);
    gl_program = program;
    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);
    console.log(gl.getError());

    console.log(gl.getError());

    // look up where the vertex data needs to go.
    var positionAttributeLocation = gl.getAttribLocation(program, "aPos");
    var uvAttributeLocation = gl.getAttribLocation(program, "uv");
    console.log(gl.getError());

    // look up uniform locations
    //var resolutionUniformLocation = gl.getUniformLocation(program, "u_resolution");
    //u_pos = gl.getUniformLocation(program, "pos");
    
    //var colorUniformLocation = gl.getUniformLocation(program, "u_color");

    // Create a buffer to put three 2d clip space points in
    var positionBuffer = gl.createBuffer();
    console.log(gl.getError());

    // Bind it to ARRAY_BUFFER (think of it as ARRAY_BUFFER = positionBuffer)
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    console.log(gl.getError());

    //webglUtils.resizeCanvasToDisplaySize(gl.canvas);

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    console.log(gl.getError());

    // Clear the canvas
    gl.clearColor(0, 0, 0, 0);
    console.log(gl.getError());
    gl.clear(gl.COLOR_BUFFER_BIT);
    console.log(gl.getError());


    // Bind the position buffer.
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    console.log(gl.getError());

    // create the buffer
    const indexBuffer = gl.createBuffer();

    // make this buffer the current 'ELEMENT_ARRAY_BUFFER'
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    console.log(gl.getError());

    // Fill the current element array buffer with data
    const indices = [
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
    ];
    gl.bufferData(
        gl.ELEMENT_ARRAY_BUFFER,
        new Uint16Array(indices),
        gl.STATIC_DRAW
    );
    console.log(gl.getError());

    // code above this line is initialization code
    // --------------------------------
    // code below this line is rendering code

    // Turn on the attribute
    gl.enableVertexAttribArray(positionAttributeLocation);
    gl.enableVertexAttribArray(uvAttributeLocation);
    //gl.enableVertexAttribArray(positionAttributeLocation);
    console.log(gl.getError());

    // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 3;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(
        positionAttributeLocation, size, type, normalize, 5 * 4, 0);
    console.log(gl.getError());
    gl.vertexAttribPointer(
        uvAttributeLocation, 2, type, normalize, 5 * 4, 3 * 4);

    // bind the buffer containing the indices
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    console.log(gl.getError());

    // set the resolution
    //gl.uniform2f(resolutionUniformLocation, gl.canvas.width, gl.canvas.height);
    //setRectangle(gl, 0, 0, 300, 300);
    let main = wasm_inst.instance.exports.main;
    main()
    let a = 0;
    //requestAnimationFrame(main)
    //window.requestAnimationFrame(drawSquare);

}

function drawSquare()
{
    x_pos += 0.01;
    //console.log(x_pos);
    //gl.uniform2f(u_pos, x_pos, 0);
    setRectangle(
        gl, 0, 0, 300, 300);
    // Set a random color.
    //gl.uniform4f(colorUniformLocation, Math.random(), Math.random(), Math.random(), 1);

    // Draw the rectangle.
    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 6;
    var indexType = gl.UNSIGNED_SHORT;
    gl.drawElements(primitiveType, count, indexType, offset);
    //window.requestAnimationFrame(drawSquare);
}

// Returns a random integer from 0 to range - 1.
function randomInt(range) {
    return Math.floor(Math.random() * range);
}

// Fill the buffer with the values that define a rectangle.
function setRectangle(gl, x, y, width, height) {
    var x1 = x;
    var x2 = x + width;
    var y1 = y;
    var y2 = y + height;


    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([

		// positions          // texture coords
		 1.0,  1.0, 0.0,   1.0, 1.0,   // top right
		 1.0,   0.0, 0.0,   1.0, 0.0,   // bottom right
		 0.0,	0.0, 0.0,   0.0, 0.0,   // bottom let
		 0.0,  1.0, 0.0,   0.0, 1.0    // top let 
    ]), gl.STATIC_DRAW);
}
