# SDLIQUID

Another SDL fluid simulation.

## What ?

This is a simple dev-playground for fluid simulation / C rendering.

## How ? 

Install requirements ; 

```sh
sudo apt install -y libsdl2-dev libsdl2-ttf-dev
```
Build with ;

```sh
git clone https://github.com/Retr0Kr0dy/sdliquid
cd sdliquid
make
./sdliquid
```
#### Usage

To set custom resolution, simply pass width and height as arguments (default is 640x480) ;

```sh
./sdliquid 1920 1080
```

You can also modify gravity, radius and more using your keyboard.

- **Q** : Quit
- **A** : Add ball
- **U** : Increase gravity
- **J** : Decrease gravity
- **I** : Increase ball radius
- **K** : Decrease ball radius
- **O** : Increase friction
- **L** : Decrease friction

You can also click on the screen to drag particles.

## Why ?

why not.

stop asking weird questions.


