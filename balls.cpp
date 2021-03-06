#include <QtWidgets>
#include <QKeyEvent>
#include "balls.h"
#include <time.h>
#include <math.h>

#define PI 3.14159265

using namespace std;	//std::cout is annoying

//GLOBALS
const int NUMBALLS = 10;
int ball1ID = -1;
int linkID = 0;
Ball* balls[NUMBALLS];
Link* links[100];
float R = 1.0;
QGraphicsScene* canvas;//canvas
BallView* view;	//the window
float GRAVITY = 0.5;

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

void Vector::Divide(Vector* vector){
	x = x / vector->x;
	y = y / vector->y;
}

void Vector::Sub(Vector* vector)
{
	x -= vector->x;
	y -= vector->y;
}

float Vector::DotProduct(Vector* vector2)
{
	return (x * vector2->x + y * vector2->y);
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

void Vector::makePerpindicular(){
	//y is -1
	float tempx = x;
	x = y * -1;
	y = tempx;
}

void Vector::makePerpindicular2(){
	//x is -1
	float tempy = y;
	y = x * -1;
	x = tempy;
}

// BALL
Ball::Ball(){}

Ball::Ball(float px, float py, float vx, float vy, float r, float m, int i, bool stationary2)
{
	position = new Vector(px, py);
	velocity = new Vector(vx, vy);
	radius = r; id = i; mass = m;
	stationary = stationary2;
	if (stationary)
	{
		stationX = px;
		stationY = py;
	}
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
	Vector* temp = new Vector(balls[ball1]->position->x, balls[ball1]->position->y);
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
	Vector *axis10 = new Vector(balls[ball1]->position->x, balls[ball1]->position->y);
	axis10->Sub(balls[ball2]->position);
	//axis = ball 0 to ball 1
	Vector *axis01 = new Vector(balls[ball2]->position->x, balls[ball2]->position->y);
	axis01->Sub(balls[ball1]->position);

	//MOVE THE BALL

	//if length of that vector is > dist
	float lendif = axis10->Length() - dist;
	if (lendif != 0) // if it's <0 or >0 ... ... ... SHOULD BE THE SAME FOR BOTH 
	{	// move each ball along the vector proportional to their masses

		//find the vector axis between the ball to each other
		Vector* norm10 = new Vector(axis10->x, axis10->y);
		norm10 = norm10->normalize();
		Vector* norm01 = new Vector(axis01->x, axis01->y);
		norm01 = norm01->normalize();
		//Find proportion based on mass
		float b0prop = balls[ball1]->mass / (balls[ball2]->mass + balls[ball1]->mass);
		float b1prop = balls[ball2]->mass / (balls[ball1]->mass + balls[ball2]->mass);

		//Find proportional move distance
		float b0distmove = lendif * b0prop;
		float b1distmove = lendif * b1prop;
		norm01->Multiply(b0distmove);
		norm10->Multiply(b1distmove);

		//Move the balls by the normal axis times move distance
		if (balls[ball1]->stationary)
		{
			balls[ball1]->position->x = balls[ball1]->stationX;
			balls[ball1]->position->y = balls[ball1]->stationY;
		}
		else
			balls[ball1]->position->Add(norm01);
		if (balls[ball2]->stationary)
		{
			balls[ball2]->position->x = balls[ball2]->stationX;
			balls[ball2]->position->y = balls[ball2]->stationY;
		}
		else
			balls[ball2]->position->Add(norm10);
	}

	if(balls[ball1]->velocity->x != 0 || balls[ball1]->velocity->y != 0){
		//how much of a vector lies along it is the dot product

		//Vvel*Vperp = Vfinalvel * Mperp
		//finalmag= Vvel * Vperp / Mperp



		Vector* perplink = new Vector(axis01->x,axis01->y);
		qDebug() << "perplink " << perplink->x << " " << perplink->y;
		perplink->makePerpindicular();
		qDebug() << "perpindi " << perplink->x << " " << perplink->y;
		float howmuch = perplink->DotProduct(balls[ball1]->velocity);
		howmuch = howmuch / perplink->Length();
		qDebug() << "howmuch " << howmuch;
		Vector* newVelocity = perplink->normalize();
		qDebug() << "norm " << newVelocity->x<<" "<< newVelocity->y;
		newVelocity->Multiply(howmuch);
		qDebug() << "mult " << newVelocity->x<<" "<< newVelocity->y;
		balls[ball1]->velocity = newVelocity;





		// // ball1 is the ball, ball2 is the structure
		// // need to compare velocity vector to perpendicular link vector
		// // axis01 is from ball to support structure
		// qDebug() << "velocity " << balls[ball1]->velocity->x << " " << balls[ball1]->velocity->y;
		// qDebug() << "axis " << axis01->x << " " << axis01->y;
		// //find proportion of velocity vector 
		// //v2 = a2 + p2
		// //v2-a2 = p2
		// //sqrt(v2-a2) = p magnitude
		// //sqrt ( vel mag ^2 - axis mag ^2 )
		// float velo = balls[ball1]->velocity->Length();
		// float linkaxis = axis01->Length();
		// qDebug() << "V" << velo;
		// qDebug() << "L" << linkaxis;
		// double pmag = sqrt( absJC( (velo * velo) - (linkaxis * linkaxis) ) );
		// qDebug() << "PMAG " << pmag;
		// //proportion out of 100 that
		// //find perp vector
		// //y,-x is needed make perp2 off of axis
		// qDebug() << "axis     "<<axis01->x<<" "<<axis01->y;
		// axis01->makePerpindicular2();
		// qDebug() << "perpaxis "<<axis01->x<<" "<<axis01->y;
		// //normalize
		// axis01= axis01->normalize();
		// qDebug() << "norpaxis "<<axis01->x<<" "<<axis01->y;
		// //multiply by p mag
		// axis01->Multiply(pmag);
		// qDebug() << "final  V "<<axis01->x<<" "<<axis01->y;
		// qDebug();
		// //apply to velocity
		// balls[ball1]->velocity->x = axis01->x;
		// balls[ball1]->velocity->y = axis01->y;




		/*
		qDebug() << "velocity " << balls[ball1]->velocity->x << " " << balls[ball1]->velocity->y;
		//CHANGE THE BALL'S VELOCITIES
		//find angle between link and ball2 velocity
		Vector* tang = new Vector(balls[ball1]->position->x, balls[ball1]->position->y);
		tang->Sub(balls[ball2]->position);
		tang->makePerpindicular();

		Vector* vel1norm = new Vector(balls[ball1]->velocity->x,balls[ball1]->velocity->y);
		vel1norm->normalize();
		axis10->normalize();
		//get angle between gravity and link to ball2

		int b1vellinkang = atan2(vel1norm->y - axis10->y,vel1norm->x - axis10 -> x ) * 180 /PI;
		b1vellinkang = absJC(b1vellinkang+90);
		qDebug() << "angle " << b1vellinkang;
		qDebug() << " proportion "<< (float)b1vellinkang/ 90.0 ;

		axis10->makePerpindicular();
		axis10->Multiply( (float)b1vellinkang / 90.0 );

		balls[ball1]->velocity = axis10;

		*/

		/*
		qDebug() << "velocity " << balls[ball1]->velocity->x << " " << balls[ball1]->velocity->y;
		double dot = vel1norm->DotProduct(axis10);
		qDebug() << "dot "<< dot;
		double b1vellinkang = acos(dot) * 180.0 / PI;
		qDebug() << "angle " << b1vellinkang;
		
		//find the magnitude of the ajacent side between gravity and 
		float newvel= cos(90-b1vellinkang) * vel1->Length();
		qDebug() << "newvel " << newvel;
		//magnitude of velocity tangent to circle

		axis10->makePerpindicular();

		axis10->Multiply(newvel);
		qDebug() << "newvector" << axis10->x << " " << axis10->y;
		balls[ball1]->velocity = axis10;
		qDebug() << "changed velocity ";
		*/
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
		timestop = timestop == true ? false : true;//this says "is timestop == true? then set it to false. else, set it to true."
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
	painter->drawLine(balls[ball1]->position->x, balls[ball1]->position->y, balls[ball2]->position->x, balls[ball2]->position->y);
	// Vector* veltemp = new Vector();
	// veltemp = balls[ball1]->velocity;
	// veltemp->makePerpindicular();
	// veltemp->normalize();
	// veltemp->Multiply(15);
	// painter->setPen(QPen(Qt::red));
	// painter->drawLine(balls[ball1]->position->x, balls[ball1]->position->y, balls[ball1]->position->x + veltemp->x, balls[ball1]->position->y + veltemp->y);
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
	if (timestop){
		return;
	}

	// MOVE THE BALLS
	for (int p = 0; p < NUMBALLS; p++){
		if (!balls[p]->stationary)
		{
			balls[p]->move(1);
			balls[p]->velocity->Add(0, GRAVITY);//add gravity 
		}
	}
	

	// CHECK FOR FAILURE POINTS
	for (int j = 0; j < NUMBALLS; j++) {
		if (!balls[j]->stationary)
			//balls[j]->velocity->Add(0, 1);//add gravity
		for (int linkNum = 0; linkNum < linkID; linkNum++)
		{
			links[linkNum]->contract();
		}
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

				if (!balls[j]->stationary)
				{
					balls[j]->velocity->Sub(xColAdjust1);
				}
				else
				{
					balls[j]->position->x = balls[j]->stationX;
					balls[j]->position->y = balls[j]->stationY;
				}
				if (!balls[i]->stationary)
				{
					balls[i]->velocity->Add(xColAdjust2);
				}
				else
				{
					balls[i]->position->x = balls[i]->stationX;
					balls[i]->position->y = balls[i]->stationY;
				}
				delete xColAdjust1;
				delete xColAdjust2;

			}
			//*/
		}
	}

	for (int linkNum = 0; linkNum < linkID; linkNum++)
	{
		links[linkNum]->contract();
	}

	// MOVE THE BALL ON THE SCREEN
	for (int d = 0; d < NUMBALLS; d++){
		balls[d]->setPos(balls[d]->position->x, balls[d]->position->y);
	}
	for (int linkNum = 0; linkNum < linkID; linkNum++)
	{
		links[linkNum]->update();
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
					Link* newLink = new Link(ball1ID, i);
					links[linkID - 1] = newLink;
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

	for (int i = 0; i < 5; i++){//these are the moveable balls
		balls[i] = new Ball((i * 40) + 170, 350, 0, 0, 20, 10, i + 10, false);
		canvas->addItem(balls[i]);
		balls[i]->setPos(balls[i]->position->x, balls[i]->position->y);
	}
	for (int i = 0; i < 5; i++)//this is the support structure
	{
		balls[i + 5] = new Ball((i * 40) + 170, 250, 0, 0, 20, 10, i + 10, true);
		canvas->addItem(balls[i + 5]);
		balls[i + 5]->setPos(balls[i + 5]->position->x, balls[i + 5]->position->y);
	}

	for (int i = 0; i < 5; i++)
	{
		links[i] = new Link(i, i + 5);
		canvas->addItem(links[i]);

	}
	balls[0]->position->x -= 1;
	balls[0]->position->y -= 200;
	balls[0]->velocity->y = 0;
	balls[0]->setPos(balls[0]->position->x, balls[0]->position->y);
	qDebug() << "velocity " << balls[0]->velocity->x << " " << balls[0]->velocity->y;

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

