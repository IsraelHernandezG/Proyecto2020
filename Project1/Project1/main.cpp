/*
Proyecto final
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

//bibliotecas para el manejo de archivos
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
//#include <string> 

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_texturizado.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

using namespace std;
const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Camera camera_Anim;
//
int banderaCamara = 3;
GLfloat cam1x = -25.0f, cam1y = 0.0f, cam1z = 25.0f; //cuarto dia muertos
GLfloat cam2x = -25.0f, cam2y = 0.0f, cam2z = -25.0f; //cuarto navidad
float cam3x = 0.0f, cam3y = 0.0f, cam3z = 0.0f; //camara libre
GLfloat cam1yaw = 0.0f, cam1pitch = 0.0f;
GLfloat cam2yaw = 0.0f, cam2pitch = 0.0f;
float cam3yaw = 0.0f, cam3pitch = 0.0f;
int banderaCanasta = 1;


Texture brickTexture;
Texture plainTexture;
Texture pisoTexture;
Texture paredLadrilloTexture;
Texture paredLadrillo2Texture;
Texture terrainTexture;
Texture TexTree;
Texture carTexture;
Texture panMuertoTexture;
Texture Tagave;
Texture vidrioTexture;
Texture papelPicado1;
Texture papelPicado2;
Texture papelPicado3;
Texture papelPicado4;
Texture digimon1;
Texture digimon2;
Texture digimon3;
Texture nacimiento1;
Texture nacimiento2;
Texture nacimiento3;
Texture nacimiento4;
Texture nacimiento5;
Texture nacimiento6;

Texture PizzaTex;
Texture RocaTex;
//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model Blackhawk_M;
Model Windo;
Model Tree;
Model Roca;
Model Pizza;
Model Pan;
Model Cajas;
Model Velas;
Model Dulce;
Model Calabaza;
Model Mesa;
Model Gisado;
Model PanMuerto;
Model NocheBuena;
Model Huesos;
Model Pinata;
Model Corona;
Model MesaC;
Model Cama;
Model Cama2;
Model Sillon;
Model Comedor;
Model Canasta;
Model Pisina;
Model Comida1;
Model Comida2;
Model Establo;
Model Regalo;
Model Marco;
Model Puerta;
Model TRICERTP;
//digimon para animar
Model Digimon;
//fantasma animacion
Model Fantasma;
Skybox skybox;
int Axi=0,Axj=0;
bool AuxL1= false,AuxL2=false,AuxL3=false, AuxL4 = false, AuxL5 = false;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat girollanta = 0.0f;
GLfloat desplazamientoX = 0.0f;
GLfloat desplazamientoY = 0.0f;
GLfloat desplazamientoZ = 0.0f;
GLfloat giroY = 0.0f;
GLfloat giroZ = 0.0f;
int bandera = 0;
//control luz spotlight
int banderaL = 0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void creaArchivo(string name) {
	ofstream fs(name.c_str());
	fs.close();
}

string leerArchivo(string name, long numlinea) {
	ifstream fs(name.c_str(), ios::in);
	char linea[128];
	long contador = 0L;
	if (fs.fail())
		printf("el fichero no existe\n");
	else
		while (!fs.eof()) {
			fs.getline(linea, sizeof(linea));

			if (numlinea == contador) {
				//cout << linea << endl;
				string str(linea);
				return str;
				break;
			}
			contador++;
		}
	fs.close();

}

void readFile(string name) {
	ifstream fs(name.c_str(), ios::in);
	char linea[128];
	long contador = 0L;
	if (fs.fail())
		printf("el fichero no existe\n");
	else
		while (!fs.eof()) {
			fs.getline(linea, sizeof(linea));
			cout << linea << endl;
		}
	fs.close();
}

void editarArchivo(string name, string contenido) {
	ofstream fs(name.c_str(), ios::app);
	fs << contenido << endl;
	fs.close();
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int terrainIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat terrainVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	5.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 5.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		5.0f, 5.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int wallIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat wallVertices[] = {
		-10.0f, -10.0f, 0.0f,	0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		10.0f, -10.0f, 0.0f,	5.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		-10.0f, 10.0f, 0.0f,	0.0f, 2.0f,	0.0f, 0.0f, -1.0f,
		10.0f, 10.0f, 0.0f,		5.0f, 2.0f,	0.0f, 0.0f, -1.0f,
	};

	unsigned int cuadroIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat cuadroVertices[] = {
		-1.0f, -1.0f, 0.0f,	0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, 0.0f,	1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, 0.0f,	1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	//0
	Mesh* obj0 = new Mesh();
	obj0->CreateMesh(terrainVertices, terrainIndices, 32, 6);
	meshList.push_back(obj0);
	//1
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj1);

	//2
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(wallVertices, wallIndices, 32, 6);
	meshList.push_back(obj2);

	//3 cuadro simple
	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(cuadroVertices, cuadroIndices, 32, 6);
	meshList.push_back(obj3);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

///////////////////////////////KEYFRAMES/////////////////////

bool animacion = false;

//NEW// Keyframes animacion 1
float movAvion_x = 0.0f,
movAvion_y = 0.0f,
movAvion_z = 0.0f,
giroAvion = 0;

//NEW// Keyframes animacion 2
float mov_x = 0.0f,
mov_y = 0.0f,
mov_z = 0.0f,
giro_y = 0.0f,
giro_z = 0.0f;


#define MAX_FRAMES 100
int i_max_steps = 90;

typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float movAvion_zInc;		//Variable para IncrementoZ
	float giroAvion;
	float giroAvionInc;
}FRAME_AVION;

typedef struct _frame2
{
	//Variables para GUARDAR Key Frames
	float movCamera_x;		//Variable para PosicionX
	float movCamera_y;		//Variable para PosicionY
	float movCamera_z;		//Variable para PosicionZ
	float movCamera_xInc;		//Variable para IncrementoX
	float movCamera_yInc;		//Variable para IncrementoY
	float movCamera_zInc;		//Variable para IncrementoZ
	float giroCamera_yaw;
	float giroCamera_yawInc;
	float giroCamera_pitch;
	float giroCamera_pitchInc;
}FRAME_CAMERA;

FRAME_AVION KeyFrame[MAX_FRAMES];
FRAME_CAMERA KeyFrame_Camera[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
int FrameIndex_Camera = 0;
bool play = false;
bool play_Camera = false;
int playIndex = 0;
int playIndex_Camera = 0;
int i_curr_steps = 5;
int i_curr_steps_Camera = 5;

void saveFrame(void){
	string name = "animation.txt";
	std::string s = "";
	std::ostringstream strsX, strsY, strsZ, strsRY, strs;

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	strsX << movAvion_x;
	s = strsX.str();
	editarArchivo(name, s);

	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	strsY << movAvion_y;
	s = strsY.str();
	editarArchivo(name, s);

	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	strsZ << movAvion_z;
	s = strsZ.str();
	editarArchivo(name, s);

	KeyFrame[FrameIndex].giroAvion = giroAvion;
	strsRY << giroAvion;
	s = strsRY.str();
	editarArchivo(name, s);

	FrameIndex++;
	strs << FrameIndex;
	s = strs.str();
	creaArchivo("frames.txt");
	editarArchivo("frames.txt", s);
}

void saveFrame_Camera(void)
{
	string name = "animation_Camera.txt";
	std::string s = "";
	std::ostringstream strsX, strsY, strsZ, strsRY, strsRZ, strs;

	printf("frameindex %d\n", FrameIndex_Camera);

	KeyFrame_Camera[FrameIndex_Camera].movCamera_x = mov_x;
	strsX << mov_x;
	s = strsX.str();
	editarArchivo(name, s);

	KeyFrame_Camera[FrameIndex_Camera].movCamera_y = mov_y;
	strsY << mov_y;
	s = strsY.str();
	editarArchivo(name, s);

	KeyFrame_Camera[FrameIndex_Camera].movCamera_z = mov_z;
	strsZ << mov_z;
	s = strsZ.str();
	editarArchivo(name, s);

	KeyFrame_Camera[FrameIndex_Camera].giroCamera_yaw = giro_y;
	strsRY << giro_y;
	s = strsRY.str();
	editarArchivo(name, s);

	KeyFrame_Camera[FrameIndex_Camera].giroCamera_pitch = giro_z;
	strsRZ << giro_z;
	s = strsRZ.str();
	editarArchivo(name, s);

	FrameIndex_Camera++;
	strs << FrameIndex_Camera;
	s = strs.str();
	creaArchivo("frames_Camera.txt");
	editarArchivo("frames_Camera.txt", s);
}

void cargaFrames(void) {
	//KeyFrame[MAX_FRAMES];
	FrameIndex = 0;
	playIndex = 0;

	string name = "animation.txt";
	string salida = leerArchivo("frames.txt", 0L);
	std::string::size_type sz;   // alias of size_t
	long numFrames = std::stol(salida, &sz);

	printf("\nKey Frames\n\n");
	long i;
	for (i = 0; i <= numFrames - 1; i++) {

		salida = leerArchivo(name, 4 * i);
		cout << "x: " << salida << endl;
		double l_posX = std::stod(salida, &sz);
		KeyFrame[FrameIndex].movAvion_x = l_posX;
		
		salida = leerArchivo(name, 4 * i + 1);
		cout << "y: " << salida << endl;
		double l_posY = std::stod(salida, &sz);
		KeyFrame[FrameIndex].movAvion_y = l_posY;
		
		salida = leerArchivo(name, 4 * i + 2);
		cout << "z: " << salida << endl;
		double l_posZ = std::stod(salida, &sz);
		KeyFrame[FrameIndex].movAvion_z = l_posZ;
		
		salida = leerArchivo(name, 4 * i + 3);
		cout << "rot_y: " << salida << endl;
		double l_rotY = std::stod(salida, &sz);
		KeyFrame[FrameIndex].giroAvion = l_rotY;
		
		FrameIndex++;
	}
}

void cargaFrames_Camera(void) {
	//KeyFrame[MAX_FRAMES];
	FrameIndex_Camera = 0;
	playIndex_Camera = 0;

	string name = "animation_Camera.txt";
	string salida = leerArchivo("frames_Camera.txt", 0L);
	std::string::size_type sz;   // alias of size_t
	long numFrames = std::stol(salida, &sz);

	printf("\nKey Frames Camera\n\n");
	long i;
	for (i = 0; i <= numFrames - 1; i++) {

		salida = leerArchivo(name, 5 * i);
		cout << "x: " << salida << endl;
		double l_posX = std::stod(salida, &sz);
		KeyFrame_Camera[FrameIndex_Camera].movCamera_x = l_posX;

		salida = leerArchivo(name, 5 * i + 1);
		cout << "y: " << salida << endl;
		double l_posY = std::stod(salida, &sz);
		KeyFrame_Camera[FrameIndex_Camera].movCamera_y = l_posY;

		salida = leerArchivo(name, 5 * i + 2);
		cout << "z: " << salida << endl;
		double l_posZ = std::stod(salida, &sz);
		KeyFrame_Camera[FrameIndex_Camera].movCamera_z = l_posZ;

		salida = leerArchivo(name, 5 * i + 3);
		cout << "rot_y: " << salida << endl;
		double l_rotY = std::stod(salida, &sz);
		KeyFrame_Camera[FrameIndex_Camera].giroCamera_yaw = l_rotY;

		salida = leerArchivo(name, 5 * i + 4);
		cout << "rot_z: " << salida << endl;
		double l_rotZ = std::stod(salida, &sz);
		KeyFrame_Camera[FrameIndex_Camera].giroCamera_pitch = l_rotZ;

		FrameIndex_Camera++;
	}
}

void resetElements(void)
{
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_z = KeyFrame[0].movAvion_z;
	giroAvion = KeyFrame[0].giroAvion;
}

void resetElements_Camera(void)
{
	mov_x = KeyFrame_Camera[0].movCamera_x;
	mov_y = KeyFrame_Camera[0].movCamera_y;
	mov_z = KeyFrame_Camera[0].movCamera_z;
	giro_y = KeyFrame_Camera[0].giroCamera_yaw;
	giro_z = KeyFrame_Camera[0].giroCamera_pitch;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
}

void interpolation_Camera(void)
{
	KeyFrame_Camera[playIndex_Camera].movCamera_xInc = (KeyFrame_Camera[playIndex_Camera + 1].movCamera_x - KeyFrame_Camera[playIndex_Camera].movCamera_x) / i_max_steps;
	KeyFrame_Camera[playIndex_Camera].movCamera_yInc = (KeyFrame_Camera[playIndex_Camera + 1].movCamera_y - KeyFrame_Camera[playIndex_Camera].movCamera_y) / i_max_steps;
	KeyFrame_Camera[playIndex_Camera].movCamera_zInc = (KeyFrame_Camera[playIndex_Camera + 1].movCamera_z - KeyFrame_Camera[playIndex_Camera].movCamera_z) / i_max_steps;
	KeyFrame_Camera[playIndex_Camera].giroCamera_yawInc = (KeyFrame_Camera[playIndex_Camera + 1].giroCamera_yaw - KeyFrame_Camera[playIndex_Camera].giroCamera_yaw) / i_max_steps;
	KeyFrame_Camera[playIndex_Camera].giroCamera_pitchInc = (KeyFrame_Camera[playIndex_Camera + 1].giroCamera_pitch - KeyFrame_Camera[playIndex_Camera].giroCamera_pitch) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("Frame index= %d\n", FrameIndex);
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}

	//Movimiento de la camara
	if (play_Camera)
	{
		if (i_curr_steps_Camera >= i_max_steps) //end of animation between frames?
		{
			playIndex_Camera++;
			printf("playindex Camera: %d\n", playIndex_Camera);
			if (playIndex_Camera > FrameIndex_Camera - 2)	//end of total animation?
			{
				printf("Frame index Camera= %d\n", FrameIndex_Camera);
				printf("termina anim Camera\n");
				playIndex_Camera = 0;
				play_Camera = false;
			}
			else //Next frame interpolations
			{
				//printf("Frame index= %d\n", FrameIndex_Camera);
				i_curr_steps_Camera = 0; //Reset counter
				//Interpolation
				interpolation_Camera();
			}
		}
		else
		{
			//Draw animation
			mov_x += KeyFrame_Camera[playIndex_Camera].movCamera_xInc;
			mov_y += KeyFrame_Camera[playIndex_Camera].movCamera_yInc;
			mov_z += KeyFrame_Camera[playIndex_Camera].movCamera_zInc;
			giro_y += KeyFrame_Camera[playIndex_Camera].giroCamera_yawInc;
			giro_z += KeyFrame_Camera[playIndex_Camera].giroCamera_pitchInc;
			i_curr_steps_Camera++;
		}

	}
}

void inputKeyframes(bool* keys)
{
	//Para vaciar los ficheros
	if (keys[GLFW_KEY_V]) {
		creaArchivo("animation.txt");
		creaArchivo("animation_Camera.txt");
		creaArchivo("frames.txt");
		creaArchivo("frames_Camera.txt");
		glfwWaitEventsTimeout(1.7);
	}

	//Para leer los ficheros
	if (keys[GLFW_KEY_R]) {
		readFile("animation.txt");
		readFile("animation_Camera.txt");
		glfwWaitEventsTimeout(1.7);
	}

	//Para cargar las posiciones guardadas en el arreglo de estructuras
	if (keys[GLFW_KEY_L]) {
		cargaFrames();
		cargaFrames_Camera();
		glfwWaitEventsTimeout(1.7);
	}
	
	//To Save a KeyFrame
	if (keys[GLFW_KEY_G]){

		if (FrameIndex < MAX_FRAMES)
		{
			//saveFrame();
			glfwWaitEventsTimeout(1.7); //delay para evitar lecturas erroneas del teclado
		}
	}

	//To Save a KeyFrame of Camera
	if (keys[GLFW_KEY_H]) {

		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame_Camera();
			glfwWaitEventsTimeout(1.7);
		}
	}
	
	//To play KeyFrame animation 1
	if (keys[GLFW_KEY_P])
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
		glfwWaitEventsTimeout(1.7);
	}

	//To play KeyFrame animation of Camera
	if (keys[GLFW_KEY_O])
	{
		if (play_Camera == false && (FrameIndex_Camera > 1))
		{
			resetElements_Camera();
			//First Interpolation				
			interpolation_Camera();

			play_Camera = true;
			playIndex_Camera = 0;
			i_curr_steps = 0;
		}
		else
		{
			play_Camera = false;
		}
		glfwWaitEventsTimeout(1.7);
	}

}

/* FIN KEYFRAMES*/
/*Animacion circulo*/
GLfloat posXC, posYC, posZC;
int banderaHC = 0;
void animacionCirculo() {
	if (posXC < 20.0 && banderaHC == 0) {
		posYC = sin(posXC);
		posXC += 0.05f;
	}
	if (posXC >= 20.0 && banderaHC == 0) {
		banderaHC = 1;

	}
	if (posXC > 0.0 && banderaHC == 1) {
		posYC = sin(posXC);
		posXC -= 0.05f;
	}
	if (posXC <= 0.0 && banderaHC == 1) {
		banderaHC = 2;

	}
	if (banderaHC == 2) {
		posXC = 0.0;
		posYC = 0.0;
		posZC = 0.0;
		banderaHC = 0;
	}
}
/*fin */
void DisplayHouse(glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	
	
	
	//plano piso
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, -1.95f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();
	//plano techo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 13.0f, 0.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrilloTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	//pared 1
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 50.0f));
	model = glm::scale(model, glm::vec3(5.0f, 0.8f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrilloTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, -50.0f));
	model = glm::scale(model, glm::vec3(5.0f, 0.8f, 1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrilloTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 3
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(50.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 5.0f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrilloTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 4
	/*model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 5.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrilloTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();*/
	//pared 5
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 2.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 6
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 5.0f, 40.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 1.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 7
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 5.0f, -40.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 1.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 8 1/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 12.0f, -25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();
	//pared 8 2/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 12.0f, 25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 8 1/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 0.8f, -25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.28f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();
	//pared 8 2/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.0f, 0.8f, 25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.28f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();




	//pared 5
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-10.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 2.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 6
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-10.0f, 5.0f, 40.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 1.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 7
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-10.0f, 5.0f, -40.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.8f, 1.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//pared 8 1/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-10.0f, 12.0f, -25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();
	//pared 8 2/2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-10.0f, 12.0f, 25.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.5f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();


	//pared 9 De en medio 
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-30.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 0.8f, 0.5f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	paredLadrillo2Texture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//Ventana 1 modelo 
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-49.5f, 7.5f, 25.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.15f, 0.1f, 0.25f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Windo.RenderModel();
	//Ventana 2 modelo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-49.5f, 7.5f, -25.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.15f, 0.1f, 0.25f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Windo.RenderModel();



}

void DisplayCasa(glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	//Elementos de la casa en general
	//Puerta
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-9.5f, -3.0f, 30.5f));
	model = glm::scale(model, glm::vec3(12.0f, 7.0f, 14.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Puerta.RenderModel();

	//Puerta marco
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-9.5f, -3.0f, 30.0f));
	model = glm::scale(model, glm::vec3(12.0f, 7.0f, 14.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Marco.RenderModel();

	//Puerta
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-9.5f, -3.0f, -20.5f));
	model = glm::scale(model, glm::vec3(12.0f, 7.0f, 14.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Puerta.RenderModel();

	//Puerta marco
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-9.5f, -3.0f, -20.0f));
	model = glm::scale(model, glm::vec3(12.0f, 7.0f, 14.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Marco.RenderModel();

	//sila
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(40.0f, -2.0f, 20.0f));
	model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Comedor.RenderModel();
}

void DisplayEscenarioMuertos(glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	//Aqui se dibujan los elementos y modelos del cuarto de dia de muertos
	//gerarquia con modelAux
	glm::mat4 modelAux;

	//Fantasma
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-30.0f, -2.0f, 25.0f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(posXC / 2, posYC / 2, posZC / 2));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	
	//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Fantasma.RenderModel();
	//Digimon
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-30.0f, -2.0f, -4.0f));
	model = glm::translate(model, glm::vec3(0.0, (posYC / 4), 0.0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Digimon.RenderModel();

	//Cabeza Roca
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-25.0f, -4.0f, 47.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Roca.RenderModel();

	//Calabaza 1
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-16.0f, -2.0f, 10.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Calabaza.RenderModel();

	//Calabaza 2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-35.0f, -2.0f, 10.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Calabaza.RenderModel();

	//Mesa
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-26.5f, 0.0f, 5.0f));
	modelAux = model;
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Mesa.RenderModel();

	//Mesa 2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 6.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Mesa.RenderModel();
	//comida 1
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.5f, 2.6f, 1.0f));
	modelAux = model;
	model = glm::translate(model, glm::vec3(-1.3f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Comida1.RenderModel();

	//comida ramen
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.6f, 0.0f, 1.5f));
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Gisado.RenderModel();

	//comida ramen
	model = modelAux;
	model = glm::translate(model, glm::vec3(4.5f, 0.0f, 1.5f));
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Gisado.RenderModel();

	////Gisado
	//model = modelAux;
	//model = glm::translate(model, glm::vec3(-1.5f, 2.6f, 1.0f));
	//modelAux = model;
	//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Gisado.RenderModel();
	////Pizza
	//model = modelAux;
	//model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	////Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Pizza.RenderModel();

	////Pan
	//model = modelAux;
	//model = glm::translate(model, glm::vec3(2.5f, 0.2f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	////Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Pan.RenderModel();

	//Pan de muerto
	//model = modelAux;
	//model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	////Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//PanMuerto.RenderModel();


	//Dulce
	model = modelAux;
	model = glm::translate(model, glm::vec3(-2.0f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.8f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.6f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.4f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.2f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.0f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.8f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.6f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.4f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.2f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.2f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.4f, -3.0f, 5.0f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();

	//Dulce
	model = modelAux;
	model = glm::translate(model, glm::vec3(-2.0f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.8f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.6f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.4f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.2f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.0f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.8f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.6f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.4f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.2f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.2f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.4f, -3.0f, 5.5f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce
	model = modelAux;
	model = glm::translate(model, glm::vec3(-2.0f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.8f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.6f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.4f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.2f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-1.0f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.8f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.6f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.4f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(-0.2f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.0f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.2f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//Dulce2
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.4f, -2.8f, 5.25f));
	model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Dulce.RenderModel();
	//velas
	model = modelAux;
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 6.0f));
	//model = glm::translate(model, glm::vec3(2.5f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Velas.RenderModel();


	////Huesos
	//model = modelAux;
	//model = glm::translate(model, glm::vec3(1.6f, -4.6f, 4.0f));
	//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -20 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	////model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//Huesos.RenderModel();

	//fotos ofrenda
	model = modelAux;
	model = glm::translate(model, glm::vec3(1.5f, 1.2f, -3.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	digimon1.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	model = modelAux;
	model = glm::translate(model, glm::vec3(-2.5f, 1.2f, -3.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	digimon2.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	model = modelAux;
	model = glm::translate(model, glm::vec3(5.5f, 1.2f, -3.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	digimon3.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();


	//Mesa
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-45.6f, -3.6f, 5.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -60 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	MesaC.RenderModel();
	//cama
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-28.6f, -3.0f, -10.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Cama.RenderModel();
	//sillon Digimon
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-13.6f, -3.0f, 38.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Sillon.RenderModel();
	
	//papel picado
	//blending: transparencia o traslucidez de una imagen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-26.5f, 9.0f, 5.0f));
	//model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.0f, 3.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	papelPicado1.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	papelPicado2.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	papelPicado3.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//
	model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	papelPicado4.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	glDisable(GL_BLEND);
}

void DisplayEscenarioNavidad(glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	//Aqui se dibujan los elementos y modelos del cuarto de Navidad

	//Arbol de navidad
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-25.0f, -2.2f, -35.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Tree.RenderModel();

	//Regalo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-25.0f, -2.2f, -30.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Regalo.RenderModel();

	//Regalo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-20.0f, -3.2f, -35.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Regalo.RenderModel();
	//Regalo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-25.0f, -3.2f, -35.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Regalo.RenderModel();
	//Regalo
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-20.0f, -4.2f, -30.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 14.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Regalo.RenderModel();

	//Noche buena1
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-25.0f, -2.0f, -45.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	NocheBuena.RenderModel();

	//Noche buena2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-15.0f, -2.0f, -35.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	NocheBuena.RenderModel();


	//piñata
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-65.0f, 5.0f, -8.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Pinata.RenderModel();

	//corona
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-20.0f, -2.0f, -6.0f));
	model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//pisoTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Corona.RenderModel();

	//cama2
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-50.6f, -3.0f, -10.0f));
	model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Cama2.RenderModel();

	//cajas
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-35.0f, 8.0f, -2.0f));
	model = glm::rotate(model, -180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Cajas.RenderModel();

	//plano piso
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-35.0f, -1.3f, -40.0f));
	model = glm::scale(model, glm::vec3(0.42f, 0.42f, 0.42f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	terrainTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	//Casita
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-41.0f, -1.35f, -45.0f));
	model = glm::scale(model, glm::vec3(0.028f, 0.02f, 0.02f));
	model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Establo.RenderModel();

	//pisina
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-42.0f, -1.4f, -40.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Pisina.RenderModel();

	//colina
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-35.0f, -0.5f, -40.0f));
	model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	// .RenderModel();

	//nacimiento
	glm::mat4 modelTemp(1.0);
	//blending: transparencia o traslucidez de una imagen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-35.0f, -0.5f, -40.0f));
	modelTemp = model;

	//animales
	model = modelTemp;
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento5.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	model = modelTemp;
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento2.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//angeles y pastores, etc
	model = modelTemp;
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento6.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	model = modelTemp;
	model = glm::translate(model, glm::vec3(-2.5f, 0.0f, -0.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento3.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	model = modelTemp;
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento4.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//
	model = modelTemp;
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	nacimiento1.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	glDisable(GL_BLEND);

	
	
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	//CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(-37.0f, 3.0f, 12.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);
	camera_Anim =  Camera(glm::vec3(-37.0f, 3.0f, 12.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);


	//Textures
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/madera2.tga");
	pisoTexture.LoadTextureA();
	paredLadrilloTexture = Texture("Textures/ladrillos1.tga");
	paredLadrilloTexture.LoadTextureA();
	paredLadrillo2Texture = Texture("Textures/pared2.tga");
	paredLadrillo2Texture.LoadTextureA();
	RocaTex = Texture("Textures/pared2.png");
	RocaTex.LoadTextureA();
	PizzaTex = Texture("Textures/pared2.png");
	PizzaTex.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	terrainTexture = Texture("Textures/pasto.tga");
	terrainTexture.LoadTextureA();
	panMuertoTexture = Texture("Textures/ch1.png");
	panMuertoTexture.LoadTextureA();
	vidrioTexture = Texture("Textures/VIDRIO.tga");
	vidrioTexture.LoadTextureA();
	papelPicado1 = Texture("Textures/papel_picado1.tga");
	papelPicado1.LoadTextureA();
	papelPicado2 = Texture("Textures/papel_picado2.tga");
	papelPicado2.LoadTextureA();
	papelPicado3 = Texture("Textures/papel_picado3.tga");
	papelPicado3.LoadTextureA();
	papelPicado4 = Texture("Textures/ingenieria_papel.tga");
	papelPicado4.LoadTextureA();
	digimon1 = Texture("Textures/wizardmon.tga");
	digimon1.LoadTextureA();
	digimon2 = Texture("Textures/blackwargraymon.tga");
	digimon2.LoadTextureA();
	digimon3 = Texture("Textures/leomon.tga");
	digimon3.LoadTextureA();
	nacimiento1 = Texture("Textures/nacimiento1.tga");
	nacimiento1.LoadTextureA();
	nacimiento2 = Texture("Textures/animales1.tga");
	nacimiento2.LoadTextureA();
	nacimiento3 = Texture("Textures/reyesMagos.tga");
	nacimiento3.LoadTextureA();
	nacimiento4 = Texture("Textures/pastor1.tga");
	nacimiento4.LoadTextureA();
	nacimiento5 = Texture("Textures/animales2.tga");
	nacimiento5.LoadTextureA();
	nacimiento6 = Texture("Textures/angel1.tga");
	nacimiento6.LoadTextureA();
	

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Models
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	Windo = Model();
	Windo.LoadModel("Models/window.obj");

	Tree = Model();
	Tree.LoadModel("Models/10447_Pine_Tree_v1_L3b.obj");

	Roca = Model();
	Roca.LoadModel("Models/Pizza.obj");

	Pizza = Model();
	Pizza.LoadModel("Models/Pizza1.obj");

	Pan = Model();
	Pan.LoadModel("Models/Pan.obj");
	Calabaza = Model();
	Calabaza.LoadModel("Models/Pumpkin.obj");
	Mesa = Model();
	Mesa.LoadModel("Models/table.obj");
	
	Gisado = Model();
	Gisado.LoadModel("Models/13561_Shrimp_Sausage_Jambalaya_v1_L1.obj");
	
	Dulce = Model();
	Dulce.LoadModel("Models/HalloweenCandy.obj");

	/*PanMuerto = Model();
	PanMuerto.LoadModel("Models/Comida2.obj");*/

	NocheBuena = Model();
	NocheBuena.LoadModel("Models/Poinsetta.obj");
	
	Huesos = Model();
	Huesos.LoadModel("Models/skull.obj");
	
	Pinata = Model();
	Pinata.LoadModel("Models/pinata.obj");

	Corona = Model();
	Corona.LoadModel("Models/Componente_58.obj");


	MesaC = Model();
	MesaC.LoadModel("Models/mesa.obj");

	Cama = Model();
	Cama.LoadModel("Models/cama.obj");

	Cama2 = Model();
	Cama2.LoadModel("Models/cama2.obj");

	Comedor = Model();
	Comedor.LoadModel("Models/comedor.obj");

	Sillon = Model();
	Sillon.LoadModel("Models/sofaD.obj");

	Canasta = Model();
	Canasta.LoadModel("Models/canasta.obj");
	
	Cajas = Model();
	Cajas.LoadModel("Models/caja.obj");

	Velas = Model();
	Velas.LoadModel("Models/velas.obj");

	Digimon = Model();
	Digimon.LoadModel("Models/digimon.obj");

	Fantasma = Model();
	Fantasma.LoadModel("Models/fantasma.obj");

	Pisina = Model();
	Pisina.LoadModel("Models/lago.obj");

	Comida1 = Model();
	Comida1.LoadModel("Models/comida1.obj");

	Comida2 = Model();
	Comida2.LoadModel("Models/comida2.obj");

	Establo = Model();
	Establo.LoadModel("Models/casa2.obj");

	Regalo = Model();
	Regalo.LoadModel("Models/regalo.obj");

	Marco = Model();
	Marco.LoadModel("Models/marco.obj");

	Puerta = Model();
	Puerta.LoadModel("Models/puerta.obj");
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	/*pointLights[0] = PointLight(1.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;*/

	unsigned int spotLightCount = 0;
	//luz fija
	spotLights[0] = SpotLight(0.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-22.5f, 10.2f, 3.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	

	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		-30.5f, 10.2f, 3.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	//linterna
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/hills_ft.tga");
	skyboxFaces.push_back("Textures/Skybox/hills_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/hills_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/hills_up.tga");
	skyboxFaces.push_back("Textures/Skybox/hills_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/hills_lf.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);


	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();

		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();
		animacionCirculo();
		//Luces 
		Axi++;
		
		if (mainWindow.getsKeys()[GLFW_KEY_Z])
		{
			AuxL1 = true;
		}
		if (AuxL1 && Axi < 10) {
			
			pointLightCount = 0;
			pointLights[0] = PointLight(1.0f, 1.0f, 0.0f,
				0.0f, 1.0f,
				-26.3f, 2.2f, 10.8f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;
				
		}
		if (AuxL1 && Axi < 20 && Axi > 10 ) {
			pointLightCount = 0;
			pointLights[0] = PointLight(1.0f, 1.0f, 0.0f,
				0.0f, 1.0f,
				-28.3f, 3.0f, 11.8f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

		}
		if (Axi > 20) {
			Axi = 0;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_X]) {
			pointLightCount = 0;
			AuxL1 = false;
			AuxL2 = false;
			AuxL3 = false;
			AuxL4 = false;
			AuxL5 = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_C]) {

			AuxL2 = true;
			
		}
		Axj++;
		if (AuxL2||AuxL3)
		{
			printf("\n%d  ", Axj);
		}
		
		if (AuxL2 && Axj < 10) {

			pointLightCount = 0;
			pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
				0.0f, 1.0f,
				-25.0f, 2.2f, -35.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

		}
		if (AuxL2 && Axj < 20 && Axj > 10) {
			pointLightCount = 0;
			pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
				0.0f, 1.0f,
				-25.0f, 1.2f, -35.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

		}
		if (AuxL2 && Axj < 30 && Axj > 20) {
			pointLightCount = 0;
			pointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
				0.0f, 1.0f,
				-25.0f, 2.5f, -35.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

		}
		if (AuxL2 && Axj < 40 && Axj > 50) {
			pointLightCount = 0;
			pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
				0.0f, 1.0f,
				-25.0f, 1.5f, -35.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

		}

		if (Axj > 50) {
			Axj = 0;
		}


		if (mainWindow.getsKeys()[GLFW_KEY_V]) {
			AuxL3 = true;
		}
		if (AuxL3) {
			pointLightCount = 0;
			pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
				0.0f, 1.0f,
				-35.0f, 1.3f, -40.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_B]) {
			AuxL4 = true;
		}
		if (AuxL4)
		{
			spotLights[0] = SpotLight(0.0f, 1.0f, 1.0f,
				0.0f, 2.0f,
				-16.0f, 10.2f, -41.0f,
				0.0f, -5.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				10.0f);


			spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
				0.0f, 2.0f,
				-22.0f, 10.2f, -41.0f,
				0.0f, -5.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				10.0f);
		}
		else {

			spotLights[0] = SpotLight(0.0f, 1.0f, 1.0f,
				0.0f, 2.0f,
				-22.5f, 10.2f, 3.0f,
				0.0f, -5.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				10.0f);


			spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
				0.0f, 2.0f,
				-30.5f, 10.2f, 3.0f,
				0.0f, -5.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				10.0f);
		}
		if (mainWindow.getsKeys()[GLFW_KEY_N]) {
			AuxL5 = true;
		}
		if (AuxL5) {
			pointLightCount = 0;
			pointLights[0] = PointLight(0.0f, 1.0f, 1.0f,
				0.0f, 1.0f,
				-16.0f, -2.0f, 10.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;

			pointLights[1] = PointLight(0.0f, 1.0f, 1.0f,
				0.0f, 1.0f,
				-35.0f, -2.0f, 10.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount++;
		}

		banderaCamara = mainWindow.getBanderaCamara();

		if (play_Camera) {
			camera.setCameraPosition(glm::vec3(mov_x, mov_y, mov_z), giro_y, giro_z);
			//banderaCanasta = 1;
		}
		else {
			if (banderaCamara == 0) {
				camera.setCameraPosition(glm::vec3(cam3x, cam3y, cam3z), cam3yaw, cam3pitch);
				banderaCanasta = 1;
				camera.keyControl(mainWindow.getsKeys(), deltaTime);
			}
			if (banderaCamara == 1) {
				camera.setCameraPosition(glm::vec3(cam1x, cam1y, cam1z), cam1yaw, cam1pitch);
				banderaCanasta = 0;
				camera.keyControl(mainWindow.getsKeys(), deltaTime);
			}
			if (banderaCamara == 2) {
				camera.setCameraPosition(glm::vec3(cam2x, cam2y, cam2z), cam2yaw, cam2pitch);
				banderaCanasta = 0;
				camera.keyControl(mainWindow.getsKeys(), deltaTime);
			}
			if (banderaCamara == 3) {
				camera.setCameraPosition(glm::vec3(cam3x, cam3y, cam3z), cam3yaw, cam3pitch);
				banderaCanasta = 1;
				camera.keyControlFree(mainWindow.getsKeys(), deltaTime);
			}

			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();



		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[2].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		banderaL = mainWindow.getBandera();
		if (banderaL == 0) {
			spotLightCount = 2;
		}
		else {
			spotLightCount = 3;
		}

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelJerarquico(1.0);

		//plano terreno
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		terrainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		/*if (banderaCanasta == 1) {
			//Canasta con camara
			model = glm::mat4(1.0);
			model = glm::translate(model, camera.getCameraPosition());
			model = glm::rotate(model, -camera.getCameraYaw() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, camera.getCameraPitch() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(2.0f, -1.5f, 0.5f));
			modelJerarquico = model;
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Canasta.RenderModel();

			//añadir mas objetos de forma jerarquica a la canasta
		}
		else {*/
			//Canasta con camara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f + mov_x, 2.0f + mov_y, mov_z));
		//model = glm::translate(model, glm::vec3(3.0f + movAvion_x, 2.0f + movAvion_y, 0.0f + movAvion_z));
		//model = glm::rotate(model, -giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, cam3pitch * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.0f, -1.5f, 0.5f));
		modelJerarquico = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Canasta.RenderModel();
		//}

		//dibujando la estructura de la casa
		DisplayHouse(model, uniformModel, uniformSpecularIntensity, uniformShininess);

		//renderizado TODO LO QUE NO SEA CUARTO
		DisplayCasa(model, uniformModel, uniformSpecularIntensity, uniformShininess);

		//renderizado del cuarto navideño
		DisplayEscenarioNavidad(model, uniformModel, uniformSpecularIntensity, uniformShininess);

		//renderizado del cuarto de dia de muertos
		DisplayEscenarioMuertos(model, uniformModel, uniformSpecularIntensity, uniformShininess);

		
		//modelo helicoptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f + movAvion_x, 6.0f + movAvion_y, 0.0f + movAvion_z));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		model = glm::rotate(model, -giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();

		//vidrio con transparencia 1
		//blending: transparencia o traslucidez de una imagen
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-49.5f, 7.5f, -25.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 3.25f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vidrioTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();


		//vidrio con transparencia 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-49.5f, 7.5f, 25.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.5f, 3.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vidrioTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		glDisable(GL_BLEND);
		glUseProgram(0);

		//actualiza valores de la camara
		if (play_Camera == false) {

			if (banderaCamara == 0) {
				cam3x = camera.getCameraPosition().x;
				cam3y = camera.getCameraPosition().y;
				cam3z = camera.getCameraPosition().z;
				cam3yaw = camera.getCameraYaw();
				cam3pitch = camera.getCameraPitch();
			}
			if (banderaCamara == 1) {
				cam1x = camera.getCameraPosition().x;
				cam1y = camera.getCameraPosition().y;
				cam1z = camera.getCameraPosition().z;
				cam1yaw = camera.getCameraYaw();
				cam1pitch = camera.getCameraPitch();
			}
			if (banderaCamara == 2) {
				cam2x = camera.getCameraPosition().x;
				cam2y = camera.getCameraPosition().y;
				cam2z = camera.getCameraPosition().z;
				cam2yaw = camera.getCameraYaw();
				cam2pitch = camera.getCameraPitch();
			}
			if (banderaCamara == 3) {
				cam3x = camera.getCameraPosition().x;
				cam3y = camera.getCameraPosition().y;
				cam3z = camera.getCameraPosition().z;
				cam3yaw = camera.getCameraYaw();
				cam3pitch = camera.getCameraPitch();
			}
		}

		mainWindow.swapBuffers();
	}

	return 0;
}