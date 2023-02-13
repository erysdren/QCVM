
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
void pixel(vector pos, vector color) = #3;

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

}
