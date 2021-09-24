# pool
  ST7793 TFT LCD and Arduino UNO Pool sim/game

Requires Adafruit_GFX, Adafruit Touchscreen, and MCUFRIEND_kbv libraries
Adjust pin and point mapping in point.h

09/24/2021
Added game logic.

---The game now has 2 players (although it could be played alone)

---The game detects what type of ball was sunk first on break, and assigns that to the player.

---If the player does not make a shot, their turn is up.

---If the player sinks the wrong ball type, their turn is up.

---If the player scratches(sinks the cue ball), their turn is up and the other player can place the ball in the first third of the table.

---If the player sinks the 8 ball(black) before sinking all of their balls, they lose and the game restarts.

---If the player scratches when on the 8 ball, they lose and the game restarts.

Game loop has been adjusted a little bit, to alternate between players and not allow for next turn until all balls have stopped moving.

Aiming mechanic changed again. Now you don't have to touch the ball, you touch anywhere and a line is drawn from the ball to the point.
That point is also used to calculate launch ball. Not only do I think this works better, I was able to eliminate a sqrt call.

Also added a ghost ball when aiming, and an estimate line of how the shot may go, but it's not perfect as the ghost ball can overlap the real ball

Still some odd bugs with the touchscreen not registering sometimes, or registering incorrectly at other times.

Shot power is still off a little.

Friction might still be a little too strong.



09/22/2021

no real game logic yet, other than balls can be sunk and if the cue ball sinks it resets

I feel like I mostly got the ball collision angles right, after a day or so of constant unexpected or undesired results.

Still having slight issues with shot power and gauging it

Friction seems close but maybe a little too linear or a little too strong. By friction I mean the amount ball power/acceleration gets decreased by every frame


