let stack_ptr_offset = 10000

let mem_view = 0
let STACK_PTR_REG = 8
let BASE_STACK_PTR_REG = 9
let RET_1_REG = 10
let RET_2_REG = 11
let FILTER_PTR = 12
let MEM_PTR_CUR_ADDR = 18000
let MEM_PTR_MAX_ADDR = 18008

let KEY_HELD =  1
let KEY_DOWN= 2
let KEY_UP =   4
let KEY_RECENTLY_DOWN =   8

let TOTAL_KEYS =   255
let TOTAL_TEXTURES =   32

let DATA_SECT_OFFSET = 130000

let DRAW_INFO_HAS_TEXTURE = 1
let DRAW_INFO_NO_SCREEN_RATIO = 2

let wasm_inst;
let should_close_val;
let ctx_addr;
let stack_ptr_addr_on_main_loop = 0;
let base_stack_ptr_addr_on_main_loop = 0;
let gl_program;
let textures_raw = [];
let textures_gl=[];

let keys = []



const key_enum = Object.freeze({
    _KEY_LEFT: 0,
    _KEY_RIGHT: 1,
    _KEY_DOWN: 2,
    _KEY_UP: 3,
    _KEY_ACT0: 4,
    _KEY_ACT1: 5,
    _KEY_ACT2: 6,
    _KEY_ACT3: 7
});
async function HasRawTexture(name) {
    for (let i = 0; i < textures_raw.length; i++) {
        let cur = textures_raw[i];
        if (cur.name == name)
            return cur;
    }
    let file = await fetch("images/"+name)
    let bleb = await file.blob();
}
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
precision mediump float;

varying vec2 TexCoord;  // This corresponds to 'in vec2 TexCoord;' in GLSL 3.3
uniform vec4 color;     // This corresponds to 'uniform vec4 color;'
uniform sampler2D tex;  // This corresponds to 'uniform sampler2D tex;'

void main() {
    vec4 texCol = texture2D(tex, TexCoord);  // texture2D is the WebGL 1.0 version of texture()
    gl_FragColor = texCol * color;            // This corresponds to 'FragColor' in GLSL 3.3
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
function DecodeBase64ToUintArray(encoded) {
    let encoded_base_64 = encoded
    let decoded_base_64 = atob(encoded_base_64)
    const byteNumbers = new Uint8Array(decoded_base_64.length);
    for (let i = 0; i < decoded_base_64.length; i++) {
        byteNumbers[i] = decoded_base_64.charCodeAt(i);
    }
    return byteNumbers;
 
}
function GetTextureSlotId()
{
	for (let i = 0; i < TOTAL_TEXTURES; i++)
    {
        let t = textures_gl[i];
		if (!t.used)
		{
			t.used = true;
			return i;
		}
    }
    console.error("no texture id slot available")
    throw new Error()
}
function HasRawTexture(name) {
    for (let i = 0; i < textures_raw.length; i++)
    {
        let cur = textures_raw[i]
        if (cur.name == name)
            return cur;
    }
    console.error("texture not found")
    throw new Error()
}
function GenTexture(src, spWidth, spHeight, xOffset, yOffset, width, height, spIdx) {
    //const gl = glState.gl;

    // Generate and bind texture
    const texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);

    // Set texture wrapping/filtering options
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    // Allocate buffer for subimage
    const spData = new Uint8Array(spWidth * spHeight * 4);

    // Adjust yOffset based on height and spHeight
    yOffset = height - (yOffset + spHeight);

    // Copy pixel data from source image
    for (let i = 0; i < spHeight; i++) {
        const curXOffset = xOffset + spIdx * spWidth * 4;
        const srcOffset = curXOffset + ((i + yOffset) * width * 4);
        spData.set(src.subarray(srcOffset, srcOffset + spWidth * 4), i * spWidth * 4);
    }

    // Upload the texture to WebGL
    if (spData) {
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, spWidth, spHeight, 0, gl.RGBA, gl.UNSIGNED_BYTE, spData);
        //gl.generateMipmap(gl.TEXTURE_2D);
    } else {
        console.log("Failed to load texture");
    }

    // Get texture slot and store the texture in the glState
    const idx = GetTextureSlotId();
    const tex = textures_gl[idx];
    tex.id = texture;

    return idx;
}
function FromGameToGLFWKey(input) {
    let key;
    switch (input) {
        case key_enum._KEY_UP:
            key = 'w';
            break;
        case key_enum._KEY_DOWN:
            key = 's';
            break;
        case key_enum._KEY_RIGHT:
            key = 'd';
            break;
        case key_enum._KEY_ACT1:
            key = 'r';
            break;
        case key_enum._KEY_ACT0:
            key = 'z';
            break;
        case key_enum._KEY_LEFT:
            key = 'a';
            break;
        default:
            throw new Error("Invalid key enum value");
    }
    return key.charCodeAt(0);
}
function _IsKeyDown() {
    // Implement the functionality here
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 8, true);
    let key = FromGameToGLFWKey(val);

    if (IS_FLAG_ON(keys[key], KEY_DOWN) || IS_FLAG_ON(keys[key], KEY_RECENTLY_DOWN))
    {
        keys[key] &= ~KEY_RECENTLY_DOWN;
        mem_view.setUint32(RET_1_REG * 8, 1, true);
    }
    else
        mem_view.setUint32(RET_1_REG * 8, 0, true);
}

function _IsKeyHeld() {
    // Implement the functionality here
    mem_view.setUint32(RET_1_REG * 8, 0, true);
}

function _GetTimeSinceStart() {
    // Implement the functionality here
}

function _GetDeltaTime() {
    // Implement the functionality here
    let float_var = 3.14;  // The float value you want to reinterpret

    // Create a buffer to hold 4 bytes (since a 32-bit float and a 32-bit int are both 4 bytes)
    let buffer = new ArrayBuffer(4);

    // Create a Float32Array and set the float value
    let floatView = new Float32Array(buffer);
    floatView[0] = cur_dt / 1000;

    // Create an Int32Array that shares the same buffer to access the same bits as an integer
    let intView = new Int32Array(buffer);
    mem_view.setUint32(RET_1_REG * 8, intView[0], true);

}
let start_time
let cur_dt
function CallMainLoopFunc(timestamp) {
    if(start_time === undefined)
        start_time = timestamp

    cur_dt = timestamp - start_time
    start_time =timestamp

    let doc_dt = document.getElementById("dt")
    doc_dt.textContent = cur_dt

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
	for (let i = 0; i < TOTAL_KEYS; i++)
	{
		let retain_flags = keys[i] & KEY_HELD;
		keys[i] &= ~(KEY_DOWN | KEY_UP);
		keys[i] |= retain_flags;

		if (IS_FLAG_ON(keys[i], KEY_RECENTLY_DOWN))
		{
			let held_from = (keys[i] >> 16);
			held_from++;
			if (held_from > 40)
			{
				keys[i] &= ~KEY_RECENTLY_DOWN;
				keys[i] = (keys[i] & 0xffff);
			}
			else
			{
				keys[i] = (keys[i] & 0xffff) | (held_from << 16);
			}
		}
	}
}

function _OpenWindow() {
    // Implement the functionality here

}

function _ClearBackground() {
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let r = mem_view.getFloat32(Number(base) + 8, true);
    let g = mem_view.getFloat32(Number(base) + 16, true);
    let b = mem_view.getFloat32(Number(base) + 24, true);
    let a = mem_view.getFloat32(Number(base) + 32, true);
    // Implement the functionality here
    gl.clearColor(r, g, b, 1.0);

    // Set the clear depth value
    gl.clearDepth(1.0);

    // Clear the color and depth buffers
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
}

function IS_FLAG_ON(flag, val) {
    return (flag & val) != 0
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

function _SubMem() {
    let base = mem_view.getUint32(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(base + 8, true);
    let addr = mem_view.getUint32(MEM_PTR_CUR_ADDR, true)

    mem_view.setUint32(MEM_PTR_CUR_ADDR, addr - val, true)

    if ((addr - val) <= 0)
    {
        console.error("too much memory to sub: cur_addr "+addr+", sz "+ val)
        throw new Error()
    }

    let a = 0;
    // Implement the functionality here
}
function _GetMem() {
    let base = mem_view.getUint32(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(base + 8, true);
    let addr = mem_view.getUint32(MEM_PTR_CUR_ADDR, true)

    if ((addr + val) > DATA_SECT_OFFSET)
    {
        console.error("too much memory asked: cur_addr "+addr+", sz "+ val)
        throw new Error()
    }

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
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getFloat32(Number(base) + 8, true);
    let s = Math.sqrt(val)
    mem_view.setFloat32(RET_1_REG * 8, s, true);
    let a = 0;
    // Implement the functionality here
}
function FromMemOffsetToV3(offset, f32_buffer) {
    f32_buffer[0] = 
    f32_buffer[1] = mem_view.getFloat32(offset + 4, true)
    f32_buffer[2] = mem_view.getFloat32(offset + 8, true)
}
let aux_buffer = new ArrayBuffer(16);
let aux_buffer_32 = new Float32Array(12);
let u_pos;
let u_color;
let u_pivot;
let u_cam_size;
let u_screen_ratio;
let u_ent_size;
let u_cam_pos;
function _PrintV3() {
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 8, true);
    //FromMemOffsetToV3(Number(base) + 8, aux_buffer_32);
    let offset = Number(base) + 8
    let f0= mem_view.getFloat32(offset, true)
    let f1= mem_view.getFloat32(offset + 8, true)
    let f2= mem_view.getFloat32(offset + 16, true)
    console.log(`x: ${f0}, y: ${f1}, z: ${f2}`);
}
function _Draw() {
    // Implement the functionality here

    /*
uniform vec3 pos;
uniform vec3 pivot;
uniform float cam_size;
uniform float screen_ratio;
uniform vec3 ent_size;
uniform vec3 cam_pos;
*/
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 16, true);

    

    let offset = val
    let pos_x= mem_view.getFloat32(offset, true)
    let pos_y= mem_view.getFloat32(offset + 4, true)
    let pos_z= mem_view.getFloat32(offset + 8, true)
    offset += 12

    let pivot_x = mem_view.getFloat32(offset, true)
    let pivot_y = mem_view.getFloat32(offset + 4, true)
    let pivot_z = mem_view.getFloat32(offset + 8, true)
    offset += 12

    let ent_sz_x = mem_view.getFloat32(offset, true)
    let ent_sz_y = mem_view.getFloat32(offset + 4, true)
    let ent_sz_z = mem_view.getFloat32(offset + 8, true)
    offset += 12

    let color_r = mem_view.getFloat32(offset, true)
    let color_g = mem_view.getFloat32(offset + 4, true)
    let color_b = mem_view.getFloat32(offset + 8, true)
    let color_a = mem_view.getFloat32(offset + 12, true)
    offset += 16

    let tex_id = mem_view.getUint32(offset, true)
    offset += 4

    let cam_sz = mem_view.getFloat32(offset, true)
    offset += 4

    let cam_pos_x = mem_view.getFloat32(offset, true)
    let cam_pos_y = mem_view.getFloat32(offset + 4, true)
    let cam_pos_z = mem_view.getFloat32(offset + 8, true)
    offset += 12

    let flags = mem_view.getUint32(offset, true)
    offset += 4
    cam_pos_x = cam_pos_x;
    cam_pos_y = cam_pos_y;

    if (IS_FLAG_ON(flags, DRAW_INFO_HAS_TEXTURE)) {
        let t_id = textures_gl[tex_id].id
        gl.bindTexture(gl.TEXTURE_2D, t_id);
    }
    gl.uniform3f(u_pos, pos_x, pos_y, 0);
    gl.uniform3f(u_pivot, pivot_x, pivot_y, 1.0);
    gl.uniform1f(u_cam_size, cam_sz);

    let screen_ratio = 0.8;
	if (IS_FLAG_ON(flags, DRAW_INFO_NO_SCREEN_RATIO))
	{
		screen_ratio = 1;
	}

    gl.uniform1f(u_screen_ratio, screen_ratio);
    gl.uniform3f(u_ent_size, ent_sz_x, ent_sz_y, 1.0);
    gl.uniform3f(u_cam_pos, cam_pos_x, cam_pos_y, 1.0);
    gl.uniform4f(u_color, color_r, color_g, color_b, color_a);


    //if (start_time && start_time > 3000)
        //x_pos += 0.01
    drawSquare();
}

function _WasmDbg() {
    let base = mem_view.getBigUint64(STACK_PTR_REG * 8, true);
    let val = mem_view.getUint32(Number(base) + 8, true);
    let scene_addr = mem_view.getUint32(val, true);
    let cels_buffer = new ArrayBuffer(16);
    let int_ar = new Int32Array(cels_buffer)
    int_ar[0] = scene_addr;
    int_ar[1] = mem_view.getUint32(scene_addr, true);
    //console.log(int_ar)



    // Implement the functionality here
    let a = 0;
}
function _LoadTex(dbg) {
    //const basePtr = dbg.memBuffer[STACK_PTR_REG * 8];
    let uint8_ar = new Uint8Array(mem_view.buffer)
    let base = mem_view.getUint32(STACK_PTR_REG * 8, true);

    let ctx = mem_view.getUint32(base + 8, true);          // ctx - unsigned long long (8 bytes)
    let file_name = mem_view.getUint32(base + 16, true);      // Assuming a pointer or handle for file_name (4 or 8 bytes, depending on platform)
    let x_offset = mem_view.getUint32(base + 24, true);    // x_offset - unsigned long long (8 bytes)
    let y_offset = mem_view.getUint32(base + 32, true);    // y_offset - unsigned long long (8 bytes)
    let sp_width = mem_view.getUint32(base + 40, true);    // sp_width - unsigned long long (8 bytes)
    let sp_height = mem_view.getUint32(base + 48, true);   // sp_height - unsigned long long (8 bytes)
    let total_sps = mem_view.getUint32(base + 56, true);   // total_sps - unsigned long long (8 bytes)

    let decoder = new TextDecoder();
    let name_len = 0;
    while (mem_view.getUint8(file_name + name_len) != 0) {
        name_len++;
    }
    let name = decoder.decode(uint8_ar.subarray(file_name, file_name + name_len));

    const texRaw = HasRawTexture(name);
    if (!texRaw) {
        return;
    }

    const src = texRaw.data;
    const width = texRaw.width;
    const height = texRaw.height;
    const nrChannels = texRaw.channels;
    if (sp_width == 0) {
        sp_width = width
        sp_height = height
    }

    const idx = GenTexture(texRaw.data, sp_width, sp_height, x_offset, y_offset, width, height, 0);

    //dbg.memBuffer[RET_1_REG * 8] = idx;
    mem_view.setUint32(RET_1_REG * 8, idx, true)
}

function _LoadClip(dbg) {
    let uint8_ar = new Uint8Array(mem_view.buffer)
    let base = mem_view.getUint32(STACK_PTR_REG * 8, true);

    let ctx = mem_view.getUint32(base + 8, true);          // ctx - unsigned long long (8 bytes)
    let file_name = mem_view.getUint32(base + 16, true);      // Assuming a pointer or handle for file_name (4 or 8 bytes, depending on platform)
    let x_offset = mem_view.getUint32(base + 24, true);    // x_offset - unsigned long long (8 bytes)
    let y_offset = mem_view.getUint32(base + 32, true);    // y_offset - unsigned long long (8 bytes)
    let sp_width = mem_view.getUint32(base + 40, true);    // sp_width - unsigned long long (8 bytes)
    let sp_height = mem_view.getUint32(base + 48, true);   // sp_height - unsigned long long (8 bytes)
    let total_sps = mem_view.getUint32(base + 56, true);   // total_sps - unsigned long long (8 bytes)
    let len = mem_view.getFloat32(base + 64, true);   
    let clip_info_ptr = mem_view.getUint32(base + 72, true);   


	mem_view.setUint32(STACK_PTR_REG * 8, base - 16, true);
	mem_view.setUint32(base + 8, total_sps * 4, true);
	//*(int*)&dbg->mem_buffer[STACK_PTR_REG * 8 + 8] = info->total_sps * sizeof(int);
	///int idx = 0;
	
	_GetMem();

	let offset = mem_view.getUint32(RET_1_REG * 8, true);

    mem_view.setUint32(clip_info_ptr, offset, true);
    mem_view.setUint32(clip_info_ptr + 8, total_sps, true);
    mem_view.setFloat32(clip_info_ptr + 16, len, true);

    base = mem_view.getUint32(STACK_PTR_REG * 8, true);
	mem_view.setUint32(STACK_PTR_REG * 8, base + 16, true);

    let decoder = new TextDecoder();
    //let name_sub_array = mem_view.subarray(file_name);
    let name_len = 0;
    while (mem_view.getUint8(file_name + name_len) != 0) {
        name_len++;
    }
    let name = decoder.decode(uint8_ar.subarray(file_name, file_name + name_len));

    const texRaw = HasRawTexture(name);
    if (!texRaw) {
        return;
    }

    const src = texRaw.data;
    const width = texRaw.width;
    const height = texRaw.height;
    const nrChannels = texRaw.channels;

    for (let i = 0; i < total_sps; i++) {
        let tex_id = GenTexture(texRaw.data, sp_width, sp_height, x_offset, y_offset, width, height, i)
        mem_view.setUint32(offset + i * 4, tex_id, true)
    }
}
function _PrintStr() {
    // Implement the functionality here
}
function SetUpWasm(result, data_sect) {
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
    //console.log(tests(view));
    let ret =view.getUint32(600, true);
    if (ret != 0) {
        console.error("Test at line " + ret + " failed")
    }
    let a = 0;
    //console.log(instance.exports);
}
function Stub() {

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
            SubMem: _SubMem,
            LoadTex: _LoadTex,
            LoadClip: _LoadClip,
            Print: _Print,
            sqrt: _sqrt,
            Draw: _Draw,
            PrintV3: _PrintV3,
            LoadClip: _LoadClip,
            WasmDbg: _WasmDbg,
            PrintStr: _PrintStr,
            AssignTexFolder: Stub,
            AssignCtxAddr: _AssignCtxAddr,
        }
    }

    let wasm_data_sect_bytes = DecodeBase64ToUintArray(document.getElementById("wasm_data_sect").textContent);
    //let data_sect_prom = await fetch('files_data_sect');
    let data_sect = wasm_data_sect_bytes;

    {
        textures_gl = Array.from({ length: TOTAL_TEXTURES }, ()=> ({id: 0, used: false}))

        let imgs_buffer = DecodeBase64ToUintArray(document.getElementById("wasm_images").textContent);
        let data_view = new DataView(imgs_buffer.buffer)
        //let imgs_prom = await fetch('images.data');
        //let imgs_buffer = await imgs_prom.arrayBuffer();
        let int_ar = new Uint32Array(imgs_buffer)
        let char_ar = new Uint8Array(imgs_buffer)

        let total_imgs = data_view.getUint32(0, true)
        let data_sect_offset = data_view.getUint32(4, true)
        let str_tbl_offset = data_view.getUint32(8, true)

        let str_table = char_ar.subarray(4 * 3 + str_tbl_offset);
        let data_sect = char_ar.subarray(4 * 3 + data_sect_offset);

        let decoder = new TextDecoder()
        for (let i = 0; i < total_imgs; i++) {
            let name_in_str_table = data_view.getUint32((i * 5) * 4 + 3 * 4, true)

            let name_len = 0
            while (str_table[name_len + name_in_str_table] != 0) {
                console.log(`name len ${name_len} name in str table ${name_in_str_table}`)
                name_len++;
                if(name_len > 200)
                    return
            }
            //return;
            let name_sub_array = str_table.subarray(name_in_str_table, name_in_str_table + name_len)

            let width = data_view.getUint32((i * 5 + 1) * 4 + 3 * 4, true)
            let height = data_view.getUint32((i * 5 + 2) * 4 + 3 * 4, true)
            let channels = data_view.getUint32((i * 5 + 3) * 4 + 3 * 4, true)
            let offset_in_data_sectt = data_view.getUint32((i * 5 + 4) * 4 + 3 * 4, true)

            let name = decoder.decode(name_sub_array)
            let data = data_sect.subarray(offset_in_data_sectt, offset_in_data_sectt + (width * height * channels));

            textures_raw.push({name: name, data: data, width: width, height: height, channels: channels })
            /*
            let name_uint8 = 
            const tex =
            {
                name: new TextDecoder().decode()

            }
            */
        }
        let a = 0;
    }
    // You can now instantiate the WebAssembly module with these bytes
    let wasm_bytes = DecodeBase64ToUintArray(document.getElementById("wasm_module").textContent);
    let result = await WebAssembly.instantiate(wasm_bytes, imports)
    SetUpWasm(result, data_sect)

    await fetch('files.wasm')
        .then(response => {
            if (!response.ok) {
                throw new Error(`Network response was not ok: ${response.statusText}`);
            }
            return response.arrayBuffer();
        })
        .then(bytes => {
        })
        .then(result => {
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
    document.addEventListener('keydown', (e) => {
        keys[e.key.charCodeAt(0)] |= KEY_DOWN | KEY_HELD | KEY_RECENTLY_DOWN
        console.log(`Key "${e.key.charCodeAt(0)}" pressed`);
    });

    document.addEventListener('keyup', (e) => {
        keys[e.code] |= KEY_UP
        console.log(`Key "${e.key}" released`);
    });
    gl = canvas.getContext("webgl");
    if (!gl) {
        return;
    }
    const audioPlayer = document.getElementById('music');
    document.addEventListener("click", ()=> {
        audioPlayer.play();
        audioPlayer.loop = true;

    })

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
    u_pos = gl.getUniformLocation(gl_program, "pos");
    u_color = gl.getUniformLocation(gl_program, "color");
    u_pivot = gl.getUniformLocation(gl_program, "pivot");
    u_cam_size = gl.getUniformLocation(gl_program, "cam_size");
    u_screen_ratio = gl.getUniformLocation(gl_program, "screen_ratio");
    u_ent_size = gl.getUniformLocation(gl_program, "ent_size");
    u_cam_pos = gl.getUniformLocation(gl_program, "cam_pos");

    gl.enable(gl.BLEND);

    gl.enable(gl.DEPTH_TEST);

    gl.depthMask(false);

    gl.depthFunc(gl.ALWAYS);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);


    main()
    let a = 0;
    //requestAnimationFrame(main)
    //window.requestAnimationFrame(drawSquare);

}

function drawSquare()
{
    //x_pos += 0.01;
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
