#ifndef CS3P98_TEST_CAMERA_H
#define CS3P98_TEST_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 200.0f;                     // commercial jet speeds are around 150-250 m/s
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

/*
	First person camera class using the lookAt technique - https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
	** We'll need a true flight sim camera with YAW, PITCH, and ROLL
	** ROLL is not supported here
    ** probably use quaternions to avoid gimbal lock - https://gamedev.stackexchange.com/questions/103502/how-can-i-implement-a-quaternion-camera
                                                     - https://www.gamedev.net/tutorials/programming/math-and-physics/a-simple-quaternion-based-camera-r1997/
    ** SUGGESTED CONTROLS FOR PLANE FLIGHTSIM:
    *   ENTER - Apply thrust (Move forward)
    *   W - Tilt down
    *   S - Tilt up
    *   A - Tilt Left (roll cam left and gradually change yaw)
    *   D - Tilt right
    
    ** Make Camera class owner of mouse and keyboard input handling functions
*/
class TestCamera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // projection matrix
    glm::mat4 proj;                 // camera projection matrix
    float renderDist;               // render distance of this camera in world space (default 100.0)

    // constructor with vectors
    TestCamera(float screenAspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
        MovementSpeed(SPEED), 
        MouseSensitivity(SENSITIVITY), 
        Zoom(ZOOM), 
        renderDist(100.0f)
    {
        redefineProjectionMatrix(screenAspectRatio);
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    TestCamera(float screenAspectRatio, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), renderDist(100.0f)
    {
        redefineProjectionMatrix(screenAspectRatio);
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // compute projection matrix for this camera object
    void redefineProjectionMatrix(float aspectRatio) {
        proj = glm::perspective(glm::radians(45.0f), 1000.0f/1000.0f, 0.1f, 4700.0f);           // horizon on earth is approximately 4.7km (4700 m) away
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif