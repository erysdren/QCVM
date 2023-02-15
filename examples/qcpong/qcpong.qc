
//
//
// exports
//
//

// draw a pixel on the screen at the specified coordinates
void drawpixel(vector pos, vector color) = #7;

// clear screen
void clearscreen(optional vector color) = #8;

// render screen
void drawscreen() = #9;

// draw rectangle
void drawrectangle(vector pos, vector size, vector color, float filled) = #10;

// draw text
void drawtext(vector pos, vector color, string text) = #11;

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

string str_debug;

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
	// clear screen
	clearscreen([1, 0, 1]);

	// create debug text for upper left corner
	str_debug = strcat("screen size: ", vtos(screensize));
	drawrectangle([12, 12], [(strlen(str_debug) * 8) + 8, 16], [0, 0, 0], 1);
	drawtext([16, 16], [1, 1, 1], str_debug);

	// draw screen
	drawscreen();
}
