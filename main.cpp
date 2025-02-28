//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
	glm::vec3(-30.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f));
float cameraSpeed = 0.5f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D scena;
gps::Model3D masina;
gps::Model3D pasare;
gps::Model3D minge;
gps::Model3D vantp;
gps::Model3D vantr;
gps::Model3D lightCube;
gps::Model3D screenQuad;

gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//car
float carMove = 0.0f;

//park light
float con = 1.0f;
float lin = 0.7f;
float quad = 1.8f;
glm::vec3 posLight1 = glm::vec3(-28.0, 5.0, 14.0);
GLuint posLocLight1;
glm::vec3 posLight2 = glm::vec3(-12.0, 5.0, 26.0);
GLuint posLocLight2;
GLuint conLoc;
GLuint linLoc;
GLuint quadLoc;

//fog
float fogDensity = 0.000f;
GLuint fogDensityLoc;

//animation
bool startAnimation = false;
int animationSpace = 0;
GLfloat angle;

//bird
float circleRadius = 500.0f;
float circleSpeed = 0.01f;  
float currentAngle = 0.0f;

//football
float rotateBall=0.0f;
float moveBall=0.0f;

//chestie vant 
float rotationAngle = 0.0f;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
	myBasicShader.useShaderProgram();
	glViewport(0, 0, retina_width, retina_height);
	float aspectRatio = (float)retina_width / (float)retina_height;
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	//TODO
	static double lastX = xpos;
	static double lastY = ypos;

	float deltaX = xpos - lastX;
	float deltaY = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;

	deltaX *= sensitivity;
	deltaY *= sensitivity;

	myCamera.rotate(deltaX, deltaY);

}

float yawSpeed = 1.0f;

void makeAnimation()
{
	if (startAnimation)
	{ 
		float animationSpeed = 0.1f;
		if (startAnimation) {
			if (animationSpace < 50)
			{
				myCamera.move(gps::MOVE_FORWARD, animationSpeed);
				animationSpace += 1.0f;
			}
			if (animationSpace < 250) {
				myCamera.move(gps::MOVE_RIGHT, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 550)
			{
				myCamera.move(gps::MOVE_LEFT, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 600)
			{
				myCamera.move(gps::MOVE_RIGHT, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 900)
			{

				myCamera.move(gps::MOVE_FORWARD, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 1100) 
			{
				myCamera.move(gps::MOVE_RIGHT, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 1300)
			{
				myCamera.move(gps::MOVE_LEFT, animationSpeed);
				animationSpace += 1.0f;
			}
			else if (animationSpace < 1650)
			{

				myCamera.move(gps::MOVE_BACKWARD, animationSpeed);
				animationSpace += 1.0f;
			}
			else {
				animationSpace = 0;
			}
			
		}
	}
	else
	{
		animationSpace = 0;
	}
}


void processMovement() {

	//move light direction left
	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	//move light direction right
	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDir = glm::vec3(lightRotation * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));

	//move car front
	if (pressedKeys[GLFW_KEY_UP]) {
			carMove+= 1.0;
	}

	//move car back
	if (pressedKeys[GLFW_KEY_DOWN]) {
			carMove -= 1.0;
	}

	//move camera to the front
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);

		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	//move camera tothe back
	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);

		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	//move camera to the left
	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);

		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	//move camera to the right
	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);

		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	// Rotate camera down
	if (pressedKeys[GLFW_KEY_Q]) {
		myCamera.rotate(-yawSpeed, 0.0f);  
	}

	// Rotate camera up
	if (pressedKeys[GLFW_KEY_E]) {
		myCamera.rotate(yawSpeed, 0.0f);
	}

	// Rotate camera left
	if (pressedKeys[GLFW_KEY_X]) {
		myCamera.rotate(0.0f, yawSpeed); 
	}

	// Rotate camera right
	if (pressedKeys[GLFW_KEY_Z]) {
		myCamera.rotate(0.0f, -yawSpeed); 
	}
	
	// solid mode
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//wireframe mode
	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//polygonal mode
	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	//smooth mode
	if (pressedKeys[GLFW_KEY_4]) {
		glShadeModel(GL_SMOOTH);
	}

	//make fog stronger
	if (pressedKeys[GLFW_KEY_9])
	{
		if (fogDensity < 0.01)
			fogDensity += 0.001f;

	}

	//make fog lighter
	if (pressedKeys[GLFW_KEY_0])
	{
		if (fogDensity > 0)
			fogDensity -= 0.001f;

	}

	//start presentation animation
	if (pressedKeys[GLFW_KEY_ENTER])
	{
		startAnimation = true;
	}

	//stop presentation animation
	if (pressedKeys[GLFW_KEY_BACKSPACE])
	{
		startAnimation = false;
	}

	//rotate ball front
	if (pressedKeys[GLFW_KEY_Y]) {
		moveBall += 0.5;
		rotateBall -= 2.0;
	}

	//rotate ball back
	if (pressedKeys[GLFW_KEY_H]) {
		moveBall -= 0.5;
		rotateBall += 2.0;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}


void initSkybox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/posx.jpg");
	faces.push_back("skybox/negx.jpg");
	faces.push_back("skybox/posy.jpg");
	faces.push_back("skybox/negy.jpg");
	faces.push_back("skybox/posz.jpg");
	faces.push_back("skybox/negz.jpg");
	mySkyBox.Load(faces);
}

void initModels() {
	scena.LoadModel("models/orasnou/scena3.obj");
	masina.LoadModel("models/masina/masina.obj");
	pasare.LoadModel("models/pasare/pasare.obj");
	minge.LoadModel("models/minge/minge.obj");
	vantr.LoadModel("models/vant/rosterup.obj");
	vantp.LoadModel("models/vant/rosterbase.obj");
	lightCube.LoadModel("models/cube/cube.obj");
	screenQuad.LoadModel("models/quad/quad.obj");
}

void initShaders() {
	myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	myBasicShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(0.4f, 0.2f, 0.1f); // Warm yellow light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));


	//park light
	conLoc = glGetUniformLocation(myBasicShader.shaderProgram, "con");
	glUniform1f(conLoc, con);
	linLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lin");
	glUniform1f(linLoc, lin); 
	quadLoc = glGetUniformLocation(myBasicShader.shaderProgram, "quad");
	glUniform1f(quadLoc, quad);
	posLocLight1 = glGetUniformLocation(myBasicShader.shaderProgram, "posLight1");
	glUniform3fv(posLocLight1, 1, glm::value_ptr(posLight1));

	posLocLight2 = glGetUniformLocation(myBasicShader.shaderProgram, "posLight2");
	glUniform3fv(posLocLight2, 1, glm::value_ptr(posLight2));

	fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, fogDensity);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	// TODO - Return the light-space transformation matrix
	glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 transformedLightDir = glm::vec3(lightRotation * glm::vec4(lightDir, 0.0f));

	// Calculate the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(transformedLightDir * 30.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat orthoSize = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 1.0f, 100.0f);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {

	//draw scene

	shader.useShaderProgram();

	model = glm::mat4(1.0f);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scena.Draw(shader);

	// draw car

	model = glm::mat4(1.0f); 
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.07f)); 
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f+carMove));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	// Draw the scaled and repositioned car
	masina.Draw(shader);

	// draw bird
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.01f)); //fac pasarea mai mica
	model = glm::translate(model, glm::vec3(0.0f, 500.0f, 500.0f)); //mut pasarea putin mai in fata

	//facem transformari pentru a face pasarea sa se miste in cerc in continu
	currentAngle += circleSpeed;
	model = glm::translate(model, glm::vec3(circleRadius * glm::cos(currentAngle), 5.0f, circleRadius * glm::sin(currentAngle)));
	model = glm::rotate(model, -currentAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	pasare.Draw(shader);

	//minge
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));
	model = glm::translate(model, glm::vec3(-230.0f , 30.0f, -2.4f));

	model = glm::translate(model, glm::vec3(0.0f + moveBall, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotateBall), glm::vec3(0.0f, 1.0f, 0.0f)); 

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	minge.Draw(shader);

	//chestie de vant
	rotationAngle += 0.1f;
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.5f));
	model = glm::translate(model, glm::vec3(-6.0f, 0.2f, -4.0f));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	vantp.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.5f));
	model = glm::translate(model, glm::vec3(-6.0f, 0.2f, -4.0f));

	model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	vantr.Draw(shader);
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map

	//glBindFramebuffer(GL_FRAMEBUFFER, 1);
	makeAnimation();
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myBasicShader.useShaderProgram();

		glUniform1f(fogDensityLoc, fogDensity);

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
		drawObjects(myBasicShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 50.0f * lightDir);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		mySkyBox.Draw(skyboxShader, view, projection);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initModels();
	initSkybox();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
