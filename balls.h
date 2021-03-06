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
	void Multiply(Vector*);
	void Divide(Vector*);
	float DotProduct(Vector*);
	float Length();
	void makePerpindicular();
	void makePerpindicular2();
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
	bool stationary;
	int stationX, stationY;


	//position x,y , velocity x,y ,radius,id, stationary
	Ball();
	Ball(float, float, float, float, float, float, int, bool);
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
	int ball1, ball2;
	float dist;
	Link();
	Link(int,int);
	//ball 1 index, ball 2 index, ID
	void contract();
	QRectF boundingRect() const;
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



class QGraphicsScene;

class BallView : public QGraphicsView
{
	Q_OBJECT
public:

	BallView(QGraphicsScene*, QWidget*x = NULL);
	int pressX, pressY;
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
};
