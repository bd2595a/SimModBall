#include <iostream>
#include <QtWidgets>
#include "balls.h"

using namespace std;	//std::cout is annoying

//GLOBALS

const int NUMBALLS = 2;
Ball* balls[NUMBALLS];

float absolute(float val)
{
	if (val<0)
		return -val;
	else
		return val;
}

//METHODS
// BALL
Ball::Ball(){}

Ball::Ball(float p, float v, float r, int i)
{
	position = p; velocity = v; radius = r; id = i;

}

void Ball::printBall()
{
	cout << "The ball " << id << " is at position " << position << endl;
}
void Ball::move(float dt)
{
	// v = dx/dt
	position = position + velocity * dt;
}

void Ball::setVelocity(float v)
{
	velocity = v;
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
	for (int p = 0; p<NUMBALLS; p++){
		balls[p]->move(1);
	}

	// CHECK FOR FAILURE POINTS
	for (int j = 0; j<NUMBALLS; j++) {
		// CHECK IF OUT OF BOUNDS
		if (balls[j]->position < 0 + balls[j]->radius){
			// Wall L @ position 0
			balls[j]->position = balls[j]->radius;
			balls[j]->setVelocity(balls[j]->velocity * -1);
		}
		if (balls[j]->position > 500 - balls[j]->radius){
			// Wall R @ position 500
			balls[j]->position = 500 - balls[j]->radius;
			balls[j]->setVelocity(balls[j]->velocity * -1);
		}
		// CHECK AGAINST ALL OTHER BALLS
		for (int i = j + 1; i<NUMBALLS; i++){
			float dist = (absolute(balls[j]->position - balls[i]->position) - (balls[j]->radius + balls[i]->radius));
			if (dist <= 0){
				// COLLISSION
				float temp = balls[j]->velocity;
				balls[j]->setVelocity(balls[i]->velocity);
				balls[i]->setVelocity(temp);
			}
			if (dist < 0){
				// OVERLAP
				float movedist = absolute(dist) / 2;
				if (balls[j]->position < balls[i]->position){
					balls[j]->position -= movedist;
					balls[i]->position += movedist;
				}
				else{ //(balls[j]->position > balls[i]->position)
					balls[j]->position += movedist;
					balls[i]->position -= movedist;
				}
			}
		}
	}

	// MOVE THE BALL ON THE SCREEN
	for (int d = 0; d<NUMBALLS; d++){
		balls[d]->setPos(balls[d]->position, 300);;
	}

}

/////////////////////////////////
//           M A I N           //
/////////////////////////////////

int main(int argc, char** argv)
{

	//jframe
	QApplication app(argc, argv);
	//panel
	QGraphicsScene scene;
	//set window size to 500,500
	scene.setSceneRect(0, 0, 500, 500);


	balls[0] = new Ball(20, 2, 14, 0);	//create object, put addr in balls[0]
	balls[1] = new Ball(460, -1, 7, 1);

	scene.addItem(balls[0]);
	scene.addItem(balls[1]);

	balls[0]->setPos(balls[0]->position, 300);
	balls[1]->setPos(balls[1]->position, 300);
	QGraphicsView view(&scene);
	view.setWindowTitle("Balls!!!");
	view.resize(500, 500);
	//setVisible true
	view.show();

	//1000 milliseconds / frames-per-second
	TimerHandler timer(1000 / 66);

	//window is created here
	return app.exec();
}