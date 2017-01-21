#include <GLFW/glfw3.h>

/* InitTimeSystem begins the time system. */
void InitTimeSystem() { glfwSetTime(0.0); }

/* GetTime returns the current time since the Time system was initialized. */
float GetTime() { return glfwGetTime(); }
