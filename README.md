# BDS Knockback Modifier
 Simple knockback editor for BDS
 
 Can bring back W-Tapping like in 1.12 (depending on your setting), edit how much knockback players/entities, etc

## Install
 Use a BDS DLL-injector like Element Minus

## How to Edit
- It should create new file called `kb_config.properties`. Edit the file and restart the server to apply changes

- Edit in Real-time: Import DLL `KBModifier.dll` and run any of the exported functions

## Features

### Knockback
 The knockback you deal without sprinting, just normally

```js
// BDSX Example:
const { NativeModule } = require("bdsx");
const knockback = new NativeModule("KBModifier.dll");
const SetKnockback = knockback.get("SetKnockback");
SetKnockback(0x2); // 0.75
```

### Sprint Knockback
 The knockback addition that gets dealed when you are sprinting or w-tapping

```js
// BDSX Example:
const { NativeModule } = require("bdsx");
const knockback = new NativeModule("KBModifier.dll");
// void SetKnockbackMode(int mode)
const SetSprintKnockback = knockback.get("SetSprintKnockback");
SetSprintKnockback(0x2); // 0.75
```

### Hurt Time
 The cooldown between each hit can be edited (the default is 10)

### Entity Collision
 A way to turn on and off global entity collision/pushing

## Version

BDS Version 1.16.200.2