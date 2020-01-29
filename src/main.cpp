#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <common.h>
#include <orbit_camera.h>
#include <mesh_io.h>
#include <mesh.h>
#include <shader.h>
#include <texture.h>


OrbitCamera camera = {
	glm::vec3(0.f,  0.f, 0.f),
	0.f, 0.f,
	10.f,
	glm::radians(45.f),
	1.f,
	0.1f, 100.f
};

static MouseState mouse_state;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse_state.dx = xpos - mouse_state.x;
	mouse_state.dy = ypos - mouse_state.y;

	mouse_state.x = xpos;
	mouse_state.y = ypos;
	
	camera_handle_mouse_move(camera, mouse_state);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	mouse_state.buttons[button] = action == GLFW_PRESS;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera_handle_scroll(camera, xoffset, yoffset);
}

int main(void)
{
    GLFWwindow* window;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    
    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
 
    window = glfwCreateWindow(2000, 1200, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
 
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
 
    GLCall(glEnable(GL_DEPTH_TEST));

    Texture siggraph_tex;
    load_texture("../resources/siggraph.png", siggraph_tex);

    MeshData mesh_data;
    load_obj("../resources/meshes/monkey.obj", mesh_data);

    Mesh mesh;
    mesh_from_mesh_data(mesh_data, mesh);

    GLuint program = load_shader("../resources/shaders/phong.glsl");
 
    GLint mvp_location, m_location;
    mvp_location = glGetUniformLocation(program, "MVP");
    m_location   = glGetUniformLocation(program, "M");

	bind_mesh(mesh);
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), (void*) 0));
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), (void*) (sizeof(glm::vec3))));
    GLCall(glEnableVertexAttribArray(2));
    GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), (void*) (sizeof(glm::vec3) + sizeof(glm::vec2))));

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {      
        float aspect_ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        aspect_ratio = width / (float) height;
 
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

        camera.aspect_ratio = aspect_ratio;
        glm::mat4 projection = get_projection_matrix(camera);
        glm::mat4 view = get_view_matrix(camera);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float) glfwGetTime() / 10.f, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float) glfwGetTime() / 10.f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;

        GLCall(glUseProgram(program));
        GLCall(glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp));
        GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) &model));
        
        bind_texture(siggraph_tex);
        draw_mesh(mesh);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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