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
QGraphicsScene* scene;//canvas
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

Link::Link(int b1num,int b2num,int idnum){
	ball[0] = b1num;
	ball[1] = b2num;
	id = idnum;
	linkID++;
}

// KEYPRESS

KeyPress::KeyPress(QWidget *parent) : QWidget(parent)
{
    myLabel = new QLabel("LABEL");
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(myLabel);
    setLayout(mainLayout);
 
}
 
void KeyPress::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        myLabel->setText("You pressed SPACE");
    }
}
 
void KeyPress::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        myLabel->setText("You released SPACE");
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
	return QRectF(balls[ball[0]]->position->x, balls[ball[0]]->position->y, 10, 10);
}


//drawing goes here
void Link::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->drawLine(50, 50, 450, 450);
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
	/*
	case Qt::Key_Space:
	{
		if (timestop)
			timestop = false;
		else //timestop
			timestop = true;
	}
	if(timestop){
		return
	}
	*/
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
				if(balls[j]->mass != balls[i]->mass){
					mj= balls[j]->mass / (balls[j]->mass + balls[i]->mass);
					mi= balls[i]->mass / (balls[j]->mass + balls[i]->mass);
				}
				else{
					mj = 0.5;
					mi = 0.5;
				}

				if (balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y)
				{	// Jx > IX && JY > IY
					balls[j]->position->Add(xoverlap * mj, yoverlap * mj);
					balls[i]->position->Add(xoverlap * -mi , yoverlap * -mi);
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
			if (dist->Length() <= balls[j]->radius + balls[i]->radius  || bounce){	
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
					new Link(ball1ID, i, linkID);
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
	scene = new QGraphicsScene();
	scene->setSceneRect(0, 0, 500, 500);
	srand(time(NULL));

	for (int i = 0; i < NUMBALLS; i++){
		balls[i] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, 10, i);
		scene->addItem(balls[i]);
		balls[i]->setPos(balls[i]->position->x, balls[i]->position->y);
	}


 	KeyPress *keyPress = new KeyPress();
    keyPress->show();

	view = new BallView(scene);
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