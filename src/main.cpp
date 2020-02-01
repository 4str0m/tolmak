
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <common.h>
#include <orbit_camera.h>
#include <mesh_io.h>
#include <mesh.h>
#include <materials.h>
#include <light.h>
#include <game_object.h>

OrbitCamera camera = {
	glm::vec3(0.f,  0.f, 0.f),
	0.f, 0.f,
	10.f,
	glm::radians(45.f),
	1.f,
	0.1f, 100.f
};

static MouseState mouse_state;
static KeyboardState keyboard_state;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    keyboard_state.action[key] = action;
    keyboard_state.mods[key] = mods;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse_state.dx = xpos - mouse_state.x;
	mouse_state.dy = ypos - mouse_state.y;

	mouse_state.x = xpos;
	mouse_state.y = ypos;
	
	camera_handle_mouse_move(camera, mouse_state);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	mouse_state.action[button] = action;
    mouse_state.mods[button] = mods;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
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
    
    lights_init();
 
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glFrontFace(GL_CCW));
    GLCall(glCullFace(GL_BACK));
    GLCall(glEnable(GL_CULL_FACE));
    // GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

    PhongMaterial phong;
    material_create(phong);
    texture_load(&phong.diff_tex, "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_COL_3K.jpg");
    texture_load(&phong.spec_tex, "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_REFL_3K.jpg");
    texture_load(&phong.bump_tex, "../resources/StoneBricksBeige015/REGULAR/3K/StoneBricksBeige015_NRM_3K.jpg");

    PlainColorMaterial plain_color;
    material_create(plain_color);

    PlainColorMaterial uid_mat;
    material_create(uid_mat);

    const uint32_t sphere_count = 100;
    GameObject spheres[sphere_count];
    glm::vec3 uid_colors[sphere_count];
    glm::mat4 models[sphere_count];
    float outline_size = .05f;
    for (uint32_t i = 0; i < sphere_count; ++i)
    {
        game_object_create(spheres[i], "../resources/meshes/sphere_hres.obj");
        uid_colors[i] = glm::vec3(
            spheres[i].uid / 256.f,
            (spheres[i].uid >> 8) / 256.f,
            (spheres[i].uid >> 16) / 256.f
        );
        models[i] = glm::translate(glm::mat4(1.0f), glm::vec3(i%10 - 5.f, 0, i/10 - 5.f) * 3.f);
    }

    uint32_t FBO;
    GLCall(glGenFramebuffers(1, &FBO));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));

    uint32_t color_buffer;
    GLCall(glGenTextures(1, &color_buffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, color_buffer));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2000, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0));

    unsigned int RBO;
    GLCall(glGenRenderbuffers(1, &RBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2000, 1200));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO));

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        exit(EXIT_FAILURE);
    }
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    uint32_t mouse_pixel_color;
    GLCall(glGenTextures(1, &mouse_pixel_color));
    GLCall(glBindTexture(GL_TEXTURE_2D, mouse_pixel_color));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        camera_update(camera, keyboard_state);

        float aspect_ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        aspect_ratio = width / (float) height;
 
        float time = (float)0.f; //glfwGetTime();
        float r = 3.f;
        point_lights[0].pos = glm::vec3(r * sin(time), 0.f, r * cos(time));
        point_lights[1].pos = glm::vec3(r * sin(time + PI), 0.f, r * cos(time + PI));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

         // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("General Infos");                          // Create a window called "Hello, world!" and append into it.

            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
            ImGui::SliderFloat("outline size", &outline_size, 0.f, .3f); // Edit 3 floats representing a color

            static int light_index = 0;
            ImGui::SliderInt("Light index", &light_index, 0, N_POINT_LIGHTS);
            ImGui::SliderFloat3("Light pos", (float*)&point_lights[light_index].pos, -10.f, 10.f);
            ImGui::ColorEdit3("Light color", (float*)&point_lights[light_index].color);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        material_imgui(phong);
        material_imgui(plain_color);
        // Rendering
        ImGui::Render();

        camera.aspect_ratio = aspect_ratio;
        glm::mat4 projection = get_projection_matrix(camera);
        glm::mat4 view = get_view_matrix(camera);
        glm::mat4 vp = projection * view;

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
        GLCall(glViewport(0, 0, width, height));
        GLCall(glClearColor(1.f, 1.f, 1.f, 1.f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        GLCall(glEnable(GL_DEPTH_TEST));

        for (uint32_t i = 0; i < sphere_count; ++i)
        {
            uid_mat.tint = uid_colors[i];
            game_object_draw(spheres[i], uid_mat, vp * models[i], models[i], camera._eye);
        }
        glm::vec3 mouseRGB(0.f);
        GLCall(glBindTexture(GL_TEXTURE_2D, mouse_pixel_color));
        GLCall(glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mouse_state.x, height - mouse_state.y, 1, 1, 0));
        GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, (float*)&mouseRGB));

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, width, height));
        GLCall(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
        GLCall(glStencilMask(0xFF));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        GLCall(glEnable(GL_DEPTH_TEST));

        GLCall(glEnable(GL_STENCIL_TEST));
        GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

        GLCall(glStencilMask(0xFF));
        for (uint32_t i = 0; i < sphere_count; ++i)
        {
            game_object_draw(spheres[i], phong, vp * models[i], models[i], camera._eye);
        }

        GLCall(glDisable(GL_DEPTH_TEST));
        GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
        GLCall(glStencilMask(0x00));

        const float threshold = .5f / 256.f;
        for (uint32_t i = 0; i < sphere_count; ++i)
        {
            glm::vec3 uid = glm::abs(mouseRGB - uid_colors[i]);
            if (uid.x < threshold && uid.y < threshold && uid.z < threshold) {
                glm::mat4 model_scale = glm::scale(models[i], glm::vec3(1.f + outline_size));
                game_object_draw(spheres[i], plain_color, vp * model_scale, model_scale, camera._eye);
            }
        }

        GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCall(glDisable(GL_STENCIL_TEST));

        lights_draw(vp);

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