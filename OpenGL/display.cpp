#include "Display.h"
#include <shader_m.h>
#include <iostream>
Display::~Display() {}
Display::Display() {
}
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int flag = 0;   //����Ƿ���
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float fov = 45.0f;
bool firstMouse = true;
Shader ourShader;
unsigned int VBO, VAO;
unsigned int VBO1, VAO1;
unsigned int texture1;
unsigned int texture2;
unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

glm::vec3 cubePositions = glm::vec3(0.0f,  0.0f,  0.0f);
int Display::display_init(cv::Mat img)
{
	SCR_WIDTH = img.cols + 650;
	SCR_HEIGHT = img.rows;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "show", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	ourShader.Shader1("E:/code/project/gimble4.5/model/7.3.camera.vs", "E:/code/project/gimble4.5/model/7.3.camera.fs");

	float ratio = static_cast<float>(img.cols - SCR_WIDTH / 2) / static_cast<float>(SCR_WIDTH / 2);
	float vertices[] = {
		-1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
		ratio, -1.0f, -0.0f,  1.0f, 1.0f,
		ratio,  1.0f, -0.0f,  1.0f, 0.0f,
		ratio,  1.0f, -0.0f,  1.0f, 0.0f,
		-1.0f, 1.0f, -0.0f,  0.0f, 0.0f,
		-1.0f, -1.0f, -0.0f,  0.0f, 1.0f
	};
	float ratio_people_width = 600.0f / static_cast<float>(SCR_WIDTH / 2);
	float ratio_people_height = 600.0f / static_cast<float>(SCR_HEIGHT / 2);
	float vertices_people[] = {
		ratio, 1.0f - ratio_people_height, -0.0f,  0.0f, 1.0f,
		ratio + ratio_people_width, 1.0f - ratio_people_height, -0.0f,  1.0f, 1.0f,
		ratio + ratio_people_width,  1.0f, -0.0f,  1.0f, 0.0f,
		ratio + ratio_people_width,  1.0f, -0.0f,  1.0f, 0.0f,
		ratio, 1.0f, -0.0f,  0.0f, 0.0f,
		ratio, 1.0f - ratio_people_height, -0.0f,  0.0f, 1.0f
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_people), vertices_people, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ourShader.use();
	ourShader.setInt("texture1", 0);
	return 0;
}


int Display::display(cv::Mat img,cv::Mat people)
{
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, people.cols, people.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, people.data);
    // -----------
    if (!glfwWindowShouldClose(window))
    {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);
        glm::mat4 model;
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(VAO1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		ourShader.use();
		glm::mat4 projection1;
		ourShader.setMat4("projection", projection1);
		glm::mat4 view1;
		ourShader.setMat4("view", view1);
		glm::mat4 model1;
		ourShader.setMat4("model", model1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//    lastX = xpos;
//    lastY = ypos;
//
//    float sensitivity = 0.1f; // change this value to your liking
//    xoffset *= sensitivity;
//    yoffset *= sensitivity;
//
//    yaw += xoffset;
//    pitch += yoffset;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (pitch > 89.0f)
//        pitch = 89.0f;
//    if (pitch < -89.0f)
//        pitch = -89.0f;
//
//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    cameraFront = glm::normalize(front);
//}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (flag == 1)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		float x_delta = xoffset / sqrt((xoffset*xoffset + yoffset*yoffset));
		float y_delta = yoffset / sqrt((xoffset*xoffset + yoffset*yoffset));

		float cameraSpeed_x = 2.5 * 0.1 * x_delta*0.2;
		float cameraSpeed_y = 2.5 * 0.1 * y_delta*0.2;

		if (xoffset < 0)   //����
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * fabs(cameraSpeed_x);
		}
		if (xoffset > 0)
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * fabs(cameraSpeed_x);
		}
		if (yoffset < 0)
		{
			cameraPos += fabs(cameraSpeed_y) * cameraUp;
		}
		if (yoffset > 0)
		{
			cameraPos -= fabs(cameraSpeed_y) * cameraUp;
		}
	}
	
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset*2;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		flag = 1;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		break;
	default:
		return;
	}
	if (action == GLFW_RELEASE) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		flag = 0;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		break;
	default:
		return;
	}
	return;
}