# OpenGL

### Assignment 1

- 只用改 main.cpp 与 primitive.h 两个文件

### Assignment 2

- src 里面的文件基本上都要进行修改
- 还得修改一个Shader文件 csucg_assignment2.glsl，实现 X-ray 效果

ps:**将解压出来的文件build了之后，main.cpp 可能会报错，如下**

```cpp
 csugl::Time::init();
 camera2D.size -= glm::vec2{ ev.vertical * csugl::Time::deltaTime() } *camera_zoom_speed;
 float deltaTime = csugl::Time::deltaTime();
 csugl::Time::update();
```

**改为**

```cpp
csugl::LowpTime::init();
camera2D.size -= glm::vec2{ ev.vertical * csugl::LowpTime::deltaTime() } *camera_zoom_speed;
float deltaTime = csugl::LowpTime::deltaTime();
csugl::LowpTime::update();
```
### 就是将所有的 Time 改为 LowpTime 
