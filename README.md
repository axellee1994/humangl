# humangl

A 42 school project introducing hierarchical modeling with OpenGL 4.0+.

Renders an articulated human figure built from a matrix stack — no deprecated `glRotate`/`glPushMatrix` allowed. Each body part is drawn by a single function call at the origin of the current matrix, with transformations applied via a custom matrix stack.

## Model

The figure consists of:
- Head
- Torso
- 2 arms (upper arm + forearm)
- 2 legs (thigh + lower leg)

Body parts are hierarchically linked — rotating the torso moves all limbs, moving the upper arm moves only the forearm, etc.

## Animations

- Walk
- Jump
- Idle (stay put)

## Dependencies

- OpenGL 4.0+
- GLEW
- GLFW

## Build

```sh
make        # build
make clean  # remove objects
make fclean # remove objects and binary
make re     # rebuild from scratch
```

## Usage

```sh
./humangl
```
