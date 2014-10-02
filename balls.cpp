#include <iostream>
#include <QtWidgets>
#include "balls.h"
#include <time.h>
#include <math.h>

using namespace std;	//std::cout is annoying

//GLOBALS
const int NUMBALLS = 10;
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
	return ((x*vector2->x)+(y*vector2->y));
}

float Vector::Length()
{
	return sqrt(((x)*(x)) + ((y)* (y)));
}

// BALL
Ball::Ball(){}

Ball::Ball(float px, float py, float vx, float vy, float r, int i)
{
	position = new Vector(px, py);
	velocity = new Vector(vx, vy);
	radius = r; id = i;
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

			/* PSEUDO CODE
			against each ball[j] and ball[i]
			Vector dist = new Vector( absJC(ball[j]->position->x - ball[i]->position->x ) , absJC(ball[j]->position->y - ball[i]->position->y )  )
			if ( dist->length() <= ( ball[j]->radius + ball[i]->radius ) ){
			collision stuff
			}
			if (same if statement as above except < instead of <= ){
			// overlap stuff
			}
			*/

			Vector* dist = new Vector(absJC(balls[j]->position->x - balls[i]->position->x),absJC(balls[j]->position->y - balls[i]->position->y));

			/*
			//X overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->x / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			// Y overlap
			( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->y / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) )
			EACH ball should be moved [ X overlap / 2 , Y overlap / 2] away from the other
			*/

			// COLLISION
			// collide
			if (dist->Length() < balls[j]->radius + balls[i]->radius){//correct for the overlap, then bounce them
				// BREE CODE
				// dist->x /= 7;
				// dist->y /= 7;
				// balls[j]->position->Add(dist->x, dist->y);
				// balls[i]->position->Sub(dist);

				//JC Code
				float xoverlap = ( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->x / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) );
				float yoverlap = ( ( balls[i]->radius + balls[j]->radius ) - sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) ) * ( dist->y / sqrt( ( dist->x * dist->x ) + ( dist->y * dist->y ) ) );

				if (balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y )
				{	// Jx > IX && JY > IY
					balls[j]->position->Add(xoverlap/2,yoverlap/2);
					balls[j]->position->Add(xoverlap/-2,yoverlap/-2);
				}
				else if(balls[j]->position->x > balls[i]->position->x && balls[j]->position->y < balls[i]->position->y )
				{	// JX > IX && JY < IY
					balls[j]->position->Add(xoverlap/2,yoverlap/-2);
					balls[j]->position->Add(xoverlap/-2,yoverlap/2);
				}
				else if(balls[j]->position->x < balls[i]->position->x && balls[j]->position->y > balls[i]->position->y )
				{	// JX < IX && JY > IY
					balls[j]->position->Add(xoverlap/-2,yoverlap/2);
					balls[j]->position->Add(xoverlap/2,yoverlap/-2);
				}
				else //if(balls[j]->position->x > balls[i]->position->x && balls[j]->position->y > balls[i]->position->y ){
				{	// JX > IX && JY > IY
					balls[j]->position->Add(xoverlap/-2,yoverlap/-2);
					balls[j]->position->Add(xoverlap/2,yoverlap/2);
				}

				// OLD CODE
				//if (balls[j]->position->x > balls[i]->position->x){
				//	balls[j]->position->Add(dist->x, dist->y);
				//	balls[i]->position->Sub(dist);
				//}
				//else{
				//	//move j right, move i left
				//	balls[i]->position->Add(dist->x, dist->y);
				//	balls[j]->position->Sub(dist);
				//}
			}
			if (dist->Length() <= balls[j]->radius + balls[i]->radius){ 
				//calculations for J's velocity
				//calculate the normal plane 
				float lengthJ = dist->Length();
				float xJ = balls[j]->velocity->x / lengthJ;
				float yJ = balls[j]->velocity->y / lengthJ;
				float lengthI = dist->Length();
				float xI = balls[i]->velocity->x / lengthI;
				float yI = balls[i]->velocity->y / lengthI;

				Vector* normalPlaneJ = new Vector(xJ, yJ);
				Vector* normalPlaneI = new Vector(xI, yI);
				//calculate the collisionPlane
				Vector* collisionPlaneJ = new Vector(-1 * normalPlaneJ->y, normalPlaneJ->x);
				Vector* collisionPlaneI = new Vector(-1 * normalPlaneI->y, normalPlaneI->x);
				//calculate prior velocities relative to the collisoin plane and normal
				float n_velRJ = normalPlaneJ->DotProduct(balls[j]->velocity);
				float c_velRJ = collisionPlaneJ->DotProduct(balls[j]->velocity);
				float n_velRI = normalPlaneI->DotProduct(balls[i]->velocity);
				float c_velRI = collisionPlaneI->DotProduct(balls[i]->velocity);


				//calculate the scaler velocities of each object after the collision
				float n_velJ_after = ((n_velRJ * (balls[j]->radius - balls[i]->radius)) + (2 * balls[i]->radius * n_velRI)) / (balls[i]->radius + balls[j]->radius);
				float n_velI_after = ((n_velRI * (balls[i]->radius - balls[j]->radius)) + (2 * balls[j]->radius * n_velRJ)) / (balls[i]->radius + balls[j]->radius);
			
				//convert scalers to vectors by multplying the normalised plane vectors and get vectors into a single vector in world space
				normalPlaneJ->Multiply(n_velJ_after);
				collisionPlaneJ->Multiply(c_velRJ);
				normalPlaneJ->Add(collisionPlaneJ);
				normalPlaneI->Multiply(n_velI_after);
				collisionPlaneI->Multiply(c_velRI);
				normalPlaneI->Add(collisionPlaneI);


				// Reapply the move-back from before the collision (using the post collision velocity)
				balls[i]->position->Add(normalPlaneI);
				balls[j]->position->Add(normalPlaneJ);
				balls[i]->velocity = normalPlaneJ;
				balls[j]->velocity = normalPlaneI;
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
		balls[i] = new Ball(RandomNumber(50, 450), RandomNumber(50, 450), RandomNumber(-5, 5), RandomNumber(-5, 5), 20, i);
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