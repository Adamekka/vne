<!-- v1 -->

# Main

> set rep_rena 10
> bg show background_images/village_evening.png fade 1.5s
> audio play music/orange-colored-time.mp3 loop

The cicadas are screaming over the rice fields.
The road back from the shrine is empty except for one familiar silhouette waiting by the drainage canal.

> sprite show left sprites/rena_smile.png fade 0.5s

**Rena**: "You're late. The others almost started the game without you."

> if rep_rena >= 5

[Laugh and greet her like usual](#warm-greet-rena)

> endif

[Ask why she is alone](#question-rena)

## warm-greet-rena

> sprite show left sprites/rena_happy.png

**Rena**: "Ahaha. That's the spirit. If everyone smiles, nothing bad can happen tonight."

Her laugh is bright, but it stops all at once.
Behind her, the shrine bell sways in the windless dusk.

> add rep_rena 1
> jmp [](#on-way-home)

## question-rena

> sprite show left sprites/rena_sad.png

**Rena**: "Alone?"

She tilts her head, still smiling.

**Rena**: "But you've been walking with someone this whole time."

## on-way-home

> sprite show left sprites/rena_smile.png
> bg show background_images/village_night.png fade 1.5s
> audio stop

Night falls too quickly.
The cicadas stop together, as if a hand closed around the village.
You keep walking, thumb rubbing the old festival coins in your pocket.
It seems like I have {coins} coins, but I only remember picking up one.

The streetlamp above you flickers out.

From the dark behind you, gravel crunches once.

> audio play sound_effects/bell.mp3
> jmp [](another-scenario.md)
