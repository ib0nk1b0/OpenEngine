#pragma once

namespace OpenEngine {

	using MouseCode = uint16_t;

	namespace Mouse {

		enum : MouseCode
		{
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			ButtonLast = Button7,
			ButtonLeft = Button0,
			ButtonRight = Button1,
			ButtonMiddle = Button2
		};

	}

}

// TODO: Remove GLFW implementation

#define OE_MOUSE_BUTTON_1         0
#define OE_MOUSE_BUTTON_2         1
#define OE_MOUSE_BUTTON_3         2
#define OE_MOUSE_BUTTON_4         3
#define OE_MOUSE_BUTTON_5         4
#define OE_MOUSE_BUTTON_6         5
#define OE_MOUSE_BUTTON_7         6
#define OE_MOUSE_BUTTON_8         7
#define OE_MOUSE_BUTTON_LAST      OE_MOUSE_BUTTON_8
#define OE_MOUSE_BUTTON_LEFT      OE_MOUSE_BUTTON_1
#define OE_MOUSE_BUTTON_RIGHT     OE_MOUSE_BUTTON_2
#define OE_MOUSE_BUTTON_MIDDLE    OE_MOUSE_BUTTON_3