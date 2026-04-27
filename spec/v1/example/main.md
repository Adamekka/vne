<!-- v1 -->

# Main

> bg show street_evening.png fade 1.5s
> audio play bgm_calm.ogg loop

The sun is setting. You see a familiar face approaching.

> sprite show left bob_happy.png slide_in_left 0.5s

**Bob**: "Hey! Long time no see."

> if rep_bob >= 5

[Greet him warmly](#warm-greet-bob)

> endif

[Ignore him](#ignore-bob)

## warm-greet-bob

> sprite show left bob_surprised.png

**Bob**: "Whoa, someone's in a good mood."

> add rep_bob 1
> jmp [](#on-way-home)

## ignore-bob

> sprite show left bob_sad.png

**Bob**: "Oh, I see how it is."

## on-way-home

> bg show street_night.png fade 1.5s
> audio stop

The night falls as you continue on your way home, the encounter with Bob lingering in your thoughts.
You continue walking, counting your coins.
It seems like I have {coins} coins in my pocket.

The lights suddenly go out.

> wait 2.0s
> audio play se_crash.ogg
> jmp [](another-scenario.md)
