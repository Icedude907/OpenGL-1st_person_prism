#pragma once

#include <vkfw/vkfw.hpp>
#include "frametimer.h"
#include "glload.h"
#include "../input/input.h"
#include "../util/util.h"

// Interfaces with gl/vkfw
// Also loads gl3w functions
namespace Render{
    namespace callbacks{
        namespace vkfw{
            using namespace ::vkfw;
            using namespace ::Input;
            void keyboard(const Window& window, Key key, int32_t scancode, KeyAction action, ModifierKeyFlags modifiers){
                if(action == vkfw::KeyAction::Press){
                    inputSystem.buttonPress(key);
                }else if(action == vkfw::KeyAction::Release){
                    inputSystem.buttonRelease(key);
                }
            }
            void mousebutton(const Window& window, MouseButton button, MouseButtonAction action, ModifierKeyFlags modifiers){
                if(action == MouseButtonAction::Press){
                    inputSystem.buttonPress(button);
                }else if(action == MouseButtonAction::Release){
                    inputSystem.buttonRelease(button);
                }
            }
            void mousemove(const Window& window, double x, double y){
                inputSystem.mouseMove(x, y);
            }
        }
    }

    struct Screen{
        size_t width, height;
        bool shouldRender;

        vkfw::UniqueInstance glfwInstance;
        vkfw::UniqueWindow window;

        Render::FrameTimer frameTimes;

        bool init(char const* title, size_t width, size_t height, GLFWerrorfun error_callback = default_error_callback){
            this->width = width;
            this->height = height;
            glfwInstance = vkfw::initUnique(); // Unique automatically deallocates
            vkfw::WindowHints hints;
                hints.clientAPI = vkfw::ClientAPI::OpenGL;
                hints.contextVersionMajor = 4u;
                hints.contextVersionMinor = 6u;
                hints.openGLProfile = vkfw::OpenGLProfile::Core;
            window = vkfw::createWindowUnique(width, height, title, hints);
            vkfw::setErrorCallback(error_callback);
            if(!window){
                std::cout << "Window Creation Failed!" << std::endl;
                return false;
            }
            window->makeContextCurrent();

            GL::load();

            resizeViewport();
            updateShouldRender();

            vkfwSetupCallbacksForWindow();

            return true;
        }
        inline void resize(size_t newWidth, size_t newHeight){
            width = newWidth;
            height = newHeight;
            resizeViewport();
            updateShouldRender();
        }
        inline void present(){
            window->swapBuffers();
        }
        inline void setMouseCapture(vkfw::CursorMode val){
            window->set<vkfw::InputMode::Cursor>(val);
        }
        inline void moveMouse(double x, double y){
            window->setCursorPos(x, y);
        }
        private:
        inline void updateShouldRender(){ shouldRender = !(width == 0 || height == 0); }
        inline void resizeViewport(){ glViewport(0, 0, (GLsizei)this->width, (GLsizei)this->height); }
        static void default_error_callback(int error_code, const char* description){
            std::cerr << "Error: " << description << std::endl;
        }

        void vkfwSetupCallbacksForWindow(){
            using namespace std::placeholders;
            auto cb = window->callbacks();
            cb->on_key = callbacks::vkfw::keyboard;
            if(vkfw::rawMouseMotionSupported()){
                window->set<vkfw::InputMode::RawMouseMotion>(true);
            }
            cb->on_mouse_button = callbacks::vkfw::mousebutton;
            cb->on_cursor_move = callbacks::vkfw::mousemove;
        }
    };
}