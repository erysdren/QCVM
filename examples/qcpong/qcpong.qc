
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
.vector mins;
.vector maxs;
.float score;

entity ball;
entity left;
entity right;

float first_draw;

#define BALL_SPEED 4
#define PADDLE_SPEED 2

struct
{
	float moveup;
	float movedown;
} inputs;

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

	// set ball values
	ball.mins = [8, 8];
	ball.maxs = [8, 8];
	ball.color = [1, 1, 1];

	// set left paddle values
	left.mins = [8, 32];
	left.maxs = [8, 32];
	left.color = [1, 1, 1];

	// set right paddle values
	right.mins = [8, 32];
	right.maxs = [8, 32];
	right.color = [1, 1, 1];

	// hack
	first_draw = TRUE;
}

void shutdown()
{
	print("Shutting down QuakeC module.\n");
}

// called every physics frame
void update()
{

}

// called whenever a keyboard input happens
void input(float key, float pressed)
{
	switch (key)
	{
		case SC_UP:
		case SC_W:
			inputs.moveup = pressed ? TRUE : FALSE;
			break;

		case SC_DOWN:
		case SC_S:
			inputs.movedown = pressed ? TRUE : FALSE;
			break;
		
		default:
			break;
	}
}

// called every draw frame
void draw(vector screensize)
{
	// clear screen
	clearscreen([0.1, 0.1, 0.1]);

	// hack
	if (first_draw)
	{
		left.origin = [32, screensize[1] / 2];
		right.origin = [screensize[0] - 32, screensize[1] / 2];
		ball.origin = screensize / 2;
		first_draw = FALSE;
	}

	// process inputs
	if (inputs.moveup) left.origin[1] -= BALL_SPEED;
	if (inputs.movedown) left.origin[1] += BALL_SPEED;

	// draw debug text
	drawtext([16, 16], [1, 1, 1], vtos(left.origin));

	// draw ball & paddles
	drawrectangle(ball.origin - ball.mins, ball.maxs + ball.mins, ball.color, TRUE);
	drawrectangle(left.origin - left.mins, left.maxs + left.mins, left.color, TRUE);
	drawrectangle(right.origin - right.mins, right.maxs + right.mins, right.color, TRUE);

	// draw screen
	drawscreen();
}
