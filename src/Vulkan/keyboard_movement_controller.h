#pragma once

#include "hdn_game_object.h"
#include "hdn_window.h"

namespace hdn{

    class KeyboardMovementController{

    public:
        struct KeyMappings {
            int moveForward = GLFW_KEY_W;
            int moveLeft = GLFW_KEY_A;
            int moveBackward = GLFW_KEY_S;
            int moveRight = GLFW_KEY_D;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, HdnGameObject& gameObject);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};

    };

}