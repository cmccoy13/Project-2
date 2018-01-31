/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>
#include <cstdlib>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "vector"

#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint IndexBufferID;
	GLuint VertexBufferColor;
	double posX = 0;
	double posY = 0;
	float rad = 1.0;
	float numVert = 80;
	float sc = 1.0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	float getParametricX(float angle, float radius)
	{
		return radius*cos(angle);
	}

	float getParametricY(float angle, float radius)
	{
		return radius*sin(angle);
	}

	std::vector<float> circleVertices(float numVertices, float radius)
	{
		std::vector<float> vertices;
		vertices.push_back(0.0);
		vertices.push_back(0.0);
		vertices.push_back(0.0);

		for (int i = 0; i < numVertices; i++)
		{
			float angle = (3.14159*2) / numVertices;

			if (i == 1 || i % 2 == 1)
			{
				vertices.push_back(getParametricX(i*angle, radius * .95));
				vertices.push_back(getParametricY(i*angle, radius * .95));
				vertices.push_back(1.0);
			}
			else
			{
				vertices.push_back(getParametricX(i*angle, radius));
				vertices.push_back(getParametricY(i*angle, radius));
				vertices.push_back(0.0);
			}			
			//vertices.push_back(getParametricY(i*angle, radius));
		}

		return vertices;
	}

	std::vector<int> circleIndices(float numVertices)
	{
		std::vector<int> indices;

		for (int i = 1; i < numVertices; i++)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		indices.push_back(0);
		indices.push_back(numVertices);
		indices.push_back(1);

		return indices;
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			/*glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);*/
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//SUN
		std::vector<float> vertices = circleVertices(numVert, rad);
		std::vector<int> indices = circleIndices(numVert);

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glDeleteBuffers(1, &IndexBufferID);

		glBindVertexArray(0);
		//END SUN
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(1, 1, 1, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("Scale");
		//prog->addUniform("Click");
		//prog->addUniform("WindowHeight");
		//prog->addUniform("WindowWidth");
		prog->addUniform("Center");
		prog->addAttribute("vertPos");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		static float sc = 1;
		static int flag = 0;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		glm::mat4 V, M, PP;
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection.
		PP = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
		{
		PP = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
		}

		if (flag == 0)
		{
			sc -= 0.01;
		}
		else
		{
			sc += 0.01;
		}

		if (sc <= 0.25)
		{
			flag = 1;
		}
		if (sc >= 1)
		{
			flag = 0;
		}


		prog->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &PP[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1f(prog->getUniform("Scale"), sc);
		glUniform4f(prog->getUniform("Center"), width / 2, height / 2, 0, 0);
		glBindVertexArray(VertexArrayID);

		//actually draw from vertex 0, 3 vertices
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		glDrawElements(GL_TRIANGLES, numVert * 3, GL_UNSIGNED_INT, (void *)0);
		// end sun
		glBindVertexArray(0);

		prog->unbind();
	}
	float p2wX(float xp)
	{
		return xp / 320 - 1;
	}

	float p2wY(float yp)
	{
		return -(yp / 240 - 1);
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
