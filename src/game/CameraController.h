#pragma once

#include <vkfw/vkfw.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../render/camera.h"
#include "../render/screen.h"
#include "../input/input.h"

/*
Screen -> Display
Screen becomes bound to a camera and holds drawing?
*/
namespace Game{
    struct CameraController{
        // References
        Render::Camera& camera;
        Render::Screen& screen;

        // Config
        float speed = 2.0f; // units/s
        float sensitivity = 100.0f;

        // Internal
        glm::vec3 camMoveVec = glm::vec3(0.f,0.f,0.f); // Relative to current pos
        glm::vec3 camRotVec = glm::vec3(0.f,0.f,0.f);
        bool captureMouse = false;
        double prevX, prevY; // Tracks mouse

        inline CameraController(Render::Camera& camera, Render::Screen& screen)
        : camera(camera), screen(screen){}

        void applyFrameMovement(){
            camera.rot += camRotVec * (float)screen.frameTimes.delta;
            camera.pos += camMoveVec * (float)screen.frameTimes.delta;
        }

        constexpr static Input::ButtonKey
            keyEsc{vkfw::Key::Escape},
            keyR{vkfw::Key::R},
            keyLc{vkfw::MouseButton::Left},
            keyW{vkfw::Key::W},
            keyS{vkfw::Key::S},
            keyA{vkfw::Key::A},
            keyD{vkfw::Key::D},
            keySp{vkfw::Key::Space},
            keySh{vkfw::Key::LeftShift},
            keyQ{vkfw::Key::Q},
            keyE{vkfw::Key::E};
        void onCameraKeyEvent(Input::Action::Button e){
            // X Y Z
            using namespace Input::Actions;
            static bool forward, back, left, right, up, down = false;
            static bool pitchL, pitchR = false;
            if(e.matches(keyR) ){ camera.resetRotation(); }
            if(e.matches(keyEsc, PRESS)){ 
                captureMouse = false; 
                screen.setMouseCapture(vkfw::CursorMode::Normal);
                screen.moveMouse(screen.width/2.0, screen.height/2.0);
            }
            if(e.matches(keyLc,  PRESS)){ 
                captureMouse = true;
                screen.setMouseCapture(vkfw::CursorMode::Disabled);
                prevX = screen.width/2.0;
                prevY = screen.height/2.0;
                screen.moveMouse(screen.width/2.0, screen.height/2.0);
            }

            if(e.matches(keyW) ){ forward = e.pressed(); }
            if(e.matches(keyS) ){    back = e.pressed(); }
            if(e.matches(keyA) ){    left = e.pressed(); }
            if(e.matches(keyD) ){   right = e.pressed(); }
            if(e.matches(keySp)){      up = e.pressed(); }
            if(e.matches(keySh)){    down = e.pressed(); }
            if(e.matches(keyQ) ){  pitchL = e.pressed(); }
            if(e.matches(keyE) ){  pitchR = e.pressed(); }
            auto computeCamMovVec = [&](){
                glm::vec3 mov = glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
                if(right^left)   { mov.x = speed*(right-left); }
                if(up^down)      { mov.y = speed*(up-down); }
                if(forward^back) { mov.z = speed*(back-forward); } // -z is into page
                if(pitchL^pitchR){ rot.z = speed*(pitchL-pitchR); }

                camMoveVec = mov;
                camRotVec = rot;
            };
            computeCamMovVec();
        };
        void onCameraMouseEvent(Input::Action::MousePos e){
            if(!captureMouse){ return; }

            float rotX = sensitivity * (float)(e.y - prevY) / screen.height;
            float rotY = sensitivity * (float)(e.x - prevX) / screen.width;

            // Calculates upcoming vertical change in the Orientation
            glm::vec3 newOrientation = glm::rotate(camera.orientation, glm::radians(-rotX), glm::normalize(glm::cross(camera.orientation, camera.up)));

            // Decides whether or not the next vertical Orientation is legal or not
            if (abs(glm::angle(newOrientation, camera.up) - glm::radians(90.0f)) < glm::radians(89.0f)){
                camera.orientation = newOrientation;
            }
            // Rotates the Orientation left and right
            camera.orientation = glm::rotate(camera.orientation, glm::radians(-rotY), camera.up);

            prevX = e.x;
            prevY = e.y;
        }
        void setupCameraInput(){
            using namespace Input;
            using namespace Input::Actions;
            auto event = std::bind(&CameraController::onCameraKeyEvent, this, std::placeholders::_1);
            auto shouldTrigger = [](ActiveContexts& contexts)->bool{return true;};
            std::vector<Trigger::Button> triggers = {
                {keyEsc, PRESS|RELEASE},
                {keyLc, PRESS|RELEASE},
                {keyR, PRESS|RELEASE},
                {keyW, PRESS|RELEASE},
                {keyS, PRESS|RELEASE},
                {keyA, PRESS|RELEASE},
                {keyD, PRESS|RELEASE},
                {keySp, PRESS|RELEASE},
                {keySh, PRESS|RELEASE},
                {keyQ, PRESS|RELEASE},
                {keyE, PRESS|RELEASE},
            };
            ButtonInputHandler keyHandle(event, shouldTrigger, triggers);
            inputSystem.addHandler(keyHandle);
            
            MousePosInputHandler mouseHandle(
                std::bind(&CameraController::onCameraMouseEvent, this, std::placeholders::_1),
                {{}} // Universal capture changed event
            );
            inputSystem.addHandler(mouseHandle);
        }
    };
}