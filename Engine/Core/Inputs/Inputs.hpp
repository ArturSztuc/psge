//#pragma once
//
//glfwSetKeyCallback(Window* window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
//    {
//    InputEvent event;
//    event.type = (action == GLFW_PRESS) ? InputEvent::Type::KeyPress : InputEvent::Type::KeyRelease;
//    event.key = key;
//    eventQueue.push(event);
//    });
//
//while(!GameShouldClose)
//{
//  while(!eventQueue.empty()){
//    const InputEvent& event = eventQueue.front();
//    if (event.type == InputEvent::Type::KeyPress)
//    {
//      // Handle key press event
//      //             // ...
//    }
//    else if (event.type == InputEvent::Type::KeyRelease)
//    {
//      // Handle key release event
//      // ...
//    }
//    eventQueue.pop();
//  }
//
//}
