#include <iostream>
#include <dlfcn.h>
#include "BlockInterface.h"
#include "BlockChain.h"
#include "BlockLibraryPaths.h"


void handleMessage(const std::string& message, const BlockData & data) {
    std::cout << "console: " << message << std::endl;
}
// Function to load a dynamic library and create a block instance
BlockInterface* loadAndCreateBlockInstance(
    const std::string& libBaseName,
    const std::string& path
    
) {
    // Append the '.dylib' extension to the base name
    std::string fullLibName = libBaseName + ".dylib";

    // Construct the full path to the dynamic library
    std::string fullLibPath = path + fullLibName;

    // Load the dynamic library
    void* libHandle = dlopen(fullLibPath.c_str(), RTLD_LAZY);
    if (!libHandle) {
        std::cerr << "Failed to open dynamic library: " << dlerror() << std::endl;
        return nullptr;
    }

    // Retrieve the factory function from the library
    auto createInstance = (CreateBlockInstanceFunction)dlsym(libHandle, "createBlockInstance");
    if (!createInstance) {
        std::cerr << "Failed to find factory function 'createBlockInstance': " << dlerror() << std::endl;
        dlclose(libHandle);
        return nullptr;
    }


    // Use the factory function to create an instance of the block
    BlockInterface* blockInstance = createInstance();
    if (!blockInstance) {
        std::cerr << "Failed to create block instance." << std::endl;
        dlclose(libHandle);
        return nullptr;
    }

//    blockInstance->init(args, messageCallback, outletCallback);
//    if(blockInstance->handlesAudioProcessing()){
//        blockInstance->configureAudioParameters(512, 44100);
//    }
    // Return the created block instance
    return blockInstance;
}



int main(int argc, char** argv) {
    
    const auto library = argc > 1
        ? std::filesystem::path(argv[1]) / "myBlock.dylib"
        : BlockLibraryPaths::find("myBlock", argv[0]);
    if (library.empty()) {
        std::cerr << "Build the modules first, or pass the module directory as the first argument." << std::endl;
        return 1;
    }
    const std::string basePath = library.parent_path().string() + "/";

    
     // Instantiate the block system
     BlockChain system;

    
    // Create an instance of 'MyBlock' block
    BlockInterface* block1 = loadAndCreateBlockInstance("myBlock", basePath);
    if (block1 == nullptr) return 1;
    block1->init(Number(2), system.systemMessageCallback,system.outletCallback);
    
    BlockInterface* block2 = loadAndCreateBlockInstance("myBlock", basePath);
    if (block2 == nullptr) return 1;
    block2->init(Number(2), system.systemMessageCallback,system.outletCallback);
    
    BlockInterface* block3 = loadAndCreateBlockInstance("myBlock", basePath);
    if (block3 == nullptr) return 1;
    block3->init(Number(2), system.systemMessageCallback,system.outletCallback);
    
    BlockInterface* block4 = loadAndCreateBlockInstance("print", basePath);
    if (block4 == nullptr) return 1;
    block4->init(Number(2), system.systemMessageCallback,system.outletCallback);
  
     // Add blocks to the system (ownership is transferred to the system)
    system.addBlock(block1);
    system.addBlock(block2);
    system.addBlock(block3);
    system.addBlock(block4);
    
    
    std::cout<<block1<<std::endl;
    std::cout<<block2<<std::endl;
    std::cout<<block3<<std::endl;
    std::cout<<block4<<std::endl;
    
     // Connect block1's first output to block2's first input
    system.connectBlocks(block1, 1, block2, 0);
    system.connectBlocks(block1, 0, block2, 1);

    system.connectBlocks(block2, 1, block3, 0);
    system.connectBlocks(block3, 1, block4, 0);
    
    //-----------------------------------------------
//    system.connectBlocks(block1,0,block4,0);
//    system.connectBlocks(block1,1,block4,0);
//    system.connectBlocks(block2,0,block4,0);
//    system.connectBlocks(block2,1,block4,0);
//    system.connectBlocks(block3,0,block4,0);
//    system.connectBlocks(block3,1,block4,0);

    // Send input to block1 and process both blocks
//    block1->setInput(1, Number(1));
    block1->setInput(0, Number(5));
//    system.disconnectBlocks(block2, 0, block3, 0);
    block1->setInput(0, Message("Trigger"));
    block3->setInput(0, Message("Trigger"));
    
    block1->setInput(0, Message("hold 1 2 4  6.5 3 inster 323 2.5"));
    block1->setInput(0, Message("getInfo"));
    
    block1->setInput(0,  List{
        
        (Message("factor")),
        (Number(23.55))
//        ListElement(Number(999)),
//        ListElement(Message("working class hero!"))
    });

    system.disconnectBlocks(block1, 0, block2, 1);
    system.disconnectBlocks(block1, 1, block2, 0);
    
    block1->setInput(0, Message("Trigger"));

    
    std::cout<<"---------------"<<std::endl;

    
     return 0;
 }
