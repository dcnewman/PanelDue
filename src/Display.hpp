/*
 * Display.h
 *
 * Created: 04/11/2014 09:43:43
 *  Author: David
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "ecv.h"
#include "Hardware/UTFT.hpp"

// Fonts are held as arrays of 8-bit data in flash.
typedef const uint8_t * array LcdFont;

// An icon is stored an array of uint16_t data normally held in flash memory. The first value is the width in pixels, the second is the height in pixels.
// After that comes the icon data, 16 bits per pixel, one row at a time.
typedef const uint16_t * array Icon;

// Unicode strings for special characters in our font
#define DECIMAL_POINT	"\xC2\xB7"		// Unicode middle-dot
#define DEGREE_SYMBOL	"\xC2\xB0"		// Unicode degree-symbol
#define THIN_SPACE		"\xC2\x80"		// Unicode control character, we use it as thin space

const Colour red = UTFT::fromRGB(255,0,0);
const Colour yellow = UTFT::fromRGB(128,128,0);
const Colour green = UTFT::fromRGB(0,255,0);
const Colour turquoise = UTFT::fromRGB(0,128,128);
const Colour blue = UTFT::fromRGB(0,0,255);
const Colour magenta = UTFT::fromRGB(128,0,128);
const Colour white = 0xFFFF;
const Colour black = 0x0000;

const uint8_t buttonGradStep = 12;

typedef uint16_t PixelNumber;
typedef uint8_t event_t;
const event_t nullEvent = 0;

enum class TextAlignment : uint8_t { Left, Centre, Right };
	
class ButtonBase;

// Small by-value class to identify what button has been pressed
class ButtonPress
{
	ButtonBase * null button;
	unsigned int index;
	
public:
	ButtonPress();
	ButtonPress(ButtonBase *b, unsigned int pi);	
	void Clear();
	
	bool IsValid() const { return button != nullptr; }
	ButtonBase * GetButton() const { return button; }
	unsigned int GetIndex() const { return index; }

	event_t GetEvent() const;		
	int GetIParam() const;
	const char* array GetSParam() const;		
	bool operator==(const ButtonPress& other) const;
	
	bool operator!=(const ButtonPress& other) const { return !operator==(other); }
};

// Base class for a displayable field
class DisplayField
{
protected:
	PixelNumber y, x;							// Coordinates of top left pixel, counting from the top left corner
	PixelNumber width;							// number of pixels occupied in each direction
	Colour fcolour, bcolour;					// foreground and background colours
	bool changed;
	bool visible;
	
	static LcdFont defaultFont;
	static Colour defaultFcolour, defaultBcolour;
	static Colour defaultButtonBorderColour, defaultGradColour, defaultPressedBackColour, defaultPressedGradColour;
	
protected:
	DisplayField(PixelNumber py, PixelNumber px, PixelNumber pw);
	
	virtual PixelNumber GetHeight() const { return 1; }		// would like to make this pure virtual but then we get 50K of library that we don't want

public:
	DisplayField * null next;					// link to next field in list

	virtual bool IsButton() const { return false; }
	bool IsVisible() const { return visible; }
	void Show(bool v);
	virtual void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) { }		// would like to make this pure virtual but then we get 50K of library that we don't want
	void SetColours(Colour pf, Colour pb);
	void SetChanged() { changed = true; }
	PixelNumber GetMinX() const { return x; }
	PixelNumber GetMaxX() const { return x + width - 1; }
	PixelNumber GetMinY() const { return y; }
	PixelNumber GetMaxY() const { return y + GetHeight() - 1; }
		
	virtual event_t GetEvent() const { return nullEvent; }

	static void SetDefaultColours(Colour pf, Colour pb) { defaultFcolour = pf; defaultBcolour = pb; }
	static void SetDefaultColours(Colour pf, Colour pb, Colour pbb, Colour pg, Colour pbp, Colour pgp);
	static void SetDefaultFont(LcdFont pf) { defaultFont = pf; }
	static ButtonPress FindEvent(PixelNumber x, PixelNumber y, DisplayField * null p);
	
	// Icon management
	static PixelNumber GetIconWidth(Icon ic) { return ic[0]; }
	static PixelNumber GetIconHeight(Icon ic) { return ic[1]; }
	static const uint16_t * array GetIconData(Icon ic) { return ic + 2; }
		
	static PixelNumber GetTextWidth(const char* array s, PixelNumber maxWidth);		// find out how much width we need to print this text
};

class PopupWindow;

class Window
{
protected:
	DisplayField * null root;
	PopupWindow * null next;
	Colour backgroundColour;
	
public:
	Window(Colour pb);
	virtual PixelNumber Xpos() const { return 0; }
	virtual PixelNumber Ypos() const { return 0; }
	void AddField(DisplayField *p);
	ButtonPress FindEvent(PixelNumber x, PixelNumber y);
	ButtonPress FindEventOutsidePopup(PixelNumber x, PixelNumber y);
	DisplayField * null GetRoot() const { return root; }
	void Redraw(DisplayField *f);
	void Show(DisplayField *f, bool v);
	void Press(ButtonPress bp, bool v);
	void SetPopup(PopupWindow * p, PixelNumber px = 0, PixelNumber py = 0);
	void ClearPopup();
	bool ObscuredByPopup(const DisplayField *p) const;
	bool Visible(const DisplayField *p) const;
};

class MainWindow : public Window
{
public:
	MainWindow();
	void Init(Colour pb);
	void ClearAll();
	void Refresh(bool full = 0);
	void SetRoot(DisplayField * null r) { root = r; }
};

class PopupWindow : public Window
{
private:
	PixelNumber height, width, xPos, yPos;
	
public:
	PopupWindow(PixelNumber ph, PixelNumber pw, Colour pb);
	PixelNumber GetHeight() const { return height; }
	PixelNumber GetWidth() const { return width; }
	PixelNumber Xpos() const override { return xPos; }
	PixelNumber Ypos() const override { return yPos; }
	void Refresh(bool full);
	void SetPos(PixelNumber px, PixelNumber py) { xPos = px; yPos = py; }
};

// Base class for fields displaying text
class FieldWithText : public DisplayField
{
	LcdFont font;
	TextAlignment align;
	
protected:
	PixelNumber GetHeight() const override { return UTFT::GetFontHeight(font); }
	
	virtual void PrintText() const {}		// would ideally be pure virtual

	FieldWithText(PixelNumber py, PixelNumber px, PixelNumber pw, TextAlignment pa)
		: DisplayField(py, px, pw), font(DisplayField::defaultFont), align(pa)
	{
	}
		
public:
	void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) override final;
};
	
class TextField : public FieldWithText
{
	const char* array null label;
	const char* array null text;
	
protected:
	void PrintText() const override;

public:
	TextField(PixelNumber py, PixelNumber px, PixelNumber pw, TextAlignment pa, const char * array null pl, const char* array null pt = nullptr)
		: FieldWithText(py, px, pw, pa), label(pl), text(pt)
	{
	}

	void SetValue(const char* array s)
	{
		text = s;
		changed = true;
	}

	void SetLabel(const char* array s)
	{
		label = s;
		changed = true;
	}
};

class FloatField : public FieldWithText
{
	const char* array null label;
	const char* array null units;
	float val;
	uint8_t numDecimals;

protected:
	void PrintText() const override;

public:
	FloatField(PixelNumber py, PixelNumber px, PixelNumber pw, TextAlignment pa, uint8_t pd, const char * array pl = NULL, const char * array null pu = NULL)
		: FieldWithText(py, px, pw, pa), label(pl), units(pu), val(0.0), numDecimals(pd)
	{
	}

	void SetValue(float v)
	{
		val = v;
		changed = true;
	}
};

class IntegerField : public FieldWithText
{
	const char* array null label;
	const char* array null units;
	int val;

protected:
	void PrintText() const override;

public:
	IntegerField(PixelNumber py, PixelNumber px, PixelNumber pw, TextAlignment pa, const char *pl = NULL, const char *pu = NULL)
		: FieldWithText(py, px, pw, pa), label(pl), units(pu), val(0)
	{
	}

	void SetValue(int v)
	{
		val = v;
		changed = true;
	}
};

class StaticTextField : public FieldWithText
{
	const char * array null text;

protected:
	void PrintText() const override;

public:
	StaticTextField(PixelNumber py, PixelNumber px, PixelNumber pw, TextAlignment pa, const char * array null pt)
		: FieldWithText(py, px, pw,pa), text(pt) {}

	void SetValue(const char* array null pt)
	{
		text = pt;
		changed = true;
	}
};

class ButtonBase : public DisplayField
{
protected:
	Colour borderColour, gradColour, pressedBackColour, pressedGradColour;
	event_t evt;								// event number that is triggered by touching this field
	bool pressed;								// putting this here instead of in SingleButton saves 4 byes per button

	
	ButtonBase(PixelNumber py, PixelNumber px, PixelNumber pw);

	void DrawOutline(PixelNumber xOffset, PixelNumber yOffset, bool isPressed) const;
	
	static PixelNumber textMargin;
	static PixelNumber iconMargin;

public:
	event_t GetEvent() const override { return evt; }
	virtual const char* null GetSParam(unsigned int index) const { return nullptr; }
	virtual int GetIParam(unsigned int index) const { return 0; }
	virtual void Press(bool p, int index) { }
};

class SingleButton : public ButtonBase
{
	union
	{
		const char* null sParam;
		int iParam;
		//float fParam;
	} param;

protected:
	SingleButton(PixelNumber py, PixelNumber px, PixelNumber pw);
	
	void DrawOutline(PixelNumber xOffset, PixelNumber yOffset) const;
	
public:
	bool IsButton() const override final { return true; }

	void SetEvent(event_t e, const char* null sp ) { evt = e; param.sParam = sp; }
	void SetEvent(event_t e, int ip ) { evt = e; param.iParam = ip; }
	//void SetEvent(event_t e, float fp ) { evt = e; param.fParam = fp; }

	const char* null GetSParam(unsigned int index) const override { return param.sParam; }
	int GetIParam(unsigned int index) const override { return param.iParam; }
	//float GetFParam() const { return param.fParam; }

	void Press(bool p, int index) override
	{
		if (p != pressed)
		{
			pressed = p;
			changed = true;
		}
	}
	
	static void SetTextMargin(PixelNumber p) { textMargin = p; }
	static void SetIconMargin(PixelNumber p) { iconMargin = p; }
};

class ButtonWithText : public SingleButton
{
	LcdFont font;
	
protected:
	PixelNumber GetHeight() const override { return UTFT::GetFontHeight(font) + 2 * textMargin + 2; }

	virtual void PrintText() const {}		// ideally would be pure virtual

public:
	ButtonWithText(PixelNumber py, PixelNumber px, PixelNumber pw)
		: SingleButton(py, px, pw), font(DisplayField::defaultFont) {}

	void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) override final;	
};

// Character button. The character on the button is the same as the integer event parameter.
class CharButton : public ButtonWithText
{
protected:
	void PrintText() const override;

public:
	CharButton(PixelNumber py, PixelNumber px, PixelNumber pw, char pc, event_t e);
};

class ButtonRow : public ButtonBase
{
	unsigned int numButtons;
	int whichPressed;
	PixelNumber step;
	
public:
	ButtonRow(PixelNumber py, PixelNumber px, PixelNumber pw, PixelNumber ps, unsigned int nb, event_t e);
};

class ButtonRowWithText : public ButtonRow
{
	LcdFont font;

protected:
	virtual void PrintText(unsigned int n) const { }

public:
	ButtonRowWithText(PixelNumber py, PixelNumber px, PixelNumber pw, PixelNumber ps, unsigned int nb, event_t e);

	void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) override final;
};

class CharButtonRow : public ButtonRowWithText
{
	const char * array text;

protected:
	void PrintText(unsigned int n) const override;

public:
	CharButtonRow(PixelNumber py, PixelNumber px, PixelNumber pw, PixelNumber ps, const char * array s, event_t e);
};

class TextButton : public ButtonWithText
{
	const char * array null text;
	
protected:
	void PrintText() const override;

public:
	TextButton(PixelNumber py, PixelNumber px, PixelNumber pw, const char * array null pt, event_t e, int param = 0);
	TextButton(PixelNumber py, PixelNumber px, PixelNumber pw, const char * array null pt, event_t e, const char * array param);

	void SetText(const char* array null pt)
	{
		text = pt;
		changed = true;
	}
};

class IconButton : public SingleButton
{
	Icon icon;
	
protected:
	PixelNumber GetHeight() const override { return GetIconHeight(icon) + 2 * iconMargin + 2; }

public:
	IconButton(PixelNumber py, PixelNumber px, PixelNumber pw, Icon ic, event_t e, int param = 0);

	void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) override final;
};

class IntegerButton : public ButtonWithText
{
	const char* array null label;
	const char* array null units;
	int val;

protected:
	void PrintText() const override;

public:
	IntegerButton(PixelNumber py, PixelNumber px, PixelNumber pw, const char * array pl = nullptr, const char * array pt = nullptr)
		: ButtonWithText(py, px, pw), label(pl), units(pt), val(0) {}

	int GetValue() const { return val; }

	void SetValue(int pv)
	{
		val = pv;
		changed = true;
	}

	void Increment(int amount)
	{
		val += amount;
		changed = true;
	}
};

class FloatButton : public ButtonWithText
{
	const char * array null units;
	float val;
	uint8_t numDecimals;

protected:
	void PrintText() const override;

public:
	FloatButton(PixelNumber py, PixelNumber px, PixelNumber pw, uint8_t pd, const char * array pt = nullptr)
		: ButtonWithText(py, px, pw), units(pt), val(0.0), numDecimals(pd) {}

	float GetValue() const { return val; }

	void SetValue(float pv)
	{
		val = pv;
		changed = true;
	}

	void Increment(int amount)
	{
		val += amount;
		changed = true;
	}
};

class ProgressBar : public DisplayField
{
	PixelNumber lastNumPixelsSet;
	uint8_t height;
	uint8_t percent;
public:
	ProgressBar(uint16_t py, uint16_t px, uint8_t ph, uint16_t pw)
		: DisplayField(py, px, pw), lastNumPixelsSet(0), height(ph), percent(0)
	{
	}
	
	void Refresh(bool full, PixelNumber xOffset, PixelNumber yOffset) override;
	
	virtual PixelNumber GetHeight() const override { return height; }

	void SetPercent(uint8_t pc)
	{
		percent = pc;
		changed = true;
	}
};

#endif /* DISPLAY_H_ */