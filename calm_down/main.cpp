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

	int side_size = 1000;//0;
	int pol_width = 200;
	int size = side_size / pol_width;
	int vertexes = 12;

	SMesh* mesh0 = new SMesh();
	SMeshBuffer *mesh_buffer0 = new SMeshBuffer();
	mesh0->addMeshBuffer(mesh_buffer0);
	mesh_buffer0->drop();

	SMesh* mesh1 = new SMesh();
	SMeshBuffer *mesh_buffer1 = new SMeshBuffer();
	mesh1->addMeshBuffer(mesh_buffer1);
	mesh_buffer1->drop();

	SMesh* mesh2 = new SMesh();
	SMeshBuffer *mesh_buffer2 = new SMeshBuffer();
	mesh2->addMeshBuffer(mesh_buffer2);
	mesh_buffer2->drop();

	SMesh* mesh3 = new SMesh();
	SMeshBuffer *mesh_buffer3 = new SMeshBuffer();
	mesh3->addMeshBuffer(mesh_buffer3);
	mesh_buffer3->drop();

	SMesh* mesh4 = new SMesh();
	SMeshBuffer *mesh_buffer4 = new SMeshBuffer();
	mesh4->addMeshBuffer(mesh_buffer4);
	mesh_buffer4->drop();

	SMesh* mesh5 = new SMesh();
	SMeshBuffer *mesh_buffer5 = new SMeshBuffer();
	mesh5->addMeshBuffer(mesh_buffer5);
	mesh_buffer5->drop();

	SMesh* mesh6 = new SMesh();
	SMeshBuffer *mesh_buffer6 = new SMeshBuffer();
	mesh6->addMeshBuffer(mesh_buffer6);
	mesh_buffer6->drop();

	SMesh* mesh7 = new SMesh();
	SMeshBuffer *mesh_buffer7 = new SMeshBuffer();
	mesh7->addMeshBuffer(mesh_buffer7);
	mesh_buffer7->drop();

	SMesh* mesh8 = new SMesh();
	SMeshBuffer *mesh_buffer8 = new SMeshBuffer();
	mesh8->addMeshBuffer(mesh_buffer8);
	mesh_buffer8->drop();

	mesh_buffer0->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer0->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer1->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer1->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer2->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer2->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer3->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer3->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer4->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer4->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer5->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer5->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer6->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer6->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer7->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer7->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	mesh_buffer8->Vertices.reallocate(vertexes); //allocate space for vertices
	mesh_buffer8->Vertices.set_used(vertexes);   //now you can access indices 0..20*20*6

	int vert_count = 0;
	float delta = 10;
	for (int x = 0; x < 1; x++) {//size		
		for (int y = 0; y < 1; y++) {//size
			mesh_buffer0->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer0->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer0->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer0->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer0->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer0->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			//vert_count += 12;
		}
	}

	for (int x = 0; x < 1; x++) {//size		
		for (int y = 1; y < 2; y++) {//size
			mesh_buffer1->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer1->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer1->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer1->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer1->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer1->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			//vert_count += 12;
		}
	}


	for (int x = 0; x < 1; x++) {//size		
		for (int y = 2; y < 3; y++) {//size
			mesh_buffer4->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer4->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer4->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer4->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer4->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer4->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			//vert_count += 12;
		}
	}

	for (int x = 1; x < 2; x++) {//size		
		for (int y = 0; y < 1; y++) {//size
			mesh_buffer2->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer2->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer2->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer2->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer2->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer2->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

		}
	}

	for (int x = 1; x < 2; x++) {//size		
		for (int y = 1; y < 2; y++) {//size
			mesh_buffer3->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer3->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer3->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer3->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer3->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer3->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

		}
	}

	for (int x = 1; x < 2; x++) {//size		
		for (int y = 2; y < 3; y++) {//size
			mesh_buffer5->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer5->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer5->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer5->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer5->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer5->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

		}
	}

	for (int x = 2; x < 3; x++) {//size		
		for (int y = 0; y < 1; y++) {//size
			mesh_buffer6->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer6->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer6->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer6->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer6->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer6->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
		}
	}

	for (int x = 2; x < 3; x++) {//size		
		for (int y = 1; y < 2; y++) {//size
			mesh_buffer7->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer7->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer7->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer7->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer7->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer7->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
		}
	}

	for (int x = 2; x < 3; x++) {//size		
		for (int y = 2; y < 3; y++) {//size
			mesh_buffer8->Vertices[vert_count] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 1] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 2] = S3DVertex(x*pol_width + pol_width / 2 - delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer8->Vertices[vert_count + 3] = S3DVertex(x*pol_width + delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 4] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 150);
			mesh_buffer8->Vertices[vert_count + 5] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer8->Vertices[vert_count + 6] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + pol_width - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 7] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 8] = S3DVertex(x*pol_width + pol_width / 2 + delta, 0, y*pol_width + pol_width / 2, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);

			mesh_buffer8->Vertices[vert_count + 9] = S3DVertex(x*pol_width + pol_width - delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 10] = S3DVertex(x*pol_width + delta, 0, y*pol_width + delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
			mesh_buffer8->Vertices[vert_count + 11] = S3DVertex(x*pol_width + pol_width / 2, 0, y*pol_width + pol_width / 2 - delta, 0, 0, 0, video::SColor(255, 0, 0, 0), 0, 0);
		}
	}


	mesh_buffer0->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer0->Indices.set_used(vertexes);

	mesh_buffer1->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer1->Indices.set_used(vertexes);

	mesh_buffer2->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer2->Indices.set_used(vertexes);

	mesh_buffer3->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer3->Indices.set_used(vertexes);

	mesh_buffer4->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer4->Indices.set_used(vertexes);

	mesh_buffer5->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer5->Indices.set_used(vertexes);

	mesh_buffer6->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer6->Indices.set_used(vertexes);

	mesh_buffer7->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer7->Indices.set_used(vertexes);

	mesh_buffer8->Indices.reallocate(vertexes);  //allocate space for indices
	mesh_buffer8->Indices.set_used(vertexes);

	for (int i = 0; i < vertexes; ++i) {
		mesh_buffer0->Indices[i] = i;
		mesh_buffer1->Indices[i] = i;
		mesh_buffer2->Indices[i] = i;
		mesh_buffer3->Indices[i] = i;
		mesh_buffer4->Indices[i] = i;
		mesh_buffer5->Indices[i] = i;
		mesh_buffer6->Indices[i] = i;
		mesh_buffer7->Indices[i] = i;
		mesh_buffer8->Indices[i] = i;
	}

	mesh_buffer0->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer1->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer2->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer3->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer4->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer5->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer6->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer7->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.
	mesh_buffer8->recalculateBoundingBox(); //Recalculate the bounding box. should be called if the mesh changed.

	IMeshSceneNode* myNode0 = smgr->addMeshSceneNode(mesh0);
	myNode0->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode0->setMaterialFlag(EMF_LIGHTING, false);
	myNode0->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode1 = smgr->addMeshSceneNode(mesh1);
	myNode1->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode1->setMaterialFlag(EMF_LIGHTING, false);
	myNode1->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode2 = smgr->addMeshSceneNode(mesh2);
	myNode2->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode2->setMaterialFlag(EMF_LIGHTING, false);
	myNode2->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode3 = smgr->addMeshSceneNode(mesh3);
	myNode3->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode3->setMaterialFlag(EMF_LIGHTING, false);
	myNode3->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode4 = smgr->addMeshSceneNode(mesh4);
	myNode4->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode4->setMaterialFlag(EMF_LIGHTING, false);
	myNode4->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode5 = smgr->addMeshSceneNode(mesh5);
	myNode5->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode5->setMaterialFlag(EMF_LIGHTING, false);
	myNode5->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode6 = smgr->addMeshSceneNode(mesh6);
	myNode6->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode6->setMaterialFlag(EMF_LIGHTING, false);
	myNode6->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode7 = smgr->addMeshSceneNode(mesh7);
	myNode7->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode7->setMaterialFlag(EMF_LIGHTING, false);
	myNode7->setPosition(vector3df(0, 0, 0));

	IMeshSceneNode* myNode8 = smgr->addMeshSceneNode(mesh8);

	myNode8->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);  //render backside of the mesh
	myNode8->setMaterialFlag(EMF_LIGHTING, false);
	myNode8->setPosition(vector3df(0, 0, 0));
	myNode8->setAutomaticCulling(EAC_FRUSTUM_SPHERE);


	ICameraSceneNode* camera = 0;
	//camera = smgr->addCameraSceneNodeFPS(0, 75.0f, 1.0f, -1, keyMap, 8);
	camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(300.0f, 200.0f, 300.0f));
	//camera->setTarget(vector3df(0.0f, 0.0f, 100.0f));
	camera->setFarValue(20000.f);//20000
	camera->setNearValue(1.0f);
	//

	
	
	irr::f32 a = camera->getFOV();
	printf("\n--%f--\n", a); //1.2566

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
