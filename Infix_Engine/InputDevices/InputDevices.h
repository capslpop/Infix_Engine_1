#pragma once


double input_key_init_pos_x = 0.0;
double input_key_init_pos_y = 0.0;

double input_key_pos_x = 0.0;
double input_key_pos_y = 0.0;



bool input_mouse_enable_cam = false;

float input_move_camera_forward = 0.0;
float input_move_camera_sideways = 0.0;
float input_move_camera_upward = 0.0;

class InputDevices
{
public:
	InputDevices(GLFWwindow* window);
	~InputDevices() {};
	void AddKeyboard( GLFWkeyfun keyfun);
	void AddMouse(GLFWcursorposfun cursorposfun, GLFWmousebuttonfun mousebuttonfun);

	void Update();
	void UpdateMouse();


	static void KeyDown(int key);// Declare KeyDown function to be used after main()
	static void KeyUp(int key);// Declare KeyUp function to be used after main()

	static void MousePos(double x, double y);// Declare MousePos function to be used after main()
	static void MouseButtonDown(int button);// Declare MousePos function to be used after main()
	static void MouseButtonUp(int button);// Declare MousePos function to be used after main()

	double mouse_x = 0.0;
	double mouse_y = 0.0;
	double mouse_dx = 0.0;
	double mouse_dy = 0.0;

private:
	double init_mouse_x, init_mouse_y;
	GLFWwindow* window;
};

InputDevices::InputDevices(GLFWwindow* in_window)
{
	window = in_window;
}

void InputDevices::AddKeyboard(GLFWkeyfun keyfun)
{
	glfwSetKeyCallback(window, keyfun);
}

void InputDevices::AddMouse(GLFWcursorposfun cursorposfun, GLFWmousebuttonfun mousebuttonfun)
{
	glfwSetCursorPosCallback(window, cursorposfun); // Only for cursor position

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetMouseButtonCallback(window, mousebuttonfun); // Then get mouse button input
}

void InputDevices::Update()
{
	glfwPollEvents();

	this->UpdateMouse();
}

void InputDevices::UpdateMouse()
{
		init_mouse_x = mouse_x;
		init_mouse_y = mouse_y;

		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		mouse_dx = mouse_x - init_mouse_x; // Calculate the velocity of the mouse X pos
		mouse_dy = mouse_y - init_mouse_y; // Calculate the velocity of the mouse Y pos


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) // If this is not done then the velocity of dx/dy will be calculated when the mouse pos is reset.
		{
			mouse_dx = 0.0;
			mouse_dy = 0.0;
		}
}


//************************************************************************************************************
// GLFW Callback Functions:
//
//************************************************************************************************************

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		InputDevices::KeyDown(key);
	}

	if (action == GLFW_RELEASE)
	{
		InputDevices::KeyUp(key);
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	InputDevices::MousePos(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		InputDevices::MouseButtonDown(button);
		if (button == GLFW_MOUSE_BUTTON_2)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			input_mouse_enable_cam = true;
		}
	}

	if (action == GLFW_RELEASE)
	{
		InputDevices::MouseButtonUp(button);
		if (button == GLFW_MOUSE_BUTTON_2)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			input_mouse_enable_cam = false;
		}
	}
}