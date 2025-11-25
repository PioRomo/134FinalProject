# 134 Final Project README! 
#### Pio Romo, Schuyler Ng aka the Super Mario Bros

##### Currently the terrain .obj file is too large to store on github.
## Requirements and How To's 
#### LEM movement and rotation
###### Schulyer Ng
This allows us to move the LEM left, right, up, down, and even rotate. 
> The 'A' and 'D' key allows you to rotate left and right, respectively.
> The 'W' and 'S' key allows you to move left and right, respectively.
> The 'Shift' key moves the lander upwards. The 'CTRL' key moves it down. 
#### AGL detection using ray intersection
###### Pio Romo
This one is very simple. What this allows us to do is show AGL statistics from a given altitude on the screen. 
Additionally, we've added a ray from the lander to the ground to help better visualize.
> To enable this, simple press the 'G' button to toggle on and off. AGL stats will be on the top right of the
> window.
#### Particle Emitting and Rendering
###### Pio Romo
One of the more frustrating requirements, we did have previous code to work off of. However, that was for 2D; 3D
is whole 'nother world. We built a particle system positioned at the thrust (bottom  of the lander) that begins 
emitting particles when thrust is activated i.e. movement and rotation. Makes for realistic exhaust!   
> Thrust is activated on movement key press: W, A, S, D, Shift, Ctrl
#### Collision Detection 
##### Schuyler Ng
We have implemented full AABB collision with octrees. All contact between the terrain and the lander are handled.
Collision resolution was also implemented with a reflective impulse force when the lander comes in contact with the terrain.
Upon colliding with terrain too quickly, the lander will explode with a shader explosion.
#### Camera
##### Schuyler Ng
We have 3 cameras implemented
> Pressing "1" uses the ofEasyCam which allows the user to manually move the camera around as they would like.
> Pressing "2" uses the chase camera which sits above the terrain and automatically tracks and zooms on the lander.
> Pressing "3" uses the down camera which stays attached to the lander and faces straight down.
#### Lighting 
###### Pio Romo
Lighting was added to our scene. Very simple implementation taken from the monsterLight code. Additionally, a ship light was 
added. 
> Ship light can be toggled on and off with the 'P' key
#### Sound
Sounds were added: thrusting, background and explosions. 
#### Dragging the Lander
#### 2D Background Image
###### Pio Romo
Starfield image gives the game a more "gamey" feel! 
