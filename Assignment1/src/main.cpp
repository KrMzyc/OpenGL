#include "primitive.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// 鼠标拖动方法
// winSize: 当前窗口大小
// primitive: 当前拖拽的图元
void MouseDragger(const glm::ivec2& winSize, csugl::Ref<Primitive> primitive);

// 窗口坐标转换为NDC坐标
// winPosition: 待转换的坐标
// winSize: 当前窗口大小
// return: 转换后的NDC坐标
glm::vec2 WindowPosToNDCPos(const glm::ivec2& winPosition, const glm::ivec2& winSize);

int main(int argc, char const *argv[]) {
    // glog 初始化
    FLAGS_stderrthreshold = 0;
    google::InitGoogleLogging(argv[0]);

    // 初始化应用程序单例，并获取实例
    auto app = csugl::singleton<csugl::Application>::getInstance();
    
    // 获取窗口
    auto window = &app->GetWindow();
    // 开启垂直同步
    window->SetVSync(true);
    // 获取窗口大小
    glm::ivec2 winSize = window->GetSize();
    // 窗口置顶
    glfwSetWindowAttrib(window->GetGLFWwindow(), GLFW_FLOATING, true);
    // 添加事件回调 (lambda写法)
    window->addEventCallback([&](csugl::Event &ev) {
        csugl::EventDispatcher dispatcher(ev);
        // resize事件(当窗口大小改变时会调用此函数)
        dispatcher.Dispatch<csugl::WindowResizeEvent>([&](csugl::WindowResizeEvent &ev){
            LOG(INFO) << ev;
            // 更新winSize
            winSize = {ev.width, ev.height};
            return false;
        });
    });
    // 重新设置窗口大小
    window->Resize(600, 600);
    
    // 加载shader, 用于绘制最简单的2D图像
    auto shader = csugl::MakeRef<csugl::Shader>("../assets/shader/csucg_assignment1.glsl");

    // 创建一个三角形
    auto triangle = csugl::MakeRef<Triangle>();
    triangle->position = {0.0f, 0.0f};
    triangle->color = {0.8f, 0.5f, 0.1f};
    
    // 创建一个矩形
    auto rectangle = csugl::MakeRef<Rectangle>();
    rectangle->position = {0.0f, 0.0f};
    rectangle->color = {0.8f, 0.5f, 0.1f};

    // 创建一个圆
    auto circle = csugl::MakeRef<Circle>();
    circle->position = {0.0f, 0.0f};
    circle->color = {0.8f, 0.5f, 0.1f};

    // 渲染对象, 默认为三角形
    csugl::Ref<Primitive> renderObj = triangle;

    // 创建两类光标
    auto normalCur = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    auto dragCur = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
    // 默认光标为箭头
    glfwSetCursor(window->GetGLFWwindow(), normalCur);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::LoadIniSettingsFromDisk("../assets/imgui/imgui.ini");

    // 主循环
    while(app->isOpen()) {
        // 设置GL的刷新clear模式
        glClear(GL_COLOR_BUFFER_BIT);
        // 设置刷新颜色
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Input Event");

        // 数字键切换绘制的图形
        if(csugl::Input::IsKeyPressed(csugl::Key::D1)) {        // 1.三角形
            triangle->position = renderObj->position;
            triangle->color = renderObj->color;
            renderObj = triangle;
            ImGui::Text("Key: 'Num1' Pressing...");
        } else if(csugl::Input::IsKeyPressed(csugl::Key::D2)) { // 2.矩形
            rectangle->position = renderObj->position;
            rectangle->color = renderObj->color;
            renderObj = rectangle;
            ImGui::Text("Key: 'Num2' Pressing...");
        } else if(csugl::Input::IsKeyPressed(csugl::Key::D3)) { // 3.圆
            circle->position = renderObj->position;
            circle->color = renderObj->color;
            renderObj = circle;
            ImGui::Text("Key: 'Num3' Pressing...");
        }

        // 设置光标类型
        if(csugl::Input::IsMouseButtonPressed(csugl::Mouse::Button0)) {
            glfwSetCursor(window->GetGLFWwindow(), dragCur);            // 点击
            ImGui::Text("Mouse: 'Button0' Pressing...");
        }
        else
            glfwSetCursor(window->GetGLFWwindow(), normalCur);          // 箭头

        // 鼠标拖拽逻辑
        MouseDragger(winSize, renderObj);

        // 按R重置图元坐标
        if(csugl::Input::IsKeyPressed(csugl::Key::R)) {
            ImGui::Text("Key: 'R' Pressing...");
            renderObj->position = {0.0f, 0.0f};
        }

        // 绘制图元
        renderObj->draw(shader);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 窗口显示（交换缓冲区、处理事件）
        window->Display();
    }

    // 释放资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::SaveIniSettingsToDisk("../assets/imgui/imgui.ini");
    ImGui::DestroyContext();

    glfwDestroyCursor(normalCur);
    glfwDestroyCursor(dragCur);
    csugl::singleton<csugl::Application>::destroy();

    return 0;
}
glm::vec2 temp2;
glm::vec2 temp3;
void MouseDragger(const glm::ivec2& winSize, csugl::Ref<Primitive> primitive) {
    // 鼠标在图元内
    static bool isContained = false;
    // 鼠标正在拖拽
    static bool isDraging = false;
    
    // 1.获取鼠标窗口坐标:
    glm::ivec2 pos;
    
    ImVec2 P =ImGui::GetMousePos();
    ImGui::Text("%f %f", P.x, P.y);
    pos.x = P.x;
    pos.y = P.y;
    ImGui::Text("%d %d", pos.x, pos.y);
    
    // 2.转换为NDC坐标: (使用WindowPosToNDCPos转换)
    glm::vec2 temp=WindowPosToNDCPos(pos, winSize);
    if (ImGui::IsMouseClicked(0)) {
        glm::ivec2 pos1;
        ImVec2 P1 = ImGui::GetMousePos();
        pos1.x = P1.x;
        pos1.y = P1.y;
        temp2 = WindowPosToNDCPos(pos1, winSize);
        temp3 = primitive->position;
    }
    // 3.是否在拖拽:
    bool flag = ImGui::IsMouseDragging(0);     //左键
    // 4.是否在图元内: (使用primitive的is_contained检测)
    bool iscontained = primitive->is_contained(temp);
    
    // 5.设置图元颜色:
    if (flag && iscontained) {
        primitive->color = { 1.0f,0.0f,0.0f };
    }
    else primitive->color = { 0.8f,0.5f,0.1f };
    // 6.设置图元新坐标:
    if (flag && iscontained) {
        ImGui::Text("position change %f %f",temp.x, temp.y);
        primitive->position.x = temp.x-temp2.x+temp3.x;
        primitive->position.y = temp.y-temp2.y+temp3.y;
    }


}

// 将窗口坐标转换为标准设备坐标(NDC)
glm::vec2 WindowPosToNDCPos(const glm::ivec2& winPosition, const glm::ivec2& winSize) {
    glm::vec2 temp;
    temp.x = winPosition.x / (winSize.x / 2.0f) - 1.0f;
    temp.y = -winPosition.y / (winSize.y / 2.0f) + 1.0f;
    return temp;
}