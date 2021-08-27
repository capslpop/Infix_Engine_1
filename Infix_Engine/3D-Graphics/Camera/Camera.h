#pragma once



class Camera
{
public:
	Camera();
	glm::mat4 UpdateCameraMat(int window_width, int window_hight, float cursor_dx, float cursor_dy);
	~Camera();
	
public:
	glm::vec3 cam_dir = glm::vec3(4.6, 3.14, 0.0); 
	glm::vec3 cam_pos = glm::vec3(257.0, 0.0, 0.0);

	void update_delta_time();

private:
	const float PI = 3.1415926535897932384626433832795028;
	const float PI_HALF = PI / 2;
	const float PI_AND_HALF = PI + PI / 2;

	double oldTimeSinceStart = 0.0;
	float deltaTime = 0.0;
};

Camera::Camera()
{
}

glm::mat4 Camera::UpdateCameraMat(int window_width, int window_hight, float cursor_dx, float cursor_dy)
{
	// Projection matrix : 60° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::mat4(1);
	Projection = glm::perspective(glm::radians(60.0f), (float)window_width / (float)window_hight, 0.1f, 1000.0f);

	cam_dir.x += cursor_dx / window_width; //* deltaTime;
	cam_dir.y -= cursor_dy / window_hight; //* deltaTime;

	if (cam_dir.y <= PI_HALF) cam_dir.y = PI_HALF;
	if (cam_dir.y >= PI_AND_HALF) cam_dir.y = PI_AND_HALF;

	//input_move_camera_forward

	float cam_x_sin = sin(cam_dir.x);
	float cam_x_cos = cos(cam_dir.x);

	cam_pos += glm::vec3(input_move_camera_sideways * cam_x_cos -  input_move_camera_forward * cam_x_sin, input_move_camera_upward, -input_move_camera_sideways * cam_x_sin -  input_move_camera_forward * cam_x_cos) * deltaTime * 40.0f;

	// view matrix
	// rotate around to a given bearing: yaw
	glm::mat4 trans = glm::translate(glm::mat4(1), cam_pos);


	glm::mat4 camera_view = glm::rotate(glm::mat4(1), cam_dir.x, glm::vec3(0, 1, 0));
	// Define the 'look up' axis, should be orthogonal to the up axis
	// rotate around to the required head tilt: pitch
	camera_view = glm::rotate(camera_view, cam_dir.y, glm::vec3(1, 0, 0)); // pitch
	// now get the view matrix by taking the camera inverse

	//camera_view = glm::translate(camera_view, glm::vec3(0.0, 0.0, 5.0));

	glm::mat4 View = glm::inverse( trans * camera_view ); // trans * camera_view for free cam



	glm::mat4 Model = glm::mat4(1);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	return mvp;
}

void Camera::update_delta_time()
{
	double timeSinceStart = glfwGetTime();
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;
}

Camera::~Camera()
{
}