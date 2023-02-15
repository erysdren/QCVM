
//
//
// exports
//
//

// print string to stdoout
void print(string s, ...) = #1;

// acquire entity handle from the engine
entity spawn() = #2;

// draw a pixel on the screen at the specified coordinates
void drawpixel(vector pos, vector color) = #3;

// clear screen
void clearscreen() = #4;

// render screen
void drawscreen() = #5;

// draw text on screen
void drawtext(vector pos, vector color, string text) = #6;

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

float r;
float g;
float b;

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
	clearscreen();

	r += 0.001;
	g += 0.01;
	b += 0.001;

	if (r > 1) r = 0;
	if (g > 1) g = 0;
	if (b > 1) b = 0;

	drawpixel([64, 64], [r, g, b]);

	drawscreen();
}
