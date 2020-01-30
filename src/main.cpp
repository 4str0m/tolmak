#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <common.h>
#include <orbit_camera.h>
#include <mesh_io.h>
#include <mesh.h>
#include <materials.h>
#include <light.h>


OrbitCamera camera = {
	glm::vec3(0.f,  0.f, 0.f),
	0.f, 0.f,
	10.f,
	glm::radians(45.f),
	1.f,
	0.1f, 100.f
};

PointLight point_light = {
    glm::vec3(0.f, 4.f, 0.f),
    glm::vec3(0.9f, .8f, 0.9f)
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
    GLCall(glFrontFace(GL_CCW));
    GLCall(glCullFace(GL_BACK));
    GLCall(glEnable(GL_CULL_FACE));
    // GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

    PhongMaterial phong;
    create_phong_material(phong,
        "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_COL_3K.jpg",
        "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_REFL_3K.jpg",
        "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_NRM_3K.jpg");

    Mesh mesh;
    {
        MeshData mesh_data;
        load_obj("../resources/meshes/cube.obj", mesh_data);
        // load_obj("../resources/meshes/monkey.obj", mesh_data);
        mesh_from_mesh_data(mesh_data, mesh);
    }

    VertexAttribs attribs;
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 2, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_enable_all(attribs, mesh);

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

            ImGui::SliderFloat3("Light pos", (float*)&point_light.pos, -10.f, 10.f);
            ImGui::ColorEdit3("Light color", (float*)&point_light.color);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        phong_material_imgui(phong);
        // Rendering
        ImGui::Render();

        camera.aspect_ratio = aspect_ratio;
        glm::mat4 projection = get_projection_matrix(camera);
        glm::mat4 view = get_view_matrix(camera);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = projection * view * model;

        use_material(phong, mvp, model, camera._eye, point_light);
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