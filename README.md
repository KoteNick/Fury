# Fury

Open-source game/rendering engine boilerplate.
Dont want to bother making your own engine from scratch, but want to have a full control of abstactions and pipeline? **Fury** is just for you!
Fork the repo and do whatever you want, whether it be making your own engine on this basis or making a game.

Currently supports only OpenGL.

## Structure

**Fury** is meant to be modular and have an hierarchy of customization.
Its made of three parts:
1. **Game** - Assets, Scenes and whatever else you might write, primary place to write your own code.
2. **Core** - Input system, `Entity` class, Components - stuff that makes up the inner guts of an engine.
3. **Renderer** - Abstractions around the raw OpenGL calls and Render Queue hadler. Probably the last thing you wanna touch

As you can see, **Fury** has a hierarchy - you can get the job done by only making/changing files in the Game folder, but if you need or want to add or change something about the engine, other parts are open to you.

## Getting started

So how do you make something with the engine?

### Adding Assets

There are three assets **Fury** holds:
#### Meshes
As of right now, **Fury** cannot load models.
However you can either make a mesh yourself (by manually writing vertex data into a vector of vertices and then creating a `MeshData` object), or use a Primitives class to generate primitves
Meshes are static and, as of now, doesn't hold any data after loading itself on GPU, so you cannot change them. Also they are provided and stored throughout thre project as pointers.
```cpp
    std::vector<Vertex2D> v = {
            {+0.0, +0.5, 1.0, 1.0, 1.0},
            {-0.5, -0.5, 1.0, 1.0, 0.0},
            {+0.5, -0.5, 0.0, 1.0, 1.0}
    };
    VertexData vd(v);
    vd.layout.Push<float>(2).Push<float>(3);
    AddMesh("Triangle", vd);

    // or use Primitives
    AddMesh("Plane100250", Primitives::CreatePlane(100, 100, 250, 250));
```

#### Shaders
Loaded from file, provided a type and compiled. Texture slots and uniform blocks are automated, so their name must be the same as expected. See `Renderer/RendererTypes.h`
Shaders are also static, so provided as pointers.
```cpp
    AddShader("blinn_phong")
        .AddProgram("assets/shaders/blinn_phong.frag", ShaderType::FRAGMENT)
        .AddProgram("assets/shaders/blinn_phong.vert", ShaderType::VERTEX)
        .Build();
```
#### Materials
Materials hold textures and shaders of objects to streamline rendering process. They also store uniform values locally and sync GPU with them.
Materials can be different for every object, so they are provided as objects.
```cpp
    AddMaterial("light3d", Material(GetShader("blinn_phong")));
```
As you can see they use the shader pointer in constructor.

### Making a scene

Game class holds the `currentScene` pointer to a `BaseScene` class. There are two ways to make a scene.
1. Make a `BaseScene` object and fill it up with entities in the Game.cpp or somewhere else.
2. Make a child of `BaseScene` inside Game/Scenes folder, to make separate scenes in a cleaner way, writing the specific scene code inside of the classes. There is a `Scene.cpp` and `Scene.h` files to copy for your convenience.

`BaseScene` has a `OnRender` method that automatically sends objects to a `RenderQueue`, while also making specific actions with light source objects and camera
`camera` is an Entity pointer in the scene that you might want to set to your camera object.
Scenes hold entities inside, you should use `CreateEntity` and `GetEntity` methods.

Example of a scene you might make:
```cpp
#include "TestScene.h"

void TestScene::Init()
{
    camera = &CreateEntity("Camera")
        .AddComponent(TransformComponent(0, 0, 3))
        .AddComponent(CameraComponent());

    CreateEntity("Cube")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Cube"), Assets::GetMaterial("light3d")));

    CreateEntity("Plane")
        .AddComponent(TransformComponent(0, -2, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane100"), Assets::GetMaterial("light3d")));

    CreateEntity("Light")
        .AddComponent(TransformComponent(0, 3, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
    .Transform()->scale = glm::vec3(0.1);

    CreateEntity("Sun")
        .AddComponent(TransformComponent(5, 3, 0))
        .AddComponent(DirectionalLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
    .GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1.0, 0.87, 0.13, 1. };

    GetEntity("Sphere")->GetComponent<RenderableComponent>()->material.uniform<float>("u_Shininess") = 128;
}

void TestScene::OnUpdate(float deltaTime)
{
    UpdateEntities(deltaTime);

    auto& sun = *GetEntity("Sun");
    auto& light = *GetEntity("Light");
    auto& cube = *GetEntity("Cube");

    static float orbitTime = 0.0f;
    orbitTime += deltaTime * 0.5f;

    float radius = 6.0f;
    float height = 5.0f;

    glm::vec3 newPos;
    newPos.x = cos(orbitTime) * radius;
    newPos.y = height;
    newPos.z = sin(orbitTime) * radius;

    sun.Transform()->pos = newPos;
    sun.Transform()->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    sun.Transform()->OnUpdate(deltaTime);

    float speed = 5.0f * deltaTime;

    auto tr = cube.Transform();
    if (tr->pos.x >= 1.5)
        tr->pos.x = -1.5;
    tr->pos.x += 1 * deltaTime;
    tr->rot += glm::vec3(45) * deltaTime;
}
```

And now to see the scene you only need one line in Game.cpp
```cpp
currentScene = new TestScene();
```

### Customization

This engine is very barebones, so you might need something that isnt realised yet. 

For Renderer customization you might first look into `Renderer/RendererTypes.h`.
Also `Material` class might not have all of the uniform types you might use, but it is easily fixable by yourself.

You are, of course, free to do whatever you want, but to keep things organized and working well there are few recommendations:
* Don't write GL code in other parts besides Renderer
* Add assets with the Assets class. Either in the Assets.cpp or somewhere else (for dynamic level loading, for example)