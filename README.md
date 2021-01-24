# Live thumbnails of other windows

This is a tiny proof of concept to show how you can create live DWM thumbnails
of other programs.

Run it by providing the window title of the window you want to watch

```
thumb.exe Untitled - Notepad
```

Note that no quotes are provided, this interprets the entirety of the 
arguments as a single string (because that was easiest).

When the window opens, it will be half the size of the window you are
thumbnailing, but it can be resized (which easily breaks the aspect ratio).
Scroll with the mouse wheel to zoom in and out of areas.

Inspired by [@DamianEdwards tweet](https://twitter.com/DamianEdwards/status/1353086983975825409).
Quite effective nerd-sniping there.

## Known limitations

This is a proof of concept, so it will likely break in every possible way:

- If the source or destination window is resized, it will probably break 
  the aspect ratio.
- When you zoom in and out, you'll get Windows 10's really nice invisible
  window borders included in the area.
  
## Suggested improvements

- Allowing click-through. Congratulations, you've invented a different Magnifier
  tool.
- Window selector instead of weird input syntax.
- Options to toggle always-on-top.