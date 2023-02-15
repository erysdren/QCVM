
//
//
// exports
//
//

// print string to stdoout
void print(string s, ...) = #1;

// acquire entity handle from the engine
entity spawn() = #2;

// get length of string
float strlen(string s) = #3;

// draw a pixel on the screen at the specified coordinates
void drawpixel(vector pos, vector color) = #4;

// clear screen
void clearscreen(optional vector color) = #5;

// render screen
void drawscreen() = #6;

// draw rectangle
void drawrectangle(vector pos, vector size, vector color, float filled) = #7;

// draw text
void drawtext(vector pos, vector color, string text) = #8;

//
//
// globals
//
//

.vector origin;
.vector color;
.vector size;
.float score;

entity ball;
entity left;
entity right;

string hellostr = "hello world!";

//
//
// these are called from c
//
//

// called at startup
void setup()
{
	print("Initializing QuakeC module.\n");

	// spawn entities
	ball = spawn();
	left = spawn();
	right = spawn();
}

void shutdown()
{
	print("Shutting down QuakeC module.\n");
}

// called every physics frame
void update()
{

}

// called every draw frame
void draw(vector screensize)
{
	clearscreen([1, 0, 1]);

	drawrectangle([12, 12], [(strlen(hellostr) * 8) + 8, 16], [0, 0, 0], 1);
	drawtext([16, 16], [1, 1, 1], hellostr);

	drawscreen();
}
