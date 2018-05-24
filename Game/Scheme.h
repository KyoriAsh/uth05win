#pragma once

namespace th5w{

class CScheme
{
public:
	virtual int Step()=0;
	virtual void Draw()=0;

public:
	CScheme(void);
	virtual ~CScheme(void);
};

}
