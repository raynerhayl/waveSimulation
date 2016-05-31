#
# CGRA Project
# If you add new source files, you must list them here
#

# TODO list your header files (.hpp) here
SET(headers
	"cgra_geometry.hpp"
	"cgra_math.hpp"
	"opengl.hpp"
	"simple_shader.hpp"
	"simple_image.hpp"
	"shady_geometry.hpp"
	"school.hpp"
	"boid.hpp"
	"wave.hpp"
	"simple_gui.hpp"
)

# TODO list your source files (.cpp) here
SET(sources
	"main.cpp"
	"school.cpp"
	"boid.cpp"
	"wave.cpp"
	"simple_gui.cpp"
)

# Add executable target and link libraries
# You do not need to touch this
add_executable(${CGRA_PROJECT} ${headers} ${sources})
target_link_libraries(${CGRA_PROJECT} PRIVATE glew glfw ${GLFW_LIBRARIES})
target_link_libraries(${CGRA_PROJECT} PRIVATE stb)
target_link_libraries(${CGRA_PROJECT} PRIVATE imgui)