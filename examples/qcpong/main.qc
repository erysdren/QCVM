
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
void renderscreen() = #5;

//
//
// globals
//
//

.vector origin;
.vector color;
.float width;
.float height;

entity ball;
entity left_paddle;
entity right_paddle;

float left_score;
float right_score;

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
	left_paddle = spawn();
	right_paddle = spawn();

	left_score = 0;
	right_score = 0;

	// draw a pixel
	drawpixel([32, 32], [1, 1, 1]);
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
	g += 0.001;
	b += 0.001;

	if (r > 1) r = 0;
	if (g > 1) g = 0;
	if (b > 1) b = 0;

	drawpixel([64, 64], [r, g, b]);

	renderscreen();
}
