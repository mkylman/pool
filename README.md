# pool
  ST7793 TFT LCD and Arduino UNO Pool sim/game

Requires Adafruit_GFX, Adafruit Touchscreen, and MCUFRIEND_kbv libraries
Adjust pin and point mapping in point.h


https://www.youtube.com/watch?v=HvlHeZh_OiI&list=PLKe_-NZKlS9rj8NIvXMHUFk0JC5LJtnIf


09/28/2021

Changed shot power mechanic. Now, instead of holding down while power builds, we can touch and drag to adjust shot power, and release to shoot.
The distance between the initial and final touch points is used to determine shot power. This is much faster, in terms of gameplay pace.
Friction is still a little wonky.

09/27/2021

Tweaked the velocity, power(acceleration), and friction parameters by figuring out the m/s to px/s conversion.
Based on a 7ft pool table vs the number of pixels on the screen, then made power range from 200cm/s to 1000cm/s, which is roughly the range of pool ball speeds.
This power gets divided by the tick and then multiplied by velocity in moveBall(). Power is adjusted once every tick as well, just subtracting 10 each tick.
Movement is smooth, excluding on break when all balls are moving.

Added bank snapping and aim estimation with edge collision, so bank shots are drawn if pointer is at the edge.
Estimate line seems more accurate with the frame rate, power/acceleration, and friction adjustments, and some of the weird "warping" collision detection issues
that were happening before have improved or been resolved.

09/26/2021
Added some more game logic

---if you sink the 8 on break, you win

---if you sink an opponent's ball when on the 8, you lose


Added ghost ball aim snapping/aim assist/fine aim. Basically, the ghost ball snaps to a target ball and unsnaps when you pull far enough away,
allowing for finer aim on the target ball. Again, an estimate line is drawn but it's not 100% accurate. There's some comments in the code about it in the
aimCue() function in ball.h

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

Aiming mechanic changed (again).

---Now you don't have to touch the ball, you touch anywhere and a line is drawn from the ball to the point.

---That point is used in launchBall() calculation. Works better and is less code.

---Added ghost ball. If ghost collides with a ball an estimate line of how the shot may go is drawn. It's not perfect as the ghost can overlap the real.



Still some odd bugs with the touchscreen not registering sometimes, or registering incorrectly at other times.

Shot power is still off a little.

Friction might still be a little too strong.



09/22/2021

no real game logic yet, other than balls can be sunk and if the cue ball sinks it resets

I feel like I mostly got the ball collision angles right, after a day or so of constant unexpected or undesired results.

Still having slight issues with shot power and gauging it

Friction seems close but maybe a little too linear or a little too strong. By friction I mean the amount ball power/acceleration gets decreased by every frame


