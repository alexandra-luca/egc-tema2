#include "Laborator4.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include "Transform3D.h"
#include "../Laborator3/Object2D.h"
#include "../Laborator3/Transform2D.h"

using namespace std;
#define NRZONE 6

float playerX, playerY, playerZ;
bool pressedW = false;
bool pressedA = false;
bool pressedS = false;
bool pressedD = false;
float lastz;
float dim_platforma = 5;
float speed = 2.5;
float player_speed;
float player_acceleration = 9.8;
float combustibil = 100;
int level = 1;

int procent_rosu = 5;
int procent_galben = 15;
int procent_portocaliu = 10;
int procent_verde = 20;
int procent_maro = 5;
int limita_rosu;
int limita_galben;
int limita_portocaliu;
int limita_verde;
int limita_maro;

bool se_colizioneaza = false;

float speedtime = 0;

float scor_zone = 0;

typedef struct {
	bool platforma[5];
	int tip[5];
	float z;
} tzona;

tzona zona[NRZONE];

void initialize(tzona* x, bool primelePlatforme) {
	int nr_platforme = 2 + rand() % 4;
	if (primelePlatforme)
		nr_platforme = 5;
	for (int i = 0; i < 5; i++) {
		x->platforma[i] = false;
	}

	for (int i = 1; i <= nr_platforme; i++) {
		int index;
		while (1) {
			index = rand() % 5;

			if (x->platforma[index] == false)
				break;
		}
		x->platforma[index] = true;

		int r = rand() % 100;
		if (r >= 0 && r < limita_rosu) {
			// platforma rosie
			x->tip[index] = 0;
		}
		else if (r >= limita_rosu && r < limita_galben) {
			// platforma galbena
			x->tip[index] = 1;
		}
		else if (r >= limita_galben && r < limita_portocaliu) {
			// platforma portocalie
			x->tip[index] = 2;
		}
		else if (r >= limita_portocaliu && r < limita_verde) {
			// platforma verde
			x->tip[index] = 3;
		}
		else if (r >= limita_verde && r < limita_maro) {
			// platforma maro - BONUS
			x->tip[index] = 5;
		}
		else if (r >= limita_maro && r < 100) {
			// platforma simpla, albastra
			x->tip[index] = 4;
		}

		if (primelePlatforme) {
			x->tip[index] = 4;
		}
	}

	int urmeaza_un_spatiu = rand() % 2;
	if (primelePlatforme) {
		urmeaza_un_spatiu = 0;
	}
	float distanta;
	if (urmeaza_un_spatiu == 0) {
		distanta = 0;
	}
	else {
		distanta = 1 + rand() % 2;
	}
	x->z = lastz + distanta + dim_platforma;

	lastz = x->z;
}

bool checkPlayerPlatformCollision(float platformaXcentru, float platformaYcentru, float platformaZcentru) {
	float latime_platforma = 1;
	float lungime_platforma = 5;
	float grosime_platforma = 0.1;

	float platformaX = platformaXcentru - latime_platforma / 2.0;
	float platformaY = platformaYcentru - grosime_platforma / 2.0;
	float platformaZ = platformaZcentru - lungime_platforma / 2.0;

	float margine_stanga = platformaX;
	float margine_dreapta = platformaX + latime_platforma;
	float margine_fata = platformaZ;
	float margine_spate = platformaZ + lungime_platforma;

	// verific daca playerul e in dreptul platformei
	if (!(playerX <= margine_dreapta && playerX >= margine_stanga && playerZ <= margine_spate && playerZ >= margine_fata)) {
		return false;
	}
	
	float distanta = playerY - platformaY;
	float raza_player = 0.5;

	// verific daca colizioneaza
	if (distanta < grosime_platforma + raza_player && distanta > grosime_platforma) {
		se_colizioneaza = true;
		return true;
	}
	else {
		return false;
	}
}

Laborator4::Laborator4()
{
}

Laborator4::~Laborator4()
{
}


void Laborator4::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, bool renderPlayer)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = glm::vec3(0, 1, 2);
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);
	
	// TODO: Set material property uniforms (shininess, kd, ks, object color) 

	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	int is_spotlight_location = glGetUniformLocation(shader->program, "is_spotlight");
	glUniform1i(is_spotlight_location, 0);

	int isPowerup = 0;
	if (speedtime > 0 && renderPlayer) {
		isPowerup = 1;
	}
	int is_powerup_location = glGetUniformLocation(shader->program, "is_powerup");
	glUniform1i(is_powerup_location, isPowerup);

	int r_player_location = glGetUniformLocation(shader->program, "render_player");
	glUniform1i(r_player_location, renderPlayer);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Laborator4::Init()
{
	polygonMode = GL_FILL;

	Mesh* box = new Mesh("box");
	box->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	meshes[box->GetMeshID()] = box;

	Mesh* sphere = new Mesh("sphere");
	sphere->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[sphere->GetMeshID()] = sphere;

	// initialize tx, ty and tz (the translation steps)
	playerX = 0;
	playerY = 50.25;
	playerZ = 0;

	// initialize sx, sy and sz (the scale factors)
	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	// initialize angularSteps
	angularStepOX = 0;
	angularStepOY = 0;
	angularStepOZ = 0;

	lastz = playerZ-2;

	{
		limita_rosu = procent_rosu;
		limita_galben = limita_rosu + procent_galben;
		limita_portocaliu = limita_galben + procent_portocaliu;
		limita_verde = limita_portocaliu + procent_verde;
		limita_maro = limita_verde + procent_maro;
	}

	for (int i = 0; i < 3; i++) {
		initialize(&zona[i], true);
	}
	for (int i = 3; i < NRZONE; i++) {
		initialize(&zona[i], false);
	}

	glm::vec3 corner = glm::vec3(0, 0, 0);
	Mesh* dreptunghi_negru = Object2D::CreateSquare("dreptunghi_negru", corner, 1, glm::vec3(0.1, 0.1, 0.1), true);
	AddMeshToList(dreptunghi_negru);

	corner = glm::vec3(0, 0, 0);
	Mesh* dreptunghi_colorat = Object2D::CreateSquare("dreptunghi_colorat", corner, 1, glm::vec3(0.941, 0.581, 0.039), true);
	AddMeshToList(dreptunghi_colorat);

	{
		lightPosition = glm::vec3(0, 2, -3);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 4;
		materialKs = 0;
	}

	{
		Shader* shader = new Shader("ShaderLab4");
		shader->AddShader("Source/Laboratoare/Laborator4/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator4/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Laborator4::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	// BONUS - in functie de level se schimba culoarea background-ului
	if (level == 1)
		glClearColor(0, 0, 0, 1);
	else if (level == 2)
		glClearColor(1, 0, 0.5, 1);
	else if (level == 3)
		glClearColor(0, 0.75, 0.85, 1);
	else if (level == 4)
		glClearColor(1, 0.65, 0.65, 1);
	else if (level == 5)
		glClearColor(0, 0, 0.4, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator4::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// platforme
	for (int i = 0; i < NRZONE; i++) {
		for (int j = 0; j < 5; j++) {
			if (zona[i].platforma[j] == true) {
				float platformaX = j-2;
				float platformaY = 0;
				float platformaZ = -zona[i].z;

				glm::vec3 color;

				if (zona[i].tip[j] == 0) {
					// platforma rosie
					color = glm::vec3(0.79, 0.05, 0);
				}
				else if (zona[i].tip[j] == 1) {
					// platforma galbena
					color = glm::vec3(0.90, 0.94, 0.11);
				}
				else if (zona[i].tip[j] == 2) {
					// platforma portocalie
					color = glm::vec3(1, 0.35, 0.03);
				}
				else if (zona[i].tip[j] == 3) {
					// platforma verde
					color = glm::vec3(0.05, 0.68, 0.16);
				}
				else if (zona[i].tip[j] == 4) {
					// platforma simpla, albastra
					color = glm::vec3(0.04, 0.57, 0.66);
				}
				else if (zona[i].tip[j] == 5) {
					// platforma maro - BONUS
					color = glm::vec3(0.27, 0.07, 0);
				}
				else if (zona[i].tip[j] == -1) {
					// platforma mov
					color = glm::vec3(0.72, 0.09, 0.80);
				} 

				modelMatrix = glm::mat4(1);
				modelMatrix *= Transform3D::Translate(platformaX, platformaY, platformaZ);
				modelMatrix *= Transform3D::Scale(1, 0.1, 5);
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab4"], modelMatrix, color, false);
			}
		}
	}

	if (speedtime > 0) {
		pressedW = false;
		pressedS = false;

		// BONUS - depinde de level
		speed = 15 * level/2.0;

		speedtime -= 1 * deltaTimeSeconds;
		if (speedtime <= 0) {
			speed = 2.5;
		}
	}

	for (int i = 0; i < NRZONE; i++) {
		zona[i].z -= speed * deltaTimeSeconds;
	}
	lastz -= speed * deltaTimeSeconds;

	for (int i = 0; i < NRZONE; i++) {
		if (zona[i].z < -5) {
			initialize(&zona[i], false);

			// scor - BONUS
			scor_zone += 10;
			cout << "Scor: " << scor_zone << endl;
		}
	}
	
	// jucator
	player_speed -= player_acceleration * deltaTimeSeconds;
	playerY += player_speed * deltaTimeSeconds;

	se_colizioneaza = false;
	for (int i = 0; i < NRZONE; i++) {
		for (int j = 0; j < 5; j++) {
			if (zona[i].platforma[j] == true) {
				float platformaX = j - 2;
				float platformaY = 0;
				float platformaZ = -zona[i].z;

				// coliziune
				if (checkPlayerPlatformCollision(platformaX, platformaY, platformaZ)) {
					if (player_speed < 0) {
						player_speed = 0;
						playerY = 0.25;
					}
					
					switch (zona[i].tip[j]) {
						case 0:
							// platforma e rosie
							// jocul se termina
							cout << "Ai nimerit platforma rosie" << endl;
							cout << "Game over" << endl;
							exit(0);

						case 1:
							// platforma e galbena
							// se pierde o parte din combustibil
							// BONUS - depinde de level
							combustibil -= 10 * level/2.0;
							cout << "Ai pierdut combustibil" << endl;
							break;

						case 2:
							// platforma e portocalie
							// viteza foarte mare
							cout << "Run, Forrest, run!!" << endl;
							speedtime = 5;
							break;

						case 3:
							// platforma e verde
							// se recupereaza o parte din combustibil
							// BONUS - depinde de level
							combustibil += 10 * level/2.0;
							cout << "Yay, ai primit combustibil" << endl;
							break;

						case 4:
							// platforma e albastra
							break;

						case 5:
							// platforma e maro - BONUS
							// scorul se micsoreaza
							scor_zone -= 20;
							cout << "Aveai scorul prea mare oricum" << endl;
							cout << "Scor: " << scor_zone << endl;
							break;
					}

					// platforma mov
					// a fost atinsa de player
					zona[i].tip[j] = -1;
				}
			}
		}
	}

	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(playerX, playerY, playerZ);
	// BONUS - animatie cand playerul cade de pe platforma
	float dim = 0.5;
	if (playerY < 0.25)
		dim = 0.5 - playerY*playerY;
	modelMatrix *= Transform3D::Scale(dim, dim, dim);
	RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab4"], modelMatrix, glm::vec3(0.5, 0.5, 0.5), true);

	if (playerY < -20) {
		cout << "Ai cazut de pe platforma" << endl;
		cout << "Game over";
		exit(0);
	}

	// level - BONUS
	if (scor_zone >= 100 && scor_zone < 200)
		level = 2;
	else if (scor_zone >= 200 && scor_zone < 300)
		level = 3;
	else if (scor_zone >= 300 && scor_zone < 400)
		level = 4;
	else if (scor_zone >= 400)
		level = 5;


	// combustibil
	combustibil -= 3 * deltaTimeSeconds;

	if (combustibil > 100)
		combustibil = 100;
	if (combustibil <= 0) {
		cout << "Ai ramas fara combustibil" << endl;
		cout << "Game over";
		exit(0);
	}

	glm::mat3 modelMatrix2D;

	modelMatrix2D = glm::mat3(1);
	modelMatrix2D *= Transform2D::Translate(-1, -0.6);
	modelMatrix2D *= Transform2D::Scale((float)2*combustibil/100, 0.1);
	RenderMesh2D(meshes["dreptunghi_colorat"], shaders["VertexColor"], modelMatrix2D);

	modelMatrix2D = glm::mat3(1);
	modelMatrix2D *= Transform2D::Translate(-1, -0.6);
	modelMatrix2D *= Transform2D::Scale(2, 0.1);
	RenderMesh2D(meshes["dreptunghi_negru"], shaders["VertexColor"], modelMatrix2D);
}

void Laborator4::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator4::OnInputUpdate(float deltaTime, int mods)
{
	if (pressedW) {
		speed += 5 * deltaTime;
	}

	if (pressedA) {
		playerX -= 5 * deltaTime;
	}

	if (pressedS) {
		speed -= 5 * deltaTime;

		if (speed < 1.5)
			speed = 1.5;
	}

	if (pressedD) {
		playerX += 5 * deltaTime;
	}
}

void Laborator4::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_W && speedtime <= 0) {
		pressedW = true;
	}

	if (key == GLFW_KEY_A) {
		pressedA = true;
	}

	if (key == GLFW_KEY_S && speedtime <= 0) {
		pressedS = true;
	}

	if (key == GLFW_KEY_D) {
		pressedD = true;
	}

	// BONUS - taste pentru testare level
	if (key == GLFW_KEY_N) {
		scor_zone += 50;
	}
	if (key == GLFW_KEY_M) {
		scor_zone -= 50;
	}

	if (se_colizioneaza) {
		if (key == GLFW_KEY_SPACE) {
			player_speed = 5;
		}
	}
}

void Laborator4::OnKeyRelease(int key, int mods)
{
	// add key release event
	if (key == GLFW_KEY_W) {
		pressedW = false;
	}

	if (key == GLFW_KEY_A) {
		pressedA = false;
	}

	if (key == GLFW_KEY_S) {
		pressedS = false;
	}

	if (key == GLFW_KEY_D) {
		pressedD = false;
	}
}

void Laborator4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator4::OnWindowResize(int width, int height)
{
}
