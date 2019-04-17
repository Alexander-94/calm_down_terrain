/** Example 010 Shaders

This tutorial shows how to use shaders for D3D8, D3D9, OpenGL, and Cg with the
engine and how to create new material types with them. It also shows how to
disable the generation of mipmaps at texture loading, and how to use text scene
nodes.

This tutorial does not explain how shaders work. I would recommend to read the
D3D, OpenGL, or Cg documentation, to search a tutorial, or to read a book about
this.

At first, we need to include all headers and do the stuff we always do, like in
nearly all other tutorials:
*/
#include <irrlicht.h>
#include <iostream>
#include <random>
#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
Because we want to use some interesting shaders in this tutorials, we need to
set some data for them to make them able to compute nice colors. In this
example, we'll use a simple vertex shader which will calculate the color of the
vertex based on the position of the camera.
For this, the shader needs the following data: The inverted world matrix for
transforming the normal, the clip matrix for transforming the position, the
camera position and the world position of the object for the calculation of the
angle of light, and the color of the light. To be able to tell the shader all
this data every frame, we have to derive a class from the
IShaderConstantSetCallBack interface and override its only method, namely
OnSetConstants(). This method will be called every time the material is set.
The method setVertexShaderConstant() of the IMaterialRendererServices interface
is used to set the data the shader needs. If the user chose to use a High Level
shader language like HLSL instead of Assembler in this example, you have to set
the variable name as parameter instead of the register index.
*/

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;
#include <random>
#define SPEED 10
extern bool fullscreen = false;

IrrlichtDevice*  device = 0;  // окно которое выдает движок для работы с трехмерным миром
IVideoDriver*    driver;      // то через что выводится вся графика
ISceneManager*   smgr;        // менеджер сцены - через него делается почти вся работа над объектами
IGUIEnvironment* guienv;      // графический интерфейс

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		//??? irr::EMIE_MOUSE_WHEEL
		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver()
	{
		for (u32 i = 0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

irr::f32 mouseWheel;
irr::s32 framelimit = 60,
		 now = 0;
irr::u32 sceneStartTime = 0,
		 sceneSkipTime = 1000 / framelimit;


irr::core::vector3df vector_combine(irr::core::vector3df v1, irr::core::vector3df v2, float f1, float f2) {
	irr::core::vector3df result;
	result.X = f1 * v1.X + f2 * v2.X;
	result.Y = f1 * v1.Y + f2 * v2.Y;
	result.Z = f1 * v1.Z + f2 * v2.Z;
	return result;
}

struct tile {
	irr::core::vector3df vertm1;
	int left_up_z;
	int right_up_z;
	int left_down_z;
	int right_down_z;
	int center_z;

	irr::video::SColor color;
};

int main()
{
	//The event receiver for keeping the pressed keys is ready, the actual responses will be made inside the render loop, right before drawing the scene.
	MyEventReceiver receiver;
	//the root object for doing anything with the engine
	device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<u32>(1080, 720), 32, false, false, false, &receiver);
	if (!device)
		return 1;
	device->setWindowCaption(L"Hello World! - Irrlicht CALM DOWN Demo");
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
	guienv->addStaticText(L"Q - Up, E - Down, WASD - movement, ESC - exit", rect<s32>(10, 10, 260, 22), true);

	SKeyMap keyMap[8];
	keyMap[0].Action = EKA_MOVE_FORWARD;
	keyMap[0].KeyCode = KEY_UP;
	keyMap[1].Action = EKA_MOVE_BACKWARD;
	keyMap[1].KeyCode = KEY_DOWN;
	keyMap[2].Action = EKA_STRAFE_LEFT;
	keyMap[2].KeyCode = KEY_LEFT;
	keyMap[3].Action = EKA_STRAFE_RIGHT;
	keyMap[3].KeyCode = KEY_RIGHT;
	keyMap[4].Action = EKA_MOVE_FORWARD;
	keyMap[4].KeyCode = KEY_KEY_W;
	keyMap[5].Action = EKA_MOVE_BACKWARD;
	keyMap[5].KeyCode = KEY_KEY_S;
	keyMap[6].Action = EKA_STRAFE_LEFT;
	keyMap[6].KeyCode = KEY_KEY_A;
	keyMap[7].Action = EKA_STRAFE_RIGHT;
	keyMap[7].KeyCode = KEY_KEY_D;

	//int side_size = 10000;
	int pol_width = 150;     //tile width
	const int size = 70;     //N of tiles
	const int vertexes = 12;

	SMesh* mesh = new SMesh();
	SMeshBuffer *mesh_buffer = new SMeshBuffer();
	mesh->addMeshBuffer(mesh_buffer);
	mesh_buffer->drop();

	mesh_buffer->Vertices.reallocate(size*size*vertexes); //allocate space for vertices
	mesh_buffer->Vertices.set_used(size*size*vertexes);   //now you can access indices 0..20*20*6
	
	//generate rnd height of Z axis
	std::random_device g;
	std::mt19937 rng(g());
	std::uniform_int_distribution<std::mt19937::result_type> generate(-100.0, 100);

	struct tile tiles[size][size];  //producing the 2dimension (XYZ + SColor) structure, so that we can store the values
	int vert_count = 0;
	irr::f32 delta = 2.0f;

	int center_z = 0;
	int left_up_z = 0;
	int right_up_z = 0;
	int left_down_z = 0;
	int right_down_z = 0;

	//random generating of height of squares
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {

			if (x == 0 && y == 0) { //generate all 4 corners - first square of first row
				tiles[x][y].left_up_z = generate(rng);
				tiles[x][y].right_up_z = generate(rng);
				tiles[x][y].left_down_z = generate(rng);
				tiles[x][y].right_down_z = generate(rng);			
			}
			else if (x == 0 && y > 0) { //generate 2 right corners, take 2 left corners from prev square - other squares of first row
				tiles[x][y].left_up_z = tiles[x][y - 1].right_up_z;
				tiles[x][y].right_up_z = generate(rng);
				tiles[x][y].left_down_z = tiles[x][y - 1].right_down_z;
				tiles[x][y].right_down_z = generate(rng);
			}
			else if (x > 0 && y == 0) { // take two up corners from first line first square - first square of N row
				tiles[x][y].left_up_z = tiles[x-1][y].left_down_z;
				tiles[x][y].right_up_z = tiles[x-1][y].right_down_z;
				tiles[x][y].left_down_z = generate(rng);
				tiles[x][y].right_down_z = generate(rng);
			}
			else if (x > 0 && y > 0){ // take three corners from prev squares - other squares of N rows
				tiles[x][y].left_up_z = tiles[x][y-1].right_up_z;
				tiles[x][y].right_up_z = tiles[x-1][y].right_down_z;
				tiles[x][y].left_down_z = tiles[x][y-1].right_down_z;
				tiles[x][y].right_down_z = generate(rng);
			}
		    							
			tiles[x][y].center_z = generate(rng);
		}
	}

	for (int x = 0; x < size; x++) {     //size  2
		for (int y = 0; y < size; y++) { //size  5

			int pick_color = generate(rng);
			if (pick_color < -50) {
				tiles[x][y].color = irr::video::SColor(100, 2, 100, 248);
			} 
			else if(pick_color < 0) {
				tiles[x][y].color = irr::video::SColor(255, 2, 100, 144);
			} 
			else if (pick_color < 50) {
				tiles[x][y].color = irr::video::SColor(100, 2, 157, 144);
			}
			else if (pick_color < 100) {
				tiles[x][y].color = irr::video::SColor(100, 2, 101, 49);
			}
						
			tiles[x][y].vertm1.X = x*pol_width;
			tiles[x][y].vertm1.Y = y*pol_width;								    
			//                                                                                                Z
			mesh_buffer->Vertices[vert_count] =      S3DVertex(tiles[x][y].vertm1.X + delta,                  tiles[x][y].left_up_z, tiles[x][y].vertm1.Y + delta, 0, 0, 0, tiles[x][y].color, 0, 0);             //*1 по х проход
			mesh_buffer->Vertices[vert_count + 1] =  S3DVertex(tiles[x][y].vertm1.X + delta,                  tiles[x][y].right_up_z, tiles[x][y].vertm1.Y + pol_width - delta, 0, 0, 0, tiles[x][y].color, 0, 0); //*1 по х проход
			mesh_buffer->Vertices[vert_count + 2] =  S3DVertex(tiles[x][y].vertm1.X + pol_width / 2 - delta,  tiles[x][y].center_z, tiles[x][y].vertm1.Y + pol_width / 2, 0, 0, 0, tiles[x][y].color, 0, 0);                //center *1 по х проход

			mesh_buffer->Vertices[vert_count + 3] =  S3DVertex(tiles[x][y].vertm1.X + delta,                  tiles[x][y].right_up_z, tiles[x][y].vertm1.Y + pol_width - delta, 0, 0, 0, tiles[x][y].color, 0, 0);                  //*1 по х проход
			mesh_buffer->Vertices[vert_count + 4] =  S3DVertex(tiles[x][y].vertm1.X + pol_width - delta,      tiles[x][y].right_down_z, tiles[x][y].vertm1.Y + pol_width - delta, 0, 0, 0, tiles[x][y].color, 0, 0);                  //*новый y проход, у++
			mesh_buffer->Vertices[vert_count + 5] =  S3DVertex(tiles[x][y].vertm1.X + pol_width / 2,          tiles[x][y].center_z, tiles[x][y].vertm1.Y + pol_width / 2 + delta, 0, 0, 0, tiles[x][y].color, 0, 0);        //center *1 по х проход

			mesh_buffer->Vertices[vert_count + 6] =  S3DVertex(tiles[x][y].vertm1.X + pol_width - delta,      tiles[x][y].right_down_z, tiles[x][y].vertm1.Y + pol_width - delta, 0, 0, 0, tiles[x][y].color, 0, 0);                  //*новый y проход, у++
			mesh_buffer->Vertices[vert_count + 7] =  S3DVertex(tiles[x][y].vertm1.X + pol_width - delta,      tiles[x][y].left_down_z, tiles[x][y].vertm1.Y + delta, 0, 0, 0, tiles[x][y].color, 0, 0);                              //*новый y проход, у++
			mesh_buffer->Vertices[vert_count + 8] =  S3DVertex(tiles[x][y].vertm1.X + pol_width / 2 + delta,  tiles[x][y].center_z, tiles[x][y].vertm1.Y + pol_width / 2, 0, 0, 0, tiles[x][y].color, 0, 0);                //center *1 по х проход

			mesh_buffer->Vertices[vert_count + 9] =  S3DVertex(tiles[x][y].vertm1.X + pol_width - delta,      tiles[x][y].left_down_z, tiles[x][y].vertm1.Y + delta, 0, 0, 0, tiles[x][y].color, 0, 0);                              //*новый y проход, у++
			mesh_buffer->Vertices[vert_count + 10] = S3DVertex(tiles[x][y].vertm1.X + delta,                  tiles[x][y].left_up_z, tiles[x][y].vertm1.Y + delta, 0, 0, 0, tiles[x][y].color, 0, 0);                              //*1 по х проход
			mesh_buffer->Vertices[vert_count + 11] = S3DVertex(tiles[x][y].vertm1.X + pol_width / 2,          tiles[x][y].center_z, tiles[x][y].vertm1.Y + pol_width / 2 - delta, 0, 0, 0, tiles[x][y].color, 0, 0);        //center *1 по х проход

			vert_count += 12;			
		}		
	}
	
	irr::core::vector3df v1;
	v1.X = 0;
	v1.Z = 0;
    v1.Y = 0;
	printf("\nV1--X:%f Y:%f Z:%f", v1.X, v1.Z, v1.Y);
	irr::core::vector3df v2;
	v2.X = 0;
	v2.Z = 10;
	v2.Y = 0;
	printf("\nV2--X:%f Y:%f Z:%f", v2.X, v2.Z, v2.Y);

	irr::f32 gap = 5.0f;
	irr::core::vector3df temp = vector_combine(v1, v2, gap, gap);

	printf("\nVcomb--X:%f Y:%f Z:%f", temp.X, temp.Z, temp.Y);

	mesh_buffer->Indices.reallocate(size*size*vertexes);  //allocate space for indices
	mesh_buffer->Indices.set_used(size*size*vertexes);
	for (int i = 0; i < size * size * vertexes; ++i) {
		mesh_buffer->Indices[i] = i;
	}

	mesh_buffer->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	IMeshSceneNode* myNode = smgr->addMeshSceneNode(mesh);
	myNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode->setMaterialFlag(EMF_LIGHTING, false);
	myNode->setAutomaticCulling(EAC_FRUSTUM_SPHERE);
	//myNode->setMaterialFlag(video::EMF_WIREFRAME, true);
	myNode->setPosition(vector3df(0, 0, 0));


	ICameraSceneNode* camera = 0;
	camera = smgr->addCameraSceneNodeFPS(0, 75.0f, 1.0f, -1, keyMap, 8);
	camera->setPosition(vector3df(5000.0f, 500.0f, 5000.0f));
	//camera->setTarget(vector3df(0.0f, 0.0f, 100.0f));
	camera->setFarValue(20000.f);//20000
	camera->setNearValue(1.0f);
			
	//The game cycle:
	//We run the device in a while() loop, until the device does not want to run any more. 
	//This would be when the user closes the window
	while (device->run())
	{
		now = device->getTimer()->getTime();      //set current time to var
		if (now - sceneStartTime > sceneSkipTime)
		{
			sceneStartTime = device->getTimer()->getTime();
			if (receiver.IsKeyDown(irr::KEY_ESCAPE))
			{
				device->drop();
				exit(0);
			}
			driver->beginScene(true, true, SColor(255, 157, 212, 140));
			smgr->drawAll();
//			guienv->drawAll();
			driver->endScene();
		}
	}
	device->closeDevice();
	device->drop();
	return 0;
}
