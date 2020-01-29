#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <common.h>
#include <mesh_io.h>
#include <shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 
int main(void)
{
    GLFWwindow* window;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit()) {
    	exit(EXIT_FAILURE);
    }
	
	const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
 
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
    	std::cout << "Failed to initialize GLAD" << std::endl;
    	return -1;
	}
    glfwSwapInterval(1);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
 
    // NOTE: OpenGL error checks have been omitted for brevity

	Mesh cube;
	load_obj("../resources/meshes/cube.obj", cube);


	unsigned int texture;
	GLCall(glGenTextures(1, &texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture));
	// set the texture wrapping/filtering options (on the currently bound texture object)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("../resources/siggraph.png", &width, &height, &nrChannels, 0);
	if (data)
	{
	    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
	    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
	    std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	GLuint VAO, VBO, EBO;

	GLCall(glEnable(GL_DEPTH_TEST));

	GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, cube.vertices.size() * sizeof(Mesh::Vertex), cube.vertices.data(), GL_STATIC_DRAW));

	GLCall(glGenBuffers(1, &EBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indices.size() * sizeof(uint32_t), cube.indices.data(), GL_STATIC_DRAW));
 
    GLuint program = load_shader("../resources/shaders/phong.glsl");
 
    GLint mvp_location, m_location, vpos_location, vuv_location, vn_location;
    mvp_location 	= glGetUniformLocation(program, "MVP");
    m_location 		= glGetUniformLocation(program, "M");
    vpos_location 	= glGetAttribLocation(program, "vPos");
    vuv_location 	= glGetAttribLocation(program, "vUV");
    vn_location 	= glGetAttribLocation(program, "vN");

    GLCall(glEnableVertexAttribArray(vpos_location));
    GLCall(glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*) 0));
    GLCall(glEnableVertexAttribArray(vuv_location));
    GLCall(glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*) (sizeof(glm::vec3))));
    GLCall(glEnableVertexAttribArray(vn_location));
    GLCall(glVertexAttribPointer(vn_location, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*) (sizeof(glm::vec3) + sizeof(glm::vec2))));

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {      
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        GLCall(glViewport(0, 0, width, height));
        GLCall(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

 		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
 
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.f));
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
		model = glm::rotate(view, (float) glfwGetTime(), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(view, (float) glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvp = projection * view * model;

        GLCall(glUseProgram(program));
        GLCall(glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp));
        GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) &model));
		// GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));

		GLCall(glBindTexture(GL_TEXTURE_2D, texture));
        GLCall(glBindVertexArray(VAO));

		GLCall(glDrawElements(
			GL_TRIANGLES,      // mode
			cube.indices.size(),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		));

        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
 
    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}