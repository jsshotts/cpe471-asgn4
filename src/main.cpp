/*
Jason Shotts
CPE 471
Asgn4: Alduin
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0, -5);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0, yangle = 0;

		if (w == 1)
			speed = 1*ftime;
		else if (s == 1)
			speed = -1*ftime;

		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;

		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	std::shared_ptr<Program> prog;
	GLuint VertexArrayID, VertexArrayID2;
	GLuint VertexBufferID, VertexTexIDBox, VertexNormIDBox;
	GLuint Texture;
	GLuint Texture2;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
	}

	void mouseCallback(GLFWwindow* window, int button, int action, int mods) { return; }
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	vec3 mkNormal(vec3 p1, vec3 p2, vec3 p3)
	{
		vec3 a = vec3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
		vec3 b = vec3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
		return cross(a, b);
	}

	void initBuffers()
	{
		unsigned int v, i, j;
		std::vector<float> verticies, texCords, normals;

		for (v = 0; v < shape->eleBuf[0].size(); v++) {
			if (v != 0 && v % 3 == 0)
			{
				vec3 normal = mkNormal(vec3(verticies[3 * (v - 3) + 0], verticies[3 * (v - 3) + 1], verticies[3 * (v - 3) + 2]),
									   vec3(verticies[3 * (v - 2) + 0], verticies[3 * (v - 2) + 1], verticies[3 * (v - 2) + 2]),
									   vec3(verticies[3 * (v - 1) + 0], verticies[3 * (v - 1) + 1], verticies[3 * (v - 1) + 2]));
				//each vertex must have same normal
				for (i = 0; i < 3; i++)
					for (j = 0; j < 3; j++)
						normals.push_back(normal[j]);
			}
			verticies.push_back(shape->posBuf[0][3 * shape->eleBuf[0][v] + 0]);
			verticies.push_back(shape->posBuf[0][3 * shape->eleBuf[0][v] + 1]);
			verticies.push_back(shape->posBuf[0][3 * shape->eleBuf[0][v] + 2]);

			texCords.push_back(shape->texBuf[0][2 * shape->eleBuf[0][v] + 0]);
			texCords.push_back(shape->texBuf[0][2 * shape->eleBuf[0][v] + 1]);
		}
		if (v != 0 && v % 3 == 0)
		{
			vec3 normal = mkNormal(vec3(verticies[3 * (v - 3) + 0], verticies[3 * (v - 3) + 1], verticies[3 * (v - 3) + 2]),
								   vec3(verticies[3 * (v - 2) + 0], verticies[3 * (v - 2) + 1], verticies[3 * (v - 2) + 2]),
								   vec3(verticies[3 * (v - 1) + 0], verticies[3 * (v - 1) + 1], verticies[3 * (v - 1) + 2]));
			//each vertex must have same normal
			for (i = 0; i < 3; i++)
				for (j = 0; j < 3; j++)
					normals.push_back(normal[j]);
		}
		sendVAOtoGPU(1, verticies, texCords, normals); //diamond
		
		//average normals - smooth
		for (v = 0; v < verticies.size(); v += 3)
		{
			vector<float*> normaddr;
			vec3 p1 = {	verticies[v], verticies[v + 1], verticies[v + 2] };
			normaddr.push_back(&normals[v]); //save normal address for p1
			normaddr.push_back(&normals[v + 1]);
			normaddr.push_back(&normals[v + 2]);
			//for each vertex, compare it to every other vertex
			for (i = v+3; i < verticies.size(); i += 3)
			{
				vec3 p2 = { verticies[i], verticies[i + 1], verticies[i + 2] };
				//check if the verticies have the same position
				if (length(p2 - p1) < 1e-10)
				{
					normaddr.push_back(&normals[i]); //save normal address for p2
					normaddr.push_back(&normals[i + 1]);
					normaddr.push_back(&normals[i + 2]);
				}
			}
			//average the normals of the verticies, and asign it back to every vertex
			vec3 resNormal = vec3(0, 0, 0);
			for (j = 0; j < normaddr.size(); j += 3)
			{
				resNormal.x += *normaddr[j];
				resNormal.y += *normaddr[j + 1];
				resNormal.z += *normaddr[j + 2];
			}
			resNormal /= normaddr.size() / 3;
			for (j = 0; j < normaddr.size(); j += 3)
			{
				*normaddr[j] = resNormal.x;
				*normaddr[j + 1] = resNormal.y;
				*normaddr[j + 2] = resNormal.z;
			}
		}
		sendVAOtoGPU(2, verticies, texCords, normals); //smooth
	}

	void sendVAOtoGPU(int id, std::vector<float> &verticies, std::vector<float> &texCords, std::vector<float> & normals)
	{
		if (id == 1)
		{
			//VAO 1
			glGenVertexArrays(1, &VertexArrayID);
			glBindVertexArray(VertexArrayID);
		}
		if (id == 2)
		{
			//VAO 2
			glGenVertexArrays(1, &VertexArrayID2);
			glBindVertexArray(VertexArrayID2);
		}

		//verticies
		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticies.size(), verticies.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//tex coordinates
		glGenBuffers(1, &VertexTexIDBox);
		glBindBuffer(GL_ARRAY_BUFFER, VertexTexIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCords.size(), texCords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//normals
		glGenBuffers(1, &VertexNormIDBox);
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}

	void initTex(const string &resourceDirectory)
	{
		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/alduin.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 2
		str = resourceDirectory + "/sky.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//tex, tex2... sampler in the fragment shader
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");

		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
	}

	void initGeom(const std::string& resourceDirectory)
	{
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/alduin.obj");
		shape->resize();
		shape->init();
		
		initBuffers();
		initTex(resourceDirectory);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertTex");
		prog->addAttribute("vertNor");
	}

	void render()
	{
		double frametime = get_last_elapsed_time();

		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform3f(prog->getUniform("campos"), -mycam.pos.x, -mycam.pos.y, -mycam.pos.z);

		static float w = 0.0;
		w += 0.4 * frametime; //rotation angle
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), (float)-M_PI/2, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), -w, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 3.0f));
		M = Trans * RotateY * RotateX;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glDrawArrays(GL_TRIANGLES, 0, 144666);

		RotateX = glm::rotate(glm::mat4(1.0f), (float)-M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 3.0f));
		M = Trans * RotateY * RotateX;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID2);
		glDrawArrays(GL_TRIANGLES, 0, 144666);

		glBindVertexArray(0);
		prog->unbind();
	}
};
/******************************************************************************************/
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources";
	if (argc >= 2)
		resourceDir = argv[1];

	Application *application = new Application();
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1000, 800);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init(resourceDir);
	application->initGeom(resourceDir);

	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();
		glfwSwapBuffers(windowManager->getHandle());
		glfwPollEvents();
	}
	windowManager->shutdown();
	return 0;
}
