### Warning

This language is not at all production ready, it changes all the time, it breaks often, so dont try to do anything serious with it.

This a very c/c++-esque language, if you know c++, you probably wont have too much trouble here.

And the compiler it only runs in windows x64.

## How to compile

At the moment, the compiler only compiles a whole folder with files inside.
It has two commands: ```run``` and ```webgame```.

```run``` will compile and run the program at the same time

```liz.exe run path/to/folder```

```webgame``` will compile and output a index.html file to a specified path. It's similar to run, in that it also compiles a whole folder, but it differs from it because it also expects the folder to have another folder called a "image" and the folder containing the code to be named "files", within which contains the
game textures. Webgame will use base64 to encode both the wasm code and the textures data in some html tags, so that it doesnt need to do any fetching.

```liz.exe webgame path/to/folder path/to/output```

## Syntax
### Declaring variables
This declares a 32-bit unsigned integer

```a :u32= 0```

A 32-bit signed integer

```a :s32= 0```

The compiler can also infer the type

```a := 0```

By the default that will be s32.
Besides 32-bit, there is 8-bit, 16-bit, 32-bit and 64-bit too. Although even if you declare a 64-bit variable and compile using webgame, the size will downgraded to 32-bit, so be careful about that.

### Conditions

```
if a > 0
{
  a = 2;
}
```

Here, parentheses are optional, also if you only have one statement curly braces are also optional
```
if a > 0 a = 2
```

There's also a basic if expression
```
val := if a == 1 2 else 3;
```
if a is 1 val will be three, else val will be 3.

### Functions

```
AddTwoNumbers::fn(a : s32, b : s32) ! s32
{
  return a + b;
}
```
Everything between parentheses are the parameters, exclamation mark says what type the function returns.

You can create function ptrs like this
```
func_ptr :fn(s32, s32) ! s32;
func_ptr = AddTwoNumbers;
val := func_ptr(2, 4);
```

### Loops

For the time being, the language only support while loops
```
i :u32= 0;
while i < 5
{
  i++;
}

```
you can also ```break``` out of a loop, there's no ```continue``` yet.

## Arrays

```
numbers :[4]u32= u32{0, 1, 2, 4};
val := numbers[0] + numbers[1];
```
## Pointers

```
  ptr := &a;
  *ptr = 1
```
```a``` will be one.

Ptr indexing is not allowed. To achive that ```(cast(u64)ptr + idx * sizeof(type))```, this is crazier code than the good old ```ptr[i]```, but i'm not sure if im gonno stick the current way.


### Structures

```
node : struct
{
  next : *node,
}
```
This declares a type structure called node, you can create variables based on that type

```
node :: struct
{
  next : *node,
}
```

Notice the double colon, it's different. Now it declares a type and makes a variable out of it.

Theres also ```unions``` and ```enums``` here

unions ara likes structs, but every member occupies the same address in memory.

```
fruits :enum
{
  APPLE,
  ORANGE,
  BANANA, 
}
...
f := fruits.APPLE;
```

## Templates

This language has a limited template functionality, it's nothing too crazy at the moment

```
array : struct(T)
{
  data : *T,
  len : u32,
}
...
ar :array(u32);
```

## Operator overloading

The language also has basic operator overloading
Here's a v3 struct
```
v3 : struct
{
	x : f32,
	y : f32,
	z : f32,
}
operator *(self : *v3, f : f32) !v3
{
	ret:v3;
	ret.x = self.x * f;
	ret.y = self.y * f;
	ret.z = self.z * f;
	return ret;
}
operator -(self : *v3, rhs : *v3) !v3
{
	ret:v3;
	ret.x = self.x - rhs.x;
	ret.y = self.y - rhs.y;
	ret.z = self.z - rhs.z;
	return ret;
}
operator +(self : *v3, rhs : *v3) !v3
{
	ret:v3;
	ret.x = self.x + rhs.x;
	ret.y = self.y + rhs.y;
	ret.z = self.z + rhs.z;
	return ret;
}
...
pos1 :v3;
pos2 :v3;
pos3 := pos1 + pos2;
```
```pos3``` will be the sum of ```pos1``` and ```pos2```



### Conclusion

I think that's basically it. In the ```examples``` theres a couple folders, try compiling and running them.
If you want to compile and run the game simply type this cmd in a prompt ```liz.exe run ../examples/game/files```
If you want to build the webgame simply type this cmd in a prompt ```liz.exe webgame ../examples/game/ ../web```
The web folder has a file called ```main.js```, it is the renderer of the game, make sure that the index.html is always in the same folder as that script, otherwise it wont work.
