#include <Qtwidgets>


class Ball : public QGraphicsItem
{
public:
	float position;
	float radius;
	float velocity;
	int id;


	//position, velocity, radius, id
	Ball();
	Ball(float, float, float, int);
	//draw
	void printBall();
	//move delta time  
	void move(float);

	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget);

	//GETSET
	void setVelocity(float);
};

//like a Runnable subclass
class TimerHandler : public QObject
{
public:
	Q_OBJECT
public:
	TimerHandler(int);
	protected slots:
	void onTimer();
};

