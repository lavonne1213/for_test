#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <iostream>

//声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//着色器源码
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n" //位置变量的属性位置为0
	"layout (location = 1) in vec3 aColor;\n" //颜色变量的属性位置为1
	"out vec3 ourColor;\n" //向片段着色器输出一个颜色

	//"out vec4 vertexColor;\n" // out
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos, 1.0);\n"
	//"	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);" //RGBA, Alpha透明度
	"	ourColor = aColor;\n" //将ourColor设置为从顶点数据得到的输入颜色
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	//"in vec4 vertexColor;\n"  //in
	"in vec3 ourColor;\n"

	//"uniform vec4 ourColor;\n" //使用uniform传递片段颜色时
	"void main()\n"
	"{\n"
	//"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" //直接赋值，前面没标记输入
	//"   FragColor = vertexColor;\n" //此时输入的是vertexColor
	//"   FragColor = ourColor;\n" //此时uniform里数据还是空的，要在while循环里更新数据
	"	FragColor = vec4(ourColor, 1.0);"
	"}\n\0";


int main()
{
	// glfw: initialize and configure
	//-----------------------------------------------------------------------------------------
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// glfw window creation
	// ----------------------------------------------------------------------------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learnOpengl", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "failed to created glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//glad: load all OpenGL function pointers，glad是用来管理OpenGL函数指针的
	//-----------------------------------------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialized glad" << std::endl;
		return -1;
	}
	
	// build and compile our shader program
	// ------------------------------------------------------------------------------
	//vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);//首先创建一个着色器对象,用ID来引用的
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);//把这个着色器源码附加到着色器对象上
	glCompileShader(vertexShader);//然后编译它

	//check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//检查是否编译成功
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//set up vertex data (and buffer(s)) and configure vertex attributes
	//-------------------------------------------------------------------------
	
	/*float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};//NDC, Normalized Device Coordinates	*/

	float vertices[] = {
		//positions           //colors
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, //左下
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, //bottom right
		 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f  //top
	};//NDC, Normalized Device Coordinates
	/*定义这样的顶点数据以后，我们会把它作为输入发送给图形渲染管线的第一个处理阶段：顶点着色器。
	它会在GPU上创建内存用于储存我们的顶点数据，还要配置OpenGL如何解释这些内存，并且指定其如何发送
	给显卡。顶点着色器接着会处理我们在内存中指定数量的顶点。*/
	

	unsigned int VAO, VBO;//通过顶点缓冲对象(Vertex Buffer Objects)管理这个内存，它会在GPU内存（显存）中储存大量顶点
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// 1.bind the Vertex Array Object first.先绑定顶点数组对象
	glBindVertexArray(VAO); 

	//2.then bind and set vertex buffer(s), 把顶点数组复制到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//顶点数据不会或几乎不会改变

	//3. and then configure vertex attributes(s), 设置顶点属性指针
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//告诉OpenGL该如何解析顶点数据
	glEnableVertexAttribArray(0);
	//因为添加了另一个顶点属性，即更新了VBO的内存，所以要重新配置顶点属性指针,上面要从3*变成6*
	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);

	// as we only have a single shader, we could also just activate our shader once beforehand if we want to 
	glUseProgram(shaderProgram); //如果用了多个就放在while循环里


	//希望程序在我们主动关闭它之前不断绘制图像并能够接受用户输入。因此，我们需要在程序中添加一个while循环，
	//我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行
	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);

		//render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// be sure to activate the shader before any calls to glUniform		
		//glUseProgram(shaderProgram); //激活这个程序对象,当然前面要先有着色器程序

		// update shader uniform
		/*float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		// render the triangle
		glBindVertexArray(VAO);
		//下面的函数会使用当前激活的着色器、之前定义的顶点属性配置和VBO的顶点数据（通过VAO间接绑定）来绘制图元
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	//de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

//在GLFW中实现输入控制可通过GLFW的glfwGetKey函数,一个窗口及按键作为输入,返回这个按键是否正在被按下
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//当用户改变窗口的大小的时候，视口也应该被调整。可以对窗口注册一个回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}