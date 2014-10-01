#include <iostream>
#include <QtWidgets>
#include "balls.h"
#include <time.h>
#include <math.h>

using namespace std;	//std::cout is annoying

//GLOBALS
const int NUMBALLS = 5;
Ball* balls[NUMBALLS];

float absJC(float val)
{
	if (val < 0)
		return -val;
	else
		return val;
}

float squared(float val){
	return val*val;
}

float RandomNumber(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

//METHODS
//VECTOR
Vector::Vector(){}
Vector::Vector(float xVal, float yVal)
{
	x = xVal;
	y = yVal;
}

void Vector::Add(float tx, float ty){
	x += tx;
	y += ty;
}

void Vector::Multiply(float multiplier)
{
	x *= multiplier;
	y *= multiplier;
}

void Vector::Sub(Vector* vector)
{
	x -= vector->x;
	y -= vector->y;
}

float Vector::Length()
{
	return sqrt(((x)*(x)) + ((y)* (y)));
}

// BALL
Ball::Ball(){}

Ball::Ball(float px, float py, float vx, float vy, float r, int i)
{
	Vector* position = new Vector(px, py);
	Vector* velocity = new Vector(vx, vy);
	radius = r; id = i;
}

void Ball::printBall()
{
	//cout << "The ball " << id << " is at position " << position.x << "," << position.y << endl;
}
void Ball::move(float dt)
{
	position.Add(((velocity.x)*dt),((velocity.y)*dt));
}

//specify where to update screen here
QRectF Ball::boundingRect() const
{
	return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

//drawing goes here
void Ball::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);
}

// TIMERHANDLER
TimerHandler::TimerHandler(int t)
{
	QTimer *timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->start(t);
}

//update frame function - this is called repeatedly by the system
void TimerHandler::onTimer()
{
	//move code goes here

	// MOVE THE BALLS
	for (int p = 0; p < NUMBALLS; p++){
		balls[p]->move(1);
	}

	// CHECK FOR FAILURE POINTS
	for (int j = 0; j < NUMBALLS; j++) {
		// CHECK IF OUT OF BOUNDS
		if (balls[j]->position.x < 0 + balls[j]->radius){
			// Wall L @ position 0
			balls[j]->position.x = balls[j]->radius;
			balls[j]->velocity.x = balls[j]->velocity.x * -1;
		}
		if (balls[j]->position.x > 500 - balls[j]->radius){
			// Wall R @ position 500
			balls[j]->position.x = 500 - balls[j]->radius;
			balls[j]->velocity.x = balls[j]->velocity.x * -1;
		}
		if (balls[j]->position.y < 0 + balls[j]->radius){
			// Wall T @ position 0
			balls[j]->position.y = balls[j]->radius;
			balls[j]->velocity.y = balls[j]->velocity.y * -1;
		}
		if (balls[j]->position.y > 500 - balls[j]->radius){
			// Wall B @ position 500
			balls[j]->position.y = 500 - balls[j]->radius;
			balls[j]->velocity.y = balls[j]->velocity.y * -1;
		}


		// CHECK AGAINST ALL OTHER BALLS
		for (int i = j + 1; i < NUMBALLS; i++){
			/*
			float posx = absJC(balls[j] -> position.x - balls[i] -> position.x);
			float posy = absJC(balls[j] -> position.y - balls[i] -> position.y);

			if( (posx*posx)+(posy*posy) < ( ( balls[j] -> radius + balls[i] -> radius)*( balls[j] -> radius + balls[i] -> radius) ) ){
			//if overlap
			float overlapx = ( balls[j] -> radius + balls[i] -> radius) - absJC(balls[j] -> position.x - balls[i] -> position.x);
			float overlapy = ( balls[j] -> radius + balls[i] -> radius) - absJC(balls[j] -> position.y - balls[i] -> position.y);
			}
			*/

			balls[j]->position.Sub(&balls[i]->position);
			Vector* dist = &balls[j]->position;
			balls[j]->position.Add(balls[i]->position.x, balls[i]->position.y);


			if (dist->Length() == 0){ // IF VECTOR length ==0
				// COLLISSION
				// switch velocities, both x and y
				Vector tempv = balls[j]->velocity;
				balls[j]->velocity = balls[i]->velocity;
				balls[i]->velocity = tempv;
			}
			if (dist->Length() < 0){ //IF VECTOR LENGTH IS NEGSTIVE
				// OVERLAP
				dist->x /= 2;
				dist->y /= 2;

				//COMBINE THESE TWO IF STATEMENTS INTO A SINGLE VECTOR , NOT SEPARATE VALUES
				//move apart x
				if (balls[j]->position.x < balls[i]->position.x){
					balls[j]->position.Add(dist->x, dist->y);
					balls[i]->position.Sub(dist);
				}
				else{ //(balls[j]->position.x > balls[i]->position.x)
					//move j right, move i left
					balls[i]->position.Add(dist->x,dist->y);
					balls[j]->position.Sub(dist);
				}
			}
		}

	}

	// MOVE THE BALL ON THE SCREEN
	for (int d = 0; d < NUMBALLS; d++){
		balls[d]->setPos(balls[d]->position.x, balls[d]->position.y);
	}

}

/////////////////////////////////
//           M A I N           //
/////////////////////////////////

int main(int argc, char** argv)
{
	cout << sqrt(2) << endl;

	//jframe
	QApplication app(argc, argv);
	//panel
	QGraphicsScene scene;
	//set window size to 500,500
	scene.setSceneRect(0, 0, 500, 500);
	srand(time(NULL));

	balls[0] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 0);	//create object, put addr in balls[0]
	balls[1] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 1); 	//position x,y , velocity x,y ,radius,id
	balls[2] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 2); 	//position x,y , velocity x,y ,radius,id
	balls[3] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 3); 	//position x,y , velocity x,y ,radius,id
	balls[4] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 4); 	//position x,y , velocity x,y ,radius,id

	scene.addItem(balls[0]);
	scene.addItem(balls[1]);
	scene.addItem(balls[2]);
	scene.addItem(balls[3]);
	scene.addItem(balls[4]);

	balls[0]->setPos(balls[0]->position.x, balls[0]->position.y);
	balls[1]->setPos(balls[1]->position.x, balls[1]->position.y);
	balls[2]->setPos(balls[2]->position.x, balls[2]->position.y);
	balls[3]->setPos(balls[3]->position.x, balls[3]->position.y);
	balls[4]->setPos(balls[4]->position.x, balls[4]->position.y);
	QGraphicsView view(&scene);
	view.setWindowTitle("Balls JC");
	view.resize(500, 500);
	//setVisible true
	view.show();

	//1000 milliseconds / frames-per-second
	TimerHandler timer(1000 / 66);

	//window is created here
	return app.exec();
}