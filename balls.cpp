#include <iostream>
#include <QtWidgets>
#include <QKeyEvent>
#include "balls.h"
#include <time.h>
#include <math.h>

using namespace std;	//std::cout is annoying

//GLOBALS
const int NUMBALLS = 10;
int ball1ID = -1;
int linkID = 0;
Ball* balls[NUMBALLS];
float R = 1.0;
QGraphicsScene* canvas;//canvas
BallView* view;	//the window

bool timestop = false;

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
	y += vector->y;
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
void Vector::Multiply(Vector* vector){
	x *= vector->x;
	y *= vector->y;
}

void Vector::Sub(Vector* vector)
{
	x -= vector->x;
	y -= vector->y;
}

float Vector::DotProduct(Vector* vector2)
{
	return (x*vector2->x + y*vector2->y);
}

float Vector::Length()
{
	return sqrt(x*x + y* y);
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
	position->Add(velocity->x*dt, velocity->y*dt);
}

//LINKages
Link::Link(){}

Link::Link(int b1num, int b2num){
	ball1 = b1num;
	ball2 = b2num;
	Vector* temp = balls[ball1]->position;
	temp->Sub(balls[ball2]->position);
	dist = temp->Length();
	id = linkID;
	linkID++;
}

void Link::contract(){
	// V2 - V1 = axis from V1 to v2
	// V1 - V2 = axis from V2 to V1
	//find vector between the ball positions
	//axis = ball 1 to ball 0
	Vector* axis10 = balls[ball1]->position;
	axis10->Sub(balls[ball2]->position);
	//axis = ball 0 to ball 1
	Vector* axis01 = balls[ball1]->position;
	axis01->Sub(balls[ball2]->position);

	//if length of that vector is > dist
	float lendif = axis10->Length() - dist;
	if(lendif != 0) // if it's <0 or >0 ... ... ... SHOULD BE THE SAME FOR BOTH 
	{	// move each ball along the vector proportional to their masses

		//find the vector axis between the ball to each other
		Vector* norm10 = axis10;
		norm10= norm10->normalize();
		Vector* norm01 = axis01;
		norm01= norm01->normalize();
		//Find proportion based on mass
		float b0prop = balls[ball[0]]->mass / ( balls[ball[0]]->mass + balls[ball[1]]->mass);
		float b1prop = balls[ball[1]]->mass / ( balls[ball[0]]->mass + balls[ball[1]]->mass);

		//Find proportional move distance
		float b0distmove = lendif * b0prop; 
		float b1distmove = lendif * b1prop;
		norm01->Multiply(b0distmove);
		norm10->Multiply(b1distmove);

		//Move the balls by the normal axis times move distance
		balls[ball[0]]->position->Add( norm01 );
		balls[ball[1]]->position->Add( norm10 );
	}
	

}
// KEYPRESS


void BallView::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Space)
	{
	}
}

void BallView::keyReleaseEvent(QKeyEvent *event)//putting this code in release because space can be pressed and hold, whilst it can only be released once

{
	if (event->key() == Qt::Key_Space)
	{
		timestop = timestop == true? false: true;//this says "is timestop == true? then set it to false. else, set it to true."
	}
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

//specify where to update screen here
QRectF Link::boundingRect() const
{
	return QRectF(0, 0, 500, 500);
}


//drawing goes here
void Link::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->drawLine(balls[ball[0]]->position->x, balls[ball[0]]->position->y, balls[ball[1]]->position->x, balls[ball[1]]->position->y);
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
	if(timestop){
		return;
	}

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
			Vector* dist = new Vector(absJC(balls[j]->position->x - balls[i]->position->x), absJC(balls[j]->position->y - balls[i]->position->y));
			bool bounce = false;
			/*
			//X overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->x / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			// Y overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->y / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			EACH ball should be moved [ X overlap / 2 , Y overlap / 2] away from the other
			*/

			// COLLISION
			// collide
			if (dist->Length() < balls[j]->radius + balls[i]->radius){
				//OVERLAP CODE
				//TODO: TRANSLATE INTO VECTORS.
				float xoverlap = ((balls[i]->radius + balls[j]->radius) - sqrt((dist->x * dist->x) + (dist->y * dist->y))) * (dist->x / sqrt((dist->x * dist->x) + (dist->y * dist->y)));
				float yoverlap = ((balls[i]->radius + balls[j]->radius) - sqrt((dist->x * dist->x) + (dist->y * dist->y))) * (dist->y / sqrt((dist->x * dist->x) + (dist->y * dist->y)));
				float mj, mi;
				if (balls[j]->mass != balls[i]->mass){
					mj = balls[j]->mass / (balls[j]->mass + balls[i]->mass);
					mi = balls[i]->mass / (balls[j]->mass + balls[i]->mass);
				}
				else{
					mj = 0.5;
					mi = 0.5;
				}

				if (balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y)
				{	// Jx > IX && JY > IY
					balls[j]->position->Add(xoverlap * mj, yoverlap * mj);
					balls[i]->position->Add(xoverlap * -mi, yoverlap * -mi);
				}
				else if (balls[j]->position->x > balls[i]->position->x && balls[j]->position->y < balls[i]->position->y)
				{	// JX > IX && JY < IY
					balls[j]->position->Add(xoverlap * mj, yoverlap * -mj);
					balls[i]->position->Add(xoverlap * mi, yoverlap * -mi);
				}
				else if (balls[j]->position->x < balls[i]->position->x && balls[j]->position->y > balls[i]->position->y)
				{	// JX < IX && JY > IY
					balls[j]->position->Add(xoverlap * -mj, yoverlap * mj);
					balls[i]->position->Add(xoverlap * mi, yoverlap * -mi);
				}
				else //if(balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y ){
				{	// JX > IX && JY > IY
					balls[j]->position->Add(xoverlap * -mj, yoverlap *-mj);
					balls[i]->position->Add(xoverlap * mi, yoverlap *mi);
				}

				bounce = true;
			}
			//*
			if (dist->Length() <= balls[j]->radius + balls[i]->radius || bounce){
				Vector* normCol = dist->normalize();
				//adjust velocity
				Vector* Vdiff = new Vector(balls[j]->velocity->x, balls[j]->velocity->y);
				Vdiff->Sub(balls[i]->velocity);
				float vadjust = Vdiff->DotProduct(normCol);
				delete Vdiff;

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
				delete xColAdjust1;
				delete xColAdjust2;
			}
			//*/
		}
	}

	// MOVE THE BALL ON THE SCREEN
	for (int d = 0; d < NUMBALLS; d++){
		balls[d]->setPos(balls[d]->position->x, balls[d]->position->y);
	}

}

//constructor for the window.  you can leave it empty if you want.
BallView::BallView(QGraphicsScene *scene, QWidget* parent) :QGraphicsView(scene, parent)
{
}

//this is called when the mouse is pressed.  use it if you want.
void BallView::mousePressEvent(QMouseEvent *event)
{
	//TODO: on mouse pressed...
	if (event->button() == Qt::LeftButton)
	{
		if (ball1ID == -1)//if we haven't already selected a ball
		{
			int counter = 0;//THIS STATEMENT NEEDS TO BE HERE
			for (counter = 0; counter < NUMBALLS; counter++)//see if we actually selected a ball
			{
				if (event->x() >= balls[counter]->position->x - balls[counter]->radius && event->x() <= balls[counter]->position->x + balls[counter]->radius && event->y() >= balls[counter]->position->y - balls[counter]->radius && event->y() <= balls[counter]->position->y + balls[counter]->radius)
				{
					ball1ID = counter;
					break;
				}
			}
		}
		else if (ball1ID != -1)//we've already selected one ball, so now we find the position of the next ball we clicked
		{
			for (int i = 0; i < NUMBALLS; i++)//see if we actually selected a ball
			{
				if (event->x() >= balls[i]->position->x - balls[i]->radius && event->x() <= balls[i]->position->x + balls[i]->radius && event->y() >= balls[i]->position->y - balls[i]->radius && event->y() <= balls[i]->position->y + balls[i]->radius)
				{
					Link *newLink = new Link(ball1ID, i);
					canvas->addItem(newLink);
					ball1ID = -1;
					break;
				}
			}
		}
	}
}

//this is called when the mouse is released.  use it if you want.
void BallView::mouseReleaseEvent(QMouseEvent *event)
{
	//TODO: on mouse released...
}

//this is called when the mouse is doubleclicked.
//currently I have it just creating a new ball at that point
//you can remove that and change this to whatever you like
void BallView::mouseDoubleClickEvent(QMouseEvent *event)
{
	//TODO: modify this if you want to...
}

/////////////////////////////////
//           M A I N           //
/////////////////////////////////

int main(int argc, char** argv)
{

	//jframe
	QApplication app(argc, argv);
	//panel
	//set window size to 500,500
	canvas = new QGraphicsScene();
	canvas->setSceneRect(0, 0, 500, 500);
	srand(time(NULL));

	for (int i = 0; i < NUMBALLS; i++){
		balls[i] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 10, i);
		canvas->addItem(balls[i]);
		balls[i]->setPos(balls[i]->position->x, balls[i]->position->y);
	}

	view = new BallView(canvas);
	view->setFocusPolicy(Qt::ClickFocus);
	view->setWindowTitle("Balls JC & Bree");
	view->resize(500, 500);
	view->setMouseTracking(true);
	//setVisible true
	view->show();





	//1000 milliseconds / frames-per-second
	TimerHandler timer(1000 / 66);

	//window is created here
	return app.exec();
}

