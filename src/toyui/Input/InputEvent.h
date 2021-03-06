//  Copyright (c) 2016 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#ifndef TOY_INPUTEVENT_H
#define TOY_INPUTEVENT_H

/* toy */
#include <toyui/Input/InputDevice.h>
#include <toyui/Widget/RootSheet.h>

namespace toy
{
	struct TOY_UI_EXPORT KeyDownEvent : public KeyEvent
	{
		KeyDownEvent(KeyCode code, char c) : KeyEvent(DEVICE_KEYBOARD, EVENT_PRESSED, code, c) {}

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_keyboard.dispatchKeyPressed(m_code, m_char); }
		virtual bool receive(InputAdapter& receiver) { return receiver.keyDown(*this); }
	};

	struct TOY_UI_EXPORT KeyUpEvent : public KeyEvent
	{
		KeyUpEvent(KeyCode code, char c) : KeyEvent(DEVICE_KEYBOARD, EVENT_RELEASED, code, c) {}

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_keyboard.dispatchKeyReleased(m_code, m_char); }
		virtual bool receive(InputAdapter& receiver) { return receiver.keyUp(*this); }
	};

	struct TOY_UI_EXPORT KeyCharEvent : public KeyEvent
	{
		KeyCharEvent(KeyCode code, char c) : KeyEvent(DEVICE_KEYBOARD, EVENT_STROKED, code, c) {}

		//virtual void dispatch(RootSheet& rootSheet) { rootSheet.keyboard().dispatchKeyReleased(m_code, m_char); }
		virtual bool receive(InputAdapter& receiver) { return receiver.keyStroke(*this); }
	};

	struct TOY_UI_EXPORT MouseMoveEvent : public MouseEvent
	{
		MouseMoveEvent(Mouse& mouse, DimFloat pos) : MouseEvent(mouse, DEVICE_MOUSE, EVENT_MOVED, pos) {}

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_mouse.dispatchMouseMoved(m_pos); }
		virtual bool receive(InputAdapter& receiver) { return receiver.mouseMoved(*this); }
	};

	struct TOY_UI_EXPORT MousePressEvent : public MouseEvent
	{
		MousePressEvent(Mouse& mouse, DeviceType deviceType, DimFloat pos) : MouseEvent(mouse, deviceType, EVENT_PRESSED, pos) {}

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_mouse.dispatchMousePressed(m_pos, m_button); }
		virtual bool receive(InputAdapter& receiver) { return receiver.mousePressed(*this); m_consumed = true; }
	};

	struct TOY_UI_EXPORT MouseReleaseEvent : public MouseEvent
	{
		MouseReleaseEvent(Mouse& mouse, DeviceType deviceType, DimFloat pos) : MouseEvent(mouse, deviceType, EVENT_RELEASED, pos) {}

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_mouse.dispatchMouseReleased(m_pos, m_button); }
		virtual bool receive(InputAdapter& receiver) { return receiver.mouseReleased(*this); }
	};

	struct TOY_UI_EXPORT MouseWheelEvent : public MouseEvent
	{
		MouseWheelEvent(Mouse& mouse, DimFloat pos, float amount) : MouseEvent(mouse, DEVICE_MOUSE_MIDDLE_BUTTON, EVENT_MOVED, pos) { m_deltaZ = amount; }

		virtual void dispatch(RootSheet& rootSheet) { rootSheet.m_mouse.dispatchMouseWheeled(m_pos, m_deltaZ); }
		virtual bool receive(InputAdapter& receiver) { return receiver.mouseWheel(*this); }
	};

	struct TOY_UI_EXPORT MouseClickEvent : public MouseEvent
	{
		MouseClickEvent(Mouse& mouse, DeviceType deviceType, MouseEvent& source) : MouseEvent(mouse, deviceType, EVENT_STROKED, source) {}
		virtual bool receive(InputAdapter& receiver) {
			if(m_deviceType == DEVICE_MOUSE_LEFT_BUTTON)
				return receiver.leftClick(*this);
			else if(m_deviceType == DEVICE_MOUSE_RIGHT_BUTTON)
				return receiver.rightClick(*this);
			else // if(m_deviceType == DEVICE_MOUSE_MIDDLE_BUTTON)
				return receiver.middleClick(*this);
		}
	};

	struct TOY_UI_EXPORT MouseDragEvent : public MouseEvent
	{
		MouseDragEvent(Mouse& mouse, DeviceType deviceType, MouseEvent& source) : MouseEvent(mouse, deviceType, EVENT_DRAGGED, source) {}

		virtual bool receive(InputAdapter& receiver) {
			if(m_deviceType == DEVICE_MOUSE_LEFT_BUTTON)
				return receiver.leftDrag(*this);
			else if(m_deviceType == DEVICE_MOUSE_RIGHT_BUTTON)
				return receiver.rightDrag(*this);
			else // if(m_deviceType == DEVICE_MOUSE_MIDDLE_BUTTON)
				return receiver.middleDrag(*this);
		}
	};

	struct TOY_UI_EXPORT MouseDragStartEvent : public MouseEvent
	{
		MouseDragStartEvent(Mouse& mouse, DeviceType deviceType, MouseEvent& source) : MouseEvent(mouse, deviceType, EVENT_DRAGGED_START, source) {}

		virtual bool receive(InputAdapter& receiver) {
			if(m_deviceType == DEVICE_MOUSE_LEFT_BUTTON)
				return receiver.leftDragStart(*this);
			else if(m_deviceType == DEVICE_MOUSE_RIGHT_BUTTON)
				return receiver.rightDragStart(*this);
			else // if(m_deviceType == DEVICE_MOUSE_MIDDLE_BUTTON)
				return receiver.middleDragStart(*this);
		}
	};

	struct TOY_UI_EXPORT MouseDragEndEvent : public MouseEvent
	{
		MouseDragEndEvent(Mouse& mouse, DeviceType deviceType, MouseEvent& source) : MouseEvent(mouse, deviceType, EVENT_DRAGGED_END, source) {}

		virtual bool receive(InputAdapter& receiver) {
			if(m_deviceType == DEVICE_MOUSE_LEFT_BUTTON)
				return receiver.leftDragEnd(*this);
			else if(m_deviceType == DEVICE_MOUSE_RIGHT_BUTTON)
				return receiver.rightDragEnd(*this);
			else // if(m_deviceType == DEVICE_MOUSE_MIDDLE_BUTTON)
				return receiver.middleDragEnd(*this);
		}
	};

	struct TOY_UI_EXPORT MouseEnterEvent : public MouseEvent
	{
		MouseEnterEvent(Mouse& mouse, DimFloat pos) : MouseEvent(mouse, DEVICE_MOUSE, EVENT_ENTERED, pos) {}

		virtual bool receive(InputAdapter& receiver) { return receiver.mouseEntered(*this); }
	};

	struct TOY_UI_EXPORT MouseLeaveEvent : public MouseEvent
	{
		MouseLeaveEvent(Mouse& mouse, DimFloat pos) : MouseEvent(mouse, DEVICE_MOUSE, EVENT_LEAVED, pos) {}

		virtual bool receive(InputAdapter& receiver) { return receiver.mouseLeaved(*this); }
	};
}

#endif // TOY_INPUTEVENT_H
