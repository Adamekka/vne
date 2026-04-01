<!-- v1 -->

# Main

> bg set street_evening fade 1.5s
> audio play bgm_calm loop

The sun is setting. You see a familiar face approaching.

> sprite show bob happy left slide_in 0.5s ease_out

**Bob**: "Hey! Long time no see."

> if rep_bob >= 5

[Greet him warmly](#warm-greet-bob)

> endif

[Ignore him](#ignore-bob)

## warm-greet-bob

> sprite set bob surprised left

**Bob**: "Whoa, someone's in a good mood."

> inc rep_bob
> jmp [](#on-way-home)

## ignore-bob

> sprite set bob sad left

**Bob**: "Oh, I see how it is."

> jmp [](#on-way-home)

## on-way-home

> bg set street_night
> audio stop bgm_calm

The night falls as you continue on your way home, the encounter with Bob lingering in your thoughts.
You continue walking, counting your coins.
It seems like I have {coins} coins in my pocket.

The lights suddenly go out.

> wait 2.0s
> audio play se_crash
> jmp [](another-scenario.md)
