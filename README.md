# bds kb modifier
 Very simple knockback editor for BDS
 
 Can bring back W-Tapping like in 1.12 (depending on your setting) and edit how much knockback players/entities can take, disable/enable knockback

## Install
 Use a BDS DLL-injector like Element Minus

## How to Edit
- It should create new file called `kb_config.properties`. Edit the file and restart the server to apply changes

- Edit in Real-time: Import DLL `KBModifier.dll` and run any of the exported functions

```js
const { NativeModule } = require("bdsx");

// bdsx example
const knockback = new NativeModule("KBModifier.dll");

// void SetKnockbackMode(int mode)
const SetKnockbackMode = knockback.get("SetKnockbackMode");

SetKnockbackMode(0x0); // 0: decreased knockback (0.25)
SetKnockbackMode(0x1); // 1: default knockback (0.50)
SetKnockbackMode(0x2); // 2: increased knockback (0.75)
// refer to config
```
