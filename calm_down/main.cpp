#include <irrlicht.h>
#include <S3DVertex.h>
#include <cmath>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class MyEventReceiver: public irr::IEventReceiver {
public:
    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        }
        return false;
    }
    virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const {
        return KeyIsDown[keyCode];
    }
    MyEventReceiver() {
        for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i) {
            KeyIsDown[i] = false;
        }
    }
private:
    bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};


int main() {

    
    MyEventReceiver receiver;

    irr::IrrlichtDevice *device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(1280, 720), 32, false, false, false, &receiver);
    if (!device) {
        return 1;
    }
    irr::video::IVideoDriver *driver = device->getVideoDriver();
    irr::scene::ISceneManager *manager = device->getSceneManager();
    irr::scene::ISceneNode *empty = manager->addEmptySceneNode();
    irr::scene::ISceneNode *camera_target = manager->addEmptySceneNode(empty);

    irr::scene::SMesh *mesh = new irr::scene::SMesh();
    irr::scene::SMeshBuffer *buffer = new irr::scene::SMeshBuffer();
    mesh->addMeshBuffer(buffer);
    buffer->drop();



    buffer->recalculateBoundingBox();

    irr::scene::IMeshSceneNode *node = manager->addMeshSceneNode(mesh);

    node->setMaterialFlag(irr::video::EMF_LIGHTING, false);

    while (device->run()) {
    
    	
        driver->beginScene(true, true, irr::video::SColor(0, 0, 0, 0));
        manager->drawAll();
        driver->endScene();
    }

    device->drop();

    return EXIT_SUCCESS;

}
