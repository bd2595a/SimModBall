#include <iostream>
#include <QtWidgets>
#include "balls.h"
#include <time.h>
#include <math.h>

using namespace std;	//std::cout is annoying

//GLOBALS
const int NUMBALLS = 10;
Ball* balls[NUMBALLS];
float R = 1.0;

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
void Vector::Add(Vector* vector){
	x += vector->x;
	y -= vector->y;
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

float Vector::DotProduct(Vector* vector2)
{
	return ((x*vector2->x) + (y*vector2->y));
}

float Vector::Length()
{
	return sqrt(((x)*(x)) + ((y)* (y)));
}

Vector* Vector::normalize(){
	// look into doing this in a void way
	Vector* r = new Vector(0, 0);
	r->x = x / sqrt(x*x + y*y);
	r->y = y / sqrt(x*x + y*y);
	return r;
}

// BALL
Ball::Ball(){}

Ball::Ball(float px, float py, float vx, float vy, float r, float m, int i)
{
	position = new Vector(px, py);
	velocity = new Vector(vx, vy);
	radius = r; id = i; mass = m;
}

void Ball::printBall()
{
	//cout << "The ball " << id << " is at position " << position->x << "," << position->y << endl;
}
void Ball::move(float dt)
{
	position->Add(((velocity->x)*dt), ((velocity->y)*dt));
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
		if (balls[j]->position->x < 0 + balls[j]->radius){
			// Wall L @ position 0
			balls[j]->position->x = balls[j]->radius;
			balls[j]->velocity->x = balls[j]->velocity->x * -1;
		}
		if (balls[j]->position->x > 500 - balls[j]->radius){
			// Wall R @ position 500
			balls[j]->position->x = 500 - balls[j]->radius;
			balls[j]->velocity->x = balls[j]->velocity->x * -1;
		}
		if (balls[j]->position->y < 0 + balls[j]->radius){
			// Wall T @ position 0c
			balls[j]->position->y = balls[j]->radius;
			balls[j]->velocity->y = balls[j]->velocity->y * -1;
		}
		if (balls[j]->position->y > 500 - balls[j]->radius){
			// Wall B @ position 500
			balls[j]->position->y = 500 - balls[j]->radius;
			balls[j]->velocity->y = balls[j]->velocity->y * -1;
		}



		// CHECK AGAINST ALL OTHER BALLS
		for (int i = j + 1; i < NUMBALLS; i++){
			Vector* dist = new Vector(balls[j]->position->x - balls[i]->position->x, balls[j]->position->y - balls[i]->position->y);
			/*
			//X overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->x / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			// Y overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->y / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			EACH ball should be moved [ X overlap / 2 , Y overlap / 2] away from the other
			*/

			// COLLISION
			// collide
			bool bounce = false;
			
			if (dist->Length() < balls[j]->radius + balls[i]->radius){
				float xoverlap = ( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->x / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) );
				float yoverlap = ( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->y / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) );
				float mi,mj;
				if (balls[i]->mass != balls[j]->mass){
					// move them apart proportional to mass
					mj=2;
					mi=2;
				}
				else{
					// move them apart 1/2 each
					mj=2;
					mi=2;
				}

				if (balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y )
				{	// Jx > IX && JY > IY
					balls[j]->position->Add(xoverlap/mj,yoverlap/mj);
					balls[i]->position->Add(xoverlap/-mi,yoverlap/-mi);
				}
				else if(balls[j]->position->x > balls[i]->position->x && balls[j]->position->y < balls[i]->position->y )
				{	// JX > IX && JY < IY
					balls[j]->position->Add(xoverlap/mj,yoverlap/-mj);
					balls[i]->position->Add(xoverlap/-mi,yoverlap/mi);
				}
				else if(balls[j]->position->x < balls[i]->position->x && balls[j]->position->y > balls[i]->position->y )
				{	// JX < IX && JY > IY
					balls[j]->position->Add(xoverlap/-mj,yoverlap/mj);
					balls[i]->position->Add(xoverlap/mi,yoverlap/-mi);
				}
				else //if(balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y ){
				{	// JX > IX && JY > IY
					balls[j]->position->Add(xoverlap/-mj,yoverlap/-mj);
					balls[i]->position->Add(xoverlap/mi,yoverlap/mi);
				}

				bounce = true;
			}
			
			if (dist->Length() <= balls[j]->radius + balls[i]->radius  || bounce){	
				Vector* normCol = dist->normalize();
				//adjust velocity
				Vector* Vdiff = new Vector(balls[j]->velocity->x, balls[j]->velocity->y);
				Vdiff->Sub(balls[i]->velocity);
				float vadjust = Vdiff->DotProduct(normCol);
				//delete Vdiff;
				//delete velocitySub;

				//determine how much each ball's velocity should change, according to the amss
				float vadjust1 = (1 + R) * vadjust *((1 / balls[j]->mass) / (1 / balls[j]->mass + 1 / balls[i]->mass));
				float vadjust2 = (1 + R) * vadjust *((1 / balls[i]->mass) / (1 / balls[j]->mass + 1 / balls[i]->mass));

				//adjust each velocity vector using the collion angle scaled properly
				Vector* xColAdjust1 = new Vector(normCol->x, normCol->y);
				Vector* xColAdjust2 = new Vector(normCol->x, normCol->y);
				xColAdjust1->Multiply(vadjust1);
				xColAdjust2->Multiply(vadjust2);

				balls[j]->velocity->Sub(xColAdjust1);
				balls[i]->velocity->Add(xColAdjust2);
				//delete xColAdjust1;
				//delete xColAdjust2;
			}
		}
	}

	// MOVE THE BALL ON THE SCREEN
	for (int d = 0; d < NUMBALLS; d++){
		balls[d]->setPos(balls[d]->position->x, balls[d]->position->y);
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

	for (int i = 0; i < NUMBALLS; i++){
		balls[i] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 10, i);
		scene.addItem(balls[i]);
		balls[i]->setPos(balls[i]->position->x, balls[i]->position->y);
	}

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