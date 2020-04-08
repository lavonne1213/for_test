#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <iostream>

//����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//��ɫ��Դ��
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n" //λ�ñ���������λ��Ϊ0
	"layout (location = 1) in vec3 aColor;\n" //��ɫ����������λ��Ϊ1
	"out vec3 ourColor;\n" //��Ƭ����ɫ�����һ����ɫ

	//"out vec4 vertexColor;\n" // out
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos, 1.0);\n"
	//"	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);" //RGBA, Alpha͸����
	"	ourColor = aColor;\n" //��ourColor����Ϊ�Ӷ������ݵõ���������ɫ
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	//"in vec4 vertexColor;\n"  //in
	"in vec3 ourColor;\n"

	//"uniform vec4 ourColor;\n" //ʹ��uniform����Ƭ����ɫʱ
	"void main()\n"
	"{\n"
	//"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" //ֱ�Ӹ�ֵ��ǰ��û�������
	//"   FragColor = vertexColor;\n" //��ʱ�������vertexColor
	//"   FragColor = ourColor;\n" //��ʱuniform�����ݻ��ǿյģ�Ҫ��whileѭ�����������
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

	//glad: load all OpenGL function pointers��glad����������OpenGL����ָ���
	//-----------------------------------------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialized glad" << std::endl;
		return -1;
	}
	
	// build and compile our shader program
	// ------------------------------------------------------------------------------
	//vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);//���ȴ���һ����ɫ������,��ID�����õ�
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);//�������ɫ��Դ�븽�ӵ���ɫ��������
	glCompileShader(vertexShader);//Ȼ�������

	//check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//����Ƿ����ɹ�
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
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, //����
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, //bottom right
		 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f  //top
	};//NDC, Normalized Device Coordinates
	/*���������Ķ��������Ժ����ǻ������Ϊ���뷢�͸�ͼ����Ⱦ���ߵĵ�һ������׶Σ�������ɫ����
	������GPU�ϴ����ڴ����ڴ������ǵĶ������ݣ���Ҫ����OpenGL��ν�����Щ�ڴ棬����ָ������η���
	���Կ���������ɫ�����Żᴦ���������ڴ���ָ�������Ķ��㡣*/
	

	unsigned int VAO, VBO;//ͨ�����㻺�����(Vertex Buffer Objects)��������ڴ棬������GPU�ڴ棨�Դ棩�д����������
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// 1.bind the Vertex Array Object first.�Ȱ󶨶����������
	glBindVertexArray(VAO); 

	//2.then bind and set vertex buffer(s), �Ѷ������鸴�Ƶ������й�OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//�������ݲ���򼸺�����ı�

	//3. and then configure vertex attributes(s), ���ö�������ָ��
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//����OpenGL����ν�����������
	glEnableVertexAttribArray(0);
	//��Ϊ�������һ���������ԣ���������VBO���ڴ棬����Ҫ�������ö�������ָ��,����Ҫ��3*���6*
	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);

	// as we only have a single shader, we could also just activate our shader once beforehand if we want to 
	glUseProgram(shaderProgram); //������˶���ͷ���whileѭ����


	//ϣ�����������������ر���֮ǰ���ϻ���ͼ���ܹ������û����롣��ˣ�������Ҫ�ڳ��������һ��whileѭ����
	//���ǿ��԰�����֮Ϊ��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);

		//render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// be sure to activate the shader before any calls to glUniform		
		//glUseProgram(shaderProgram); //��������������,��Ȼǰ��Ҫ������ɫ������

		// update shader uniform
		/*float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		// render the triangle
		glBindVertexArray(VAO);
		//����ĺ�����ʹ�õ�ǰ�������ɫ����֮ǰ����Ķ����������ú�VBO�Ķ������ݣ�ͨ��VAO��Ӱ󶨣�������ͼԪ
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

//��GLFW��ʵ��������ƿ�ͨ��GLFW��glfwGetKey����,һ�����ڼ�������Ϊ����,������������Ƿ����ڱ�����
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//���û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ����������ԶԴ���ע��һ���ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}