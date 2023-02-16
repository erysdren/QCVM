
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

float screenshot_time;

string text_score_label1 = "SCORE";
string text_score_label2;
string text_score;
string text_leftwin = "THE WINNER IS LEFT!";
string text_rightwin = "THE WINNER IS RIGHT!";
string text_exit = "Press escape to exit.";

float developer;
float gameover;
float first_draw;

#define BALL_SPEED 4
#define PADDLE_SPEED 4
#define WIN_SCORE 8

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
// our functions
//
//

float collides2D(entity a, entity b)
{
	return a.origin[0] - a.mins[0] < b.origin[0] + b.maxs[0] &&
			a.origin[0] + a.maxs[0] > b.origin[0] - b.mins[0] &&
			a.origin[1] - a.mins[1] < b.origin[1] + b.maxs[1] &&
			a.origin[1] + a.maxs[1] > b.origin[1] - b.mins[1];
}

float collides3D(entity a, entity b)
{
	return a.origin[0] - a.mins[0] < b.origin[0] + b.maxs[0] &&
			a.origin[0] + a.maxs[0] > b.origin[0] - b.mins[0] &&
			a.origin[1] - a.mins[1] < b.origin[1] + b.maxs[1] &&
			a.origin[1] + a.maxs[1] > b.origin[1] - b.mins[1] &&
			a.origin[2] - a.mins[2] < b.origin[2] + b.maxs[2] &&
			a.origin[2] + a.maxs[2] > b.origin[2] - b.mins[2];
}

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

		case SC_D:
			if (pressed) developer = developer ? FALSE : TRUE;
			break;

		case SC_ESCAPE:
			if (pressed) exit();
			break;
		
		case SC_F12:
			if (pressed && time > screenshot_time)
			{
				screenshot("screenshot.png");
				print("Took a screenshot.\n");
				screenshot_time = time + 2;
			}
			break;

		default:
			break;
	}
}

void draw_game(vector screensize)
{
	// process inputs
	if (gamestate.moveup) left.origin[1] -= PADDLE_SPEED;
	if (gamestate.movedown) left.origin[1] += PADDLE_SPEED;

	// process ball < - > screen collision
	if (ball.origin[0] - ball.mins[0] <= 0)
	{
		// hitting left
		if (gamestate.ball_dir == BALL_UPLEFT) gamestate.ball_dir = BALL_UPRIGHT;
		else if (gamestate.ball_dir == BALL_DOWNLEFT) gamestate.ball_dir = BALL_DOWNRIGHT;
		ball.origin = screensize / 2;
		right.score += 1;
	}
	if (ball.origin[1] - ball.mins[1] <= 0)
	{
		// hitting top
		if (gamestate.ball_dir == BALL_UPLEFT) gamestate.ball_dir = BALL_DOWNLEFT;
		else if (gamestate.ball_dir == BALL_UPRIGHT) gamestate.ball_dir = BALL_DOWNRIGHT;
	}
	if (ball.origin[0] + ball.maxs[0] >= screensize[0])
	{
		// hitting right
		if (gamestate.ball_dir == BALL_UPRIGHT) gamestate.ball_dir = BALL_UPLEFT;
		else if (gamestate.ball_dir == BALL_DOWNRIGHT) gamestate.ball_dir = BALL_DOWNLEFT;
		ball.origin = screensize / 2;
		left.score += 1;
	}
	if (ball.origin[1] + ball.maxs[1] >= screensize[1])
	{
		// hitting bottom
		if (gamestate.ball_dir == BALL_DOWNLEFT) gamestate.ball_dir = BALL_UPLEFT;
		else if (gamestate.ball_dir == BALL_DOWNRIGHT) gamestate.ball_dir = BALL_UPRIGHT;
	}

	// process ball < - > paddle collision
	if (ball.origin[0] < screensize[0] / 2)
	{
		// left paddle
		if (collides2D(ball, left) == TRUE)
		{
			if (gamestate.ball_dir == BALL_UPLEFT) gamestate.ball_dir = BALL_UPRIGHT;
			else if (gamestate.ball_dir == BALL_DOWNLEFT) gamestate.ball_dir = BALL_DOWNRIGHT;
		}
	}
	else
	{
		// right paddle
		if (collides2D(ball, right) == TRUE)
		{
			if (gamestate.ball_dir == BALL_UPRIGHT) gamestate.ball_dir = BALL_UPLEFT;
			else if (gamestate.ball_dir == BALL_DOWNRIGHT) gamestate.ball_dir = BALL_DOWNLEFT;
		}
	}

	// right paddle ai
	if (ball.origin[1] > right.origin[1]) right.origin[1] += PADDLE_SPEED / 2;
	if (ball.origin[1] < right.origin[1]) right.origin[1] -= PADDLE_SPEED / 2;

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

	// draw ball & paddles
	drawrectangle(ball.origin - ball.mins, ball.maxs + ball.mins, ball.color, TRUE);
	drawrectangle(left.origin - left.mins, left.maxs + left.mins, left.color, TRUE);
	drawrectangle(right.origin - right.mins, right.maxs + right.mins, right.color, TRUE);

	// draw score text
	text_score = strcat(ftos(left.score), " | ", ftos(right.score));
	text_score_label2 = strcat("PLAYING TO ", ftos(WIN_SCORE));
	drawtext([(screensize[0] / 2) - ((strlen(text_score_label1) / 2) * 8), 16], [1, 1, 1], text_score_label1);
	drawtext([(screensize[0] / 2) - ((strlen(text_score) / 2) * 8), 32], [1, 1, 1], text_score);
	drawtext([(screensize[0] / 2) - ((strlen(text_score_label2) / 2) * 8), screensize[1] - 16], [1, 1, 1], text_score_label2);

	// draw exit text
	drawtext([(screensize[0] / 2) - ((strlen(text_exit) / 2) * 8), screensize[1] - 8], [1, 1, 1], text_exit);
}

void draw_gameover(vector screensize)
{
	// who won
	if (left.score >= WIN_SCORE)
		drawtext([(screensize[0] / 2) - ((strlen(text_leftwin) / 2) * 8), (screensize[1] / 2) - 4], [1, 1, 1], text_leftwin);
	else if (right.score >= WIN_SCORE)
		drawtext([(screensize[0] / 2) - ((strlen(text_rightwin) / 2) * 8), (screensize[1] / 2) - 4], [1, 1, 1], text_rightwin);

	// draw exit text
	drawtext([(screensize[0] / 2) - ((strlen(text_exit) / 2) * 8), (screensize[1] / 2) + 16], [1, 1, 1], text_exit);
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

	// draw game or gameover
	if (gameover)
		draw_gameover(screensize);
	else
		draw_game(screensize);

	// developer text
	if (developer)
	{
		drawtext([16, 16], [1, 1, 1], strcat("time: ", ftos(time)));
		drawtext([16, 24], [1, 1, 1], strcat("fps: ", ftos(fps)));
	}

	// check if someone has won
	if (left.score >= WIN_SCORE || right.score >= WIN_SCORE)
	{
		gameover = TRUE;
	}

	// draw screen
	drawscreen();
}
