
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

#define BALL_SPEED 6
#define PADDLE_SPEED 4

enum
{
	BALL_UPLEFT,
	BALL_UPRIGHT,
	BALL_DOWNLEFT,
	BALL_DOWNRIGHT
};

struct
{
	float moveup;
	float movedown;
	float ball_dir;
} gamestate;

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
			gamestate.moveup = pressed ? TRUE : FALSE;
			break;

		case SC_DOWN:
		case SC_S:
			gamestate.movedown = pressed ? TRUE : FALSE;
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
	if (gamestate.moveup) left.origin[1] -= PADDLE_SPEED;
	if (gamestate.movedown) left.origin[1] += PADDLE_SPEED;

	// process ball collision
	if (ball.origin[0] - ball.mins[0] <= 0)
	{
		// hitting left
		if (gamestate.ball_dir == BALL_UPLEFT) gamestate.ball_dir = BALL_UPRIGHT;
		if (gamestate.ball_dir == BALL_DOWNLEFT) gamestate.ball_dir = BALL_DOWNRIGHT;
	}
	if (ball.origin[1] - ball.mins[1] <= 0)
	{
		// hitting top
		if (gamestate.ball_dir == BALL_UPLEFT) gamestate.ball_dir = BALL_DOWNLEFT;
		if (gamestate.ball_dir == BALL_UPRIGHT) gamestate.ball_dir = BALL_DOWNRIGHT;
	}
	if (ball.origin[0] + ball.maxs[0] >= screensize[0])
	{
		// hitting right
		if (gamestate.ball_dir == BALL_UPRIGHT) gamestate.ball_dir = BALL_UPLEFT;
		if (gamestate.ball_dir == BALL_DOWNRIGHT) gamestate.ball_dir = BALL_DOWNLEFT;
	}
	if (ball.origin[1] + ball.maxs[1] >= screensize[1])
	{
		// hitting bottom
		if (gamestate.ball_dir == BALL_DOWNLEFT) gamestate.ball_dir = BALL_UPLEFT;
		if (gamestate.ball_dir == BALL_DOWNRIGHT) gamestate.ball_dir = BALL_UPRIGHT;
	}

	// move ball
	switch (gamestate.ball_dir)
	{
		case BALL_UPLEFT:
			ball.origin[0] -= BALL_SPEED;
			ball.origin[1] -= BALL_SPEED;
			break;

		case BALL_UPRIGHT:
			ball.origin[0] += BALL_SPEED;
			ball.origin[1] -= BALL_SPEED;
			break;

		case BALL_DOWNLEFT:
			ball.origin[0] -= BALL_SPEED;
			ball.origin[1] += BALL_SPEED;
			break;

		case BALL_DOWNRIGHT:
			ball.origin[0] += BALL_SPEED;
			ball.origin[1] += BALL_SPEED;
			break;
		
		default:
			break;
	}

	// draw debug text
	drawtext([16, 16], [1, 1, 1], strcat("ball.origin ", vtos(ball.origin)));

	// draw ball & paddles
	drawrectangle(ball.origin - ball.mins, ball.maxs + ball.mins, ball.color, TRUE);
	drawrectangle(left.origin - left.mins, left.maxs + left.mins, left.color, TRUE);
	drawrectangle(right.origin - right.mins, right.maxs + right.mins, right.color, TRUE);

	// draw screen
	drawscreen();
}
