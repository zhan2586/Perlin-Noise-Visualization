#include <windows.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "InitShader.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "imgui_impl_glut.h"

#include "Cube.h"

static const std::string vertex_shader("msn_vs.glsl");
static const std::string fragment_shader("msn_fs.glsl");

GLuint shader_program = -1;
GLuint texture_id = -1; //Texture map for fish

//static const std::string mesh_name = "Amago0.obj";
//static const std::string texture_name = "AmagoT.bmp";
//static const std::string texture_name = "AngelST_2.bmp";
static const std::string mesh_name = "bunny.obj";
static const std::string texture_name = "bunny_fur.jpg";

//GroundForestPath005_COL_1K.jpg
//Plaster13_COL_VAR1_1K.jpg
//RockDark004_COL_VAR1_1K.jpg
//RockMossyFull006_COL_VAR1_1K.jpg
//ConcreteWorn001_COL_VAR1_1K.jpg
MeshData mesh_data;

static const std::string cube_name = "cubemap";
GLuint cubemap_id = -1; //Texture id for cubemap

float angle = 25.0f;
bool clearScreen = true;
bool depthTesting = true;
float clearColor[3] = { 0.6f, 0.8f, 0.9f };
//float brightness = 0.5f;
float scale = 0.3f;
glm::vec3 pos(0.0f, 0.0f, 0.0f);
bool show_app_about = true;
glm::vec3 camPos(0.0f, 0.3f, 1.0f);
float fieldAngle = 35.0f;
float nearClip = 0.1f;
float farClip = 100.0f;
float size = 1.0f;
float normalized[3] = { 0, 0, 0 };
float diffuse_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float diffuse_light[4] = { 1.0f, 0.0f, 0.8f, 1.0f };
float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float specular_light[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
bool texEnable = true;
bool noise_type = 1;
static int rockTex = 0;
int texture_type = 1;
float shininess = 2.0f;
float frequency = 40.0;

//Cube files and IDs
static const std::string cube_vs("cube_vs.glsl");
static const std::string cube_fs("cube_fs.glsl");
GLuint cube_shader_program = -1;
GLuint cube_vao = -1;
bool cube_enabled = false;

void draw_cube(const glm::mat4& P, const glm::mat4& V)
{
	glUseProgram(cube_shader_program);
	int PVM_loc = glGetUniformLocation(cube_shader_program, "PVM");
	if (PVM_loc != -1)
	{
		glm::mat4 Msky = glm::scale(glm::vec3(50.0f)) * glm::rotate(angle,glm::vec3(0.0f,1.0f,0.0f));
		glm::mat4 PVM = P*V*Msky;
		PVM[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
	int cube_loc = glGetUniformLocation(cube_shader_program, "cubemap");
	if (cube_loc != -1)
	{
		glUniform1i(cube_loc, 1); // we bound our texture to texture unit 1
	}

	draw_cube_vao(cube_vao);
}



void draw_gui()
{
   ImGui_ImplGlut_NewFrame();
   
   if (show_app_about)
   {
	   auto red = ImColor(0, 0, 0, 255);
	   ImGui::PushStyleColor(ImGuiCol_TitleBg, red);
	   ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, red);
	   ImGui::PushStyleColor(ImGuiCol_TitleBgActive, red);

	   ImGui::Begin("Control Panel", &show_app_about, ImGuiWindowFlags_AlwaysAutoResize);
	   //uncomment the following line to create a slider which changes the viewing angle
	   ImGui::SliderFloat("View Angle", &angle, -180.0f, +180.0f);
	   //ImGui::Checkbox("Clear Screen", &clearScreen);
	   //ImGui::Checkbox("Depth Testing", &depthTesting);
	   ImGui::Checkbox("Perlin Noise", &texEnable); ImGui::SameLine;
	   //ImGui::Checkbox("Perlin Noise", &rockTex);
	   ImGui::ColorEdit4("diffuse_color", diffuse_color, false);
	   ImGui::ColorEdit4("diffuse_light", diffuse_light, false);
	   ImGui::ColorEdit4("specular_color", specular_color, false);
	   ImGui::ColorEdit4("specular_light", specular_light, false);
	   //ImGui::SliderFloat("Shininess", &shininess, 0.0f, 10.0f);
	   //ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
	   //ImGui::SliderFloat("Scale", &scale, -2.0f, 2.0f);
	   ImGui::SliderFloat3("Pos", &pos[0], -1.0f, 1.0f);
	   //ImGui::SliderFloat3("Camera Translate", &camPos[0], -5.0f, 5.0f);
	   ImGui::SliderFloat("Field of Angle", &fieldAngle, 0.0f, 180.0f);
	   //ImGui::SliderFloat("Near Clip", &nearClip, 0.1f, 5.0f);
	   //ImGui::SliderFloat("Far Clip", &farClip, 5.0f, 10.0f);
	   // ImGui::SliderFloat("Size", &size, 0.0f, 2.0f);
	   ImGui::SliderFloat("Frequency", &frequency, 0.0f, 500.0f);
	   
	   ImGui::End();
	   ImGui::PopStyleColor(3);
   }

   static bool show_test = false;
   ImGui::ShowTestWindow(&show_test);
   ImGui::Render();
 }


// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
	
	if (clearScreen == true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the back buffer
	}
	if (depthTesting == true)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
   
   glm::mat4 T = glm::translate(glm::mat4(),pos);
   glm::mat4 M = T * glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));
   glm::mat4 V = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 P = glm::perspective(fieldAngle, 1.0f, nearClip, farClip);
   

   int PVM_loc = glGetUniformLocation(shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   int M_loc = glGetUniformLocation(shader_program, "M2");
   if (M_loc != -1)
   {
	   glUniformMatrix4fv(M_loc, 1, false, glm::value_ptr(M));
   }

   int V_loc = glGetUniformLocation(shader_program, "V");
   if (V_loc != -1)
   {
	   glUniformMatrix4fv(V_loc, 1, false, glm::value_ptr(V));
   }

   draw_cube(P, V);

   glUseProgram(shader_program);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   /*switch (noise_type) {
   case 1:  texture_id = LoadTexture("AngelST.bmp"); break;
   case 2:  texture_id = LoadTexture("AngelST_2.bmp"); break;
   default:texture_id = LoadTexture("AngelST.bmp"); break;
   }
 */
   int tex_loc = glGetUniformLocation(shader_program, "texture");
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   int texEnable_loc = glGetUniformLocation(shader_program, "textureEnable");
   if (texEnable_loc != -1)
   {
	   glUniform1f(texEnable_loc, texEnable);
   }

   glActiveTexture(GL_TEXTURE1);
   /*
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
   int cube_loc = glGetUniformLocation(shader_program, "cubemap");
   if (cube_loc != -1)
   {
	   glUniform1i(cube_loc, 1); // we bound our texture to texture unit 1
   }
  
   */
   // send new nosie type
   int noise_type_loc = glGetUniformLocation(shader_program, "noise_type");
   if (noise_type_loc != -1)
   {
	   glUniform1i(noise_type_loc, noise_type);
   }
   glUniform1i(glGetUniformLocation(shader_program, "noise_type"), noise_type);
   glBindTexture(GL_TEXTURE_2D, texture_id);

   

   glBindVertexArray(mesh_data.mVao);
   glDrawElements(GL_TRIANGLES, mesh_data.mNumIndices, GL_UNSIGNED_INT, 0);
   
   //int brightness_loc = glGetUniformLocation(shader_program, "brightness");
   //if (brightness_loc != -1)
   //{
	  // glUniform1f(brightness_loc, brightness); // we bound our texture to texture unit 0
   //}

   //int size_loc = glGetUniformLocation(shader_program, "size");
   //if (size_loc != -1)
   //{
	  // glUniform1f(size_loc, size); 
   //}

   int diffuse_color_loc = glGetUniformLocation(shader_program, "diffuse_color");
   if (diffuse_color_loc != -1)
   {
	   glUniform4f(diffuse_color_loc, diffuse_color[0], diffuse_color[1], diffuse_color[2], diffuse_color[3]);
   }
   
   int diffuse_light_loc = glGetUniformLocation(shader_program, "diffuse_light");
   if (diffuse_light_loc != -1)
   {
	  glUniform4f(diffuse_light_loc, diffuse_light[0], diffuse_light[1], diffuse_light[2], diffuse_light[3]);
   }

   int specular_color_loc = glGetUniformLocation(shader_program, "specular_color");
   if (specular_color_loc != -1)
   {
	   glUniform4f(specular_color_loc, specular_color[0], specular_color[1], specular_color[2], specular_color[3]);
   }

   int specular_light_loc = glGetUniformLocation(shader_program, "specular_light");
   if (specular_light_loc != -1)
   {
	   glUniform4f(specular_light_loc, specular_light[0], specular_light[1], specular_light[2], specular_light[3]);
   }

   int shininess_loc = glGetUniformLocation(shader_program, "shininess");
   if (shininess_loc != -1)
   {
	   glUniform1f(shininess_loc, shininess);
   }

   int frequency_loc = glGetUniformLocation(shader_program, "frequency");
   if (frequency_loc != -1)
   {
	   glUniform1f(frequency_loc, frequency);
   }
   
      draw_gui();

   glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   float time_sec = 0.001f*time_ms;
  
   int time_loc = glGetUniformLocation(shader_program, "time");
   if (time_loc != -1)
   {
      glUniform1f(time_loc, time_sec);
   }
}

void reload_shader()
{
   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

   if(new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
   }
   else
   {
      glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

      if(shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;

      if(mesh_data.mVao != -1)
      {
         BufferIndexedVerts(mesh_data);
      }
   }
}

void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}

void initOpenGl()
{
   glewInit();

   glEnable(GL_DEPTH_TEST);

   reload_shader();

   //mesh and texture to be rendered
   mesh_data = LoadMesh(mesh_name);
   texture_id = LoadTexture(texture_name);

   cube_shader_program = InitShader(cube_vs.c_str(), cube_fs.c_str());
   cube_vao = create_cube_vao();
   cubemap_id = LoadCube(cube_name);
}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);
   //std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();     
      break;
   }
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x,y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}


int main (int argc, char **argv)
{
   //Configure initial window state
   glutInit(&argc, argv); 
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (1000, 1000);
   int win = glutCreateWindow ("Make Some Noise");

   printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special);
   glutKeyboardUpFunc(keyboard_up);
   glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);
   glutIdleFunc(idle);

   initOpenGl();
   ImGui_ImplGlut_Init(); // initialize the imgui system

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;		
}

