# 1.0.0-beta.1
- Fixed game crash on exiting and reentering without save.
- Fixed unnecessary skip deduction on skipping completed levels.
- Fixed giving skips while ratelimited.
- Changed nested `LevelInfoLayer` behaviour.
  - If a player goes to the creator's levels while in `ChallengeLevelInfoLayer`, and plays the current level (i.e. nested), deaths will count but the completion would not.
  - The skip button will not show in the nested `LevelInfoLayer`.
  - If the player completes the current level while in a nested layer with coins, then completes the level again in the main `ChallengeLevelInfoLayer` without coins, the extra lives will still be awarded.

# 1.0.0-beta
- Beta release.
