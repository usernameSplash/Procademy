
enum class eObjectType
{
	Player = 0,
	Enemy,
	Bullet
};

class BaseObject
{
public:
	BaseObject(eObjectType objectType, int x, int y);
	virtual ~BaseObject();

	virtual bool Update(void) = 0;
	virtual void Render(void) = 0;

	eObjectType GetObjectType(void);

protected:
	eObjectType mObjectType;
	int mX;
	int mY;
};