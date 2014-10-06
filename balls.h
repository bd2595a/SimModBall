#include <Qtwidgets>
#include <QWidget>

class Vector
{

public:
	float x;
	float y;
	Vector(float, float);
	Vector();
	void Add(Vector*);
	void Add(float, float);
	void Sub(Vector*);
	void Multiply(float);
	float DotProduct(Vector*);
	float Length();
	Vector* normalize();
};

class Ball : public QGraphicsItem
{
public:
	Vector* position;
	Vector* velocity;
	float radius;
	float mass;
	int id;


	//position x,y , velocity x,y ,radius,id
	Ball();
	Ball(float, float, float, float, float, float, int);
	//draw
	void printBall();
	//move delta time  
	void move(float);

	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget);

};

class Link : public QGraphicsItem
{
public:
	int id;
	int ball[];
	Link();
	Link(Ball,Ball,int);
	void contract();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget);
};

//like a Runnable subclass
class TimerHandler : public QObject
{
Q_OBJECT
public:
	TimerHandler(int);
	protected slots:
	void onTimer();
};



