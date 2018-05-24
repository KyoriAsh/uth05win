#pragma once

namespace th5w{

class CCommonFunctionInput
{
public:
	static unsigned short GetAllKeyState();
	__forceinline static bool UpKey(unsigned short keyState)
	{
		return (keyState&1)!=0;
	}
	__forceinline static bool DownKey(unsigned short keyState)
	{
		return (keyState&2)!=0;
	}
	__forceinline static bool LeftKey(unsigned short keyState)
	{
		return (keyState&4)!=0;
	}
	__forceinline static bool RightKey(unsigned short keyState)
	{
		return (keyState&8)!=0;
	}
	__forceinline static bool XKey(unsigned short keyState)
	{
		return (keyState&16)!=0;
	}
	__forceinline static bool ZKey(unsigned short keyState)
	{
		return (keyState&32)!=0;
	}
	__forceinline static bool LShiftKey(unsigned short keyState)
	{
		return (keyState&64)!=0;
	}
	__forceinline static bool LCtrlKey(unsigned short keyState)
	{
		return (keyState&128)!=0;
	}
	__forceinline static bool ESCKey(unsigned short keyState)
	{
		return (keyState&4096)!=0;
	}
	__forceinline static bool EnterKey(unsigned short keyState)
	{
		return (keyState&8192)!=0;
	}
	__forceinline static bool QKey(unsigned short keyState)
	{
		return (keyState&16384)!=0;
	}
	__forceinline static bool UpPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&1)!=0;
	}
	__forceinline static bool DownPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&2)!=0;
	}
	__forceinline static bool LeftPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&4)!=0;
	}
	__forceinline static bool RightPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&8)!=0;
	}
	__forceinline static bool XPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&16)!=0;
	}
	__forceinline static bool ZPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&32)!=0;
	}
	__forceinline static bool LShiftPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&64)!=0;
	}
	__forceinline static bool LCtrlPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&128)!=0;
	}
	__forceinline static bool ESCPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&4096)!=0;
	}
	__forceinline static bool EnterPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&8192)!=0;
	}
	__forceinline static bool QPressed(unsigned short keyState,unsigned short lastState)
	{
		return (keyState&(~lastState)&16384)!=0;
	}
public:
	CCommonFunctionInput(void);
	~CCommonFunctionInput(void);
};

}
