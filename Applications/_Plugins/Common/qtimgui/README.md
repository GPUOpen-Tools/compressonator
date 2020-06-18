* Copyright(c) 2014 - 2018 Omar Cornut

# QtImGui

Qt (QOpenGLWidget / QOpenGLWindow) backend for [ImGui](https://github.com/ocornut/imgui)

## How to use

* Add QtImGui sources and headers to your project
    * If you are using git submodule, run `git submodule update --init --recursive` to ensure that the inner submodule is initialized as well.
* Add `include(path/to/qtimgui.pri)` to youre `.pro` file
* Subclass `QOpenGLWindow` or `QOpenGLWidget` and:

```cpp
class DemoWindow : public QOpenGLWindow
{
protected:
    void initializeGL() override
    {
        QtImGui::initialize(this);
    }
    void paintGL() override
    {
        // you can do custom GL rendering as well in paintGL

        QtImGui::newFrame();

        ImGui::Text("Hello");
        // more widgets...

        ImGui::Render();
    }
};
```
